#include "pch.h"
#include "Entity.h"

MT_NAMESPACE_BEGIN

Entity::Entity(entt::entity handle, Scene* scene)
	: m_entity(handle), m_scene(scene)
{
}

bool Entity::operator==(const Entity& entity) const
{
	return m_entity == entity.m_entity&&m_scene==entity.m_scene;
}

bool Entity::operator!=(const Entity& entity) const
{
	return !(*this == entity);
}


MT_NAMESPACE_END