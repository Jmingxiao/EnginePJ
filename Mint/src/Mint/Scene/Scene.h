#pragma once

#include<entt/entt.hpp>
#include "Mint/Core/TimeStep.h"
#include "Components.h"
#include "Mint/Core/UUID.h"
#include "Entity.h"

MT_NAMESPACE_BEGIN

class Scene
{
public:
	Scene();
	~Scene();

	static Ref<Scene> Copy(Ref<Scene> other);


	Entity CreateEntity(const std::string& name = std::string());
	Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
	void DestroyEntity(const Entity spEntity);

	void OnUpdate(const Timestep& timeStep);
	void OnViewPortResize(uint32_t uiWidth, uint32_t uiHeight);
	const Ref<entt::registry>& Registry()const;

	Entity FindEntityByName(std::string_view name);
	Entity GetEntityByUUID(UUID uuid);


private:
	
	bool m_IsRunning = false;
	bool m_IsPaused = false;
	uint32_t m_width =0,m_height=0;
	Ref<entt::registry> m_registry;
	std::unordered_map<UUID, entt::entity> m_entitymap;
};

MT_NAMESPACE_END