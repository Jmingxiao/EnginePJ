#pragma once

#include "Scene.h"
#include "entt.hpp"
#include "Mint/Core/UUID.h"
#include "Components.h"
#include "Mint/Physics/PhysicsManger.h"

namespace Mint {

	class Entity
	{
	public:
		Entity() = default;
		Entity(const Entity& other) = default;
		Entity(entt::entity handle, Scene* scene);


		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			MT_ASSERT(!HasComponent<T>(), "Entity already has component!");
			T& component = m_scene->m_registry.emplace<T>(m_entity, std::forward<Args>(args)...);
			m_scene->OnComponentAdded<T>(*this, component);
			return component;;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = m_scene->m_registry.emplace_or_replace<T>(m_entity, std::forward<Args>(args)...);
			m_scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			MT_ASSERT(HasComponent<T>(), "Entity does not has component!");
			return m_scene->m_registry.get<T>(m_entity);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_scene->m_registry.has<T>(m_entity);
		}

		template<typename T>
		void RemoveComponent()
		{
			m_scene->m_registry.remove<T>(m_entity);
		}


		bool operator==(const Entity& p_entity) const;
		bool operator!=(const Entity& p_entity) const;

		operator bool() const { return m_entity != entt::null; }
		operator entt::entity() const { return m_entity; }
		operator uint32_t() const { return static_cast<uint32_t>(m_entity); }

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		const std::string& GetName() { return GetComponent<TagComponent>().Tag; }

		virtual void OnContact(Entity other, OnCollisionType type){}
		virtual void OnTrigger(Entity other, OnCollisionType type){ }

	private:
		entt::entity m_entity{ entt::null };
		Scene* m_scene{ nullptr };
	};
}