#pragma once

#include "Mint/Scene/Entity.h"


namespace Mint{

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T>
		T& GetComponent()
		{
			return m_entity.GetComponent<T>();
		}

	protected:
		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void OnUpdate(Timestep ts);
		virtual void OnContact(Entity other, OnCollisionType type);
		virtual void OnTrigger(Entity other, OnCollisionType type);
	private:
		Entity m_entity;
	private:
		friend class Scene;
		friend class PhysicsManager;
	};
}