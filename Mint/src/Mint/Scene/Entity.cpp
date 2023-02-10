#include "pch.h"
#include "Entity.h"

MT_NAMESPACE_BEGIN


Entity::Entity()
{
	m_registry = CreateRef<entt::registry>();
	m_entity = m_registry->create();
}

Entity::Entity(const Ref<entt::registry>& p_registry)
	:m_registry(p_registry)
{
	m_entity = m_registry->create();
}

Entity::Entity(const Ref<entt::registry>& p_registry, const entt::entity& p_entity)
	:m_registry(p_registry), m_entity(p_entity)
{
}

bool Entity::operator==(const Entity& entity) const
{
	return m_entity == entity.m_entity;
}

bool Entity::operator!=(const Entity& entity) const
{
	return !(*this == entity);
}

Entity& Entity::operator=(const Entity& spEntity)
{
	m_entity = spEntity.m_entity;
	m_registry = spEntity.m_registry;
	return *this;
}

Ref<Entity>& Entity::operator=(const Ref<Entity>& spEntity)
{
	m_entity = spEntity->m_entity;
	m_registry = spEntity->m_registry;
	return std::move(Ref<Entity>(this));
}

MT_NAMESPACE_END