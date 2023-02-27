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

	private:
		Entity m_entity;
	private:
		friend class Scene;
	};
}