#pragma once

#include "entt/entt.hpp"
#include "Mint/Core/UUID.h"
#include "Components.h"

MT_NAMESPACE_BEGIN
class Entity
{
public:
	Entity();
	Entity(const Entity& other) = default;
	Entity(const Ref<entt::registry>& p_registry);
	Entity(const Ref<entt::registry>& p_registry, const entt::entity& p_entity);

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		MT_ASSERT(!HasComponent<T>(), "Entity already has component!");
		return m_registry->emplace<T>(m_entity, std::forward<Args>(args)...);
	}

	template<typename T>
	T& GetComponent()
	{
		MT_ASSERT(HasComponent<T>(), "Entity does not has component!");
		return m_registry->get<T>(m_entity);
	}

	template<typename T>
	bool HasComponent()
	{
		return m_registry->has<T>(m_entity);
	}

	template<typename T>
	void RemoveComponent()
	{
		m_registry->remove<T>(m_entity);
	}


	bool operator==(const Entity& p_entity) const;
	bool operator!=(const Entity& p_entity) const;
	Entity& operator=(const Entity& p_entity);
	Ref<Entity>& operator=(const Ref<Entity>& p_entity);

	operator bool() const { return m_entity != entt::null; }
	operator entt::entity() const { return m_entity; }
	operator uint32_t() const { return static_cast<uint32_t>(m_entity);}

	UUID GetUUID() { return GetComponent<IDComponent>().ID; }
	const std::string& GetName() { return GetComponent<TagComponent>().Tag; }

private:
	Ref<entt::registry> m_registry;
	entt::entity m_entity;
};
MT_NAMESPACE_END