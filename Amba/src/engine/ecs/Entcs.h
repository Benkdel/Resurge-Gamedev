#pragma once

#include <glm/glm.hpp>

// definitions
#define MAX_ENTITIES 1025
#define INVALID_ENTITY CreateEntityId(EntityIndex(-1), 0)

// typedefs to increase readability
typedef unsigned long long EntityId;
typedef unsigned int EntityIndex;
typedef unsigned int EntityVersion;

extern int s_CompotentCounter;

template<class t>
int GetComponentId()
{
	static int s_ComponentId = s_CompotentCounter++;
	return s_ComponentId;
}

EntityId CreateEntityId(EntityIndex index, EntityVersion version);
EntityIndex GetEntityIndex(EntityId id);
EntityVersion GetEntityVersion(EntityId id);
bool IsEntityValid(EntityId id);


class ComponentPool
{
public:
	
	ComponentPool(size_t elementSize);
	~ComponentPool();
	
	inline void* get(size_t index) { return m_pData + index * m_ElementSize; }

	char* m_pData;
	size_t m_ElementSize;

private:

};




/* ======================
	COMPONENTS 
====================== */

class MeshComponent
{

};

class TransformComponent
{
public:
	glm::vec3 Position;
	glm::vec3 Scale;
	glm::vec3 rotation;
	float angle;
};

class RendererComponent
{

};

class CollisionComponent
{

};

class AudioComponent
{

};

class BioAttributes
{

};