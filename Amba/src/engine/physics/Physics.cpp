#include "Physics.h"

#include <engine/scene/Scene.h>

namespace Amba {

	Physics::Physics()
	: m_Gravity(glm::vec3(0.0f, -9.8f, 0.0f)),
		m_IsGravityOn(false)
	{
		// init spatial grid
		p_SpatialGrid = new Spatial2DGrid();
	}

	Physics::~Physics()
	{
		AB_WARN("Physics object destructor called");
	}

	void Physics::ApplyMotion(Scene* scene, float dt)
	{
		for (EntityId ent : EntityGroup<TransformComponent, PhysicsComponent>(scene->p_EntHandler))
		{
			scene->GetComponent<PhysicsComponent>(ent)
						->Integrate(scene->GetComponent<MeshComponent>(ent),
									scene->GetComponent<TransformComponent>(ent), dt);
		}
	}

	void Physics::SolveCollisions(Scene* scene)
	{
		for (EntityId ent : EntityGroup<TransformComponent, MeshComponent>(scene->p_EntHandler))
		{
			IntersectData intersect = CheckForCollision(ent, scene);
			
			if (intersect.GetDoesIntersect())
			{
				//AB_INFO("COLLISION - Entity: {0} with Entity {1} || Dist: {2} ", ent, intersect.GetOtherEnt(), intersect.GetDistance());
				//AB_INFO("DIRECTION: {0}, {1}, {2}", intersect.GetDirection().x, intersect.GetDirection().y, 
				//	intersect.GetDirection().z);

				// lets try momentum equations
				// momentum is conserved assuming there is no friction
				// elastic collision
				
				glm::vec3 entV0 = scene->GetComponent<PhysicsComponent>(ent)->GetVelocity();
				glm::vec3 otherEntV0 = scene->GetComponent<PhysicsComponent>(intersect.GetOtherEnt())->GetVelocity();
			
				float entMass = scene->GetComponent<PhysicsComponent>(ent)->m_Mass;
				float otherEntMass = scene->GetComponent<PhysicsComponent>(intersect.GetOtherEnt())->m_Mass;

				glm::vec3 relVelocity = entV0 - otherEntV0;

				// I know objects will collide in X and normal vector pointing to ent is x negative
				// implement getting real normal from IntersectData
				glm::vec3 normal = intersect.GetCollisionNorm();

				float e = 1.0f; // 100% elastic

				float numerator		= glm::dot(- 1 * (1 + e) * relVelocity, normal);
				float denominator	= glm::dot(normal, normal * (1 / entMass + 1 / otherEntMass)); // 1 instead of dot normal normal, because is the same when unit length?

				float impulse = numerator / denominator;

				scene->GetComponent<PhysicsComponent>(ent)->ApplyCollisionResults(impulse, normal, intersect.GetOtherEnt());
				scene->GetComponent<PhysicsComponent>(intersect.GetOtherEnt())->ApplyCollisionResults(-impulse, normal, ent);
			
			}
		}
	}

	IntersectData Physics::CheckForCollision(EntityId id, Scene* scene)
	{
		int componentID = GetColliderTypeIndex(id, scene);
		
		if (componentID < 0)
			return IntersectData(false, glm::vec3(0.0f), glm::vec3(0.0f));
	
		ComponentMask entMask = scene->p_EntHandler->m_Entities[
			scene->p_EntHandler->GetEntityIndex(id)].mask;

		if (!entMask.test(componentID))
			return IntersectData(false, glm::vec3(0.0f), glm::vec3(0.0f));

		TransformComponent* tsr = scene->GetComponent<TransformComponent>(id);

		GridCell gridCell = p_SpatialGrid->GetGridCell(tsr->GetPosition());
		if (!gridCell.IsCellValid())
			return IntersectData(false, glm::vec3(0.0f), glm::vec3(0.0f));

		Cell& cell = gridCell.GetCell();
		ColliderComponent* collider = scene->GetComponentWithId<ColliderComponent>(id, componentID);

		// update collider position (center)
		collider->TransformCollider(tsr);

		// check entities in cell
		for (auto& other : cell.entities)
		{
			if (id != other)
			{
				// check if other entity has collider component
				ComponentMask otherMask = scene->p_EntHandler->m_Entities[
					scene->p_EntHandler->GetEntityIndex(other)].mask;

				int otherComponentID = GetColliderTypeIndex(other, scene);

				if (otherMask.test(otherComponentID))
				{
					ColliderComponent* otherCollider = scene->GetComponentWithId<ColliderComponent>(other, otherComponentID);
					IntersectData intersect = collider->Intersect(*otherCollider, other);

					if (intersect.GetDoesIntersect())
					{
						return intersect;
					}
				}
			}
		}

		// check planes
		for (EntityId plane : EntityGroup<PlaneCollider, PhysicsComponent>(scene->p_EntHandler))
		{
			int otherComponentID = GetColliderTypeIndex(plane, scene);

			ColliderComponent* otherCollider = scene->GetComponentWithId<ColliderComponent>(plane, otherComponentID);
			IntersectData intersect = collider->Intersect(*otherCollider, plane);

			if (intersect.GetDoesIntersect())
			{
				return intersect;
			}
		}

		return IntersectData(false, glm::vec3(0.0f), glm::vec3(0.0f));
	}
}



