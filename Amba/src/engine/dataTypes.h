#pragma once

// TODO: change file name to something more meaninfull

#include <engine/importer/Importer.h>

// typedefs 
typedef unsigned long long EntityId;
typedef unsigned int EntityIndex;
typedef unsigned int EntityVersion;

// definitions

#define BOX_COLLIDER_SIZE 5.0f
#define PI_VALUE 3.141592653589793

#define NEAR_PLANE 0.1f
#define FAR_PLANE 100.f

// only for colliders
class IntersectData
{
public:

	IntersectData(const bool doesIntersect, const float distance) :
		m_DoesIntersect(doesIntersect), m_Distance(distance) {}

	inline bool GetDoesIntersect() const { return m_DoesIntersect; };
	inline float GetDistance() const { return m_Distance; };
private:

	const bool m_DoesIntersect = false;
	const float m_Distance;
};


namespace Amba {
	
	// data types

	class Cell
	{
	public:
		Cell() {};

		~Cell() {};

		std::vector<EntityId> entities; // entities in current cell
		
		// corners of cell
		glm::vec3 bottomLeft;
		glm::vec3 topLeft;
		glm::vec3 topRight;
		glm::vec3 bottomRight;
		
		//std::vector<glm::vec3> vertices; // corners of the cell

	};

	struct Rotation {
		float angle;
		glm::vec3 vec3;
	};

	struct Vertex {
		glm::vec3 v_Position;
		glm::vec3 v_Normals;
		glm::vec2 v_TexCoords;
	};
}

