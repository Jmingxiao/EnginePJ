#pragma once

#include "Mint/Core/TimeStep.h"
#include "Mint/Core/UUID.h"
#include "Mint/Render/Camera/EditorCamera.h"
#include "Components.h"
#include "entt.hpp"
#include "Mint/Physics/PhysicsManger.h"


MT_NAMESPACE_BEGIN

class Entity;
class PhysicsTimer;

class Scene
{
public:
	Scene();
	~Scene();

	static Ref<Scene> Copy(Ref<Scene> other);


	Entity CreateEntity(const std::string& name = std::string());
	Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
	void DestroyEntity(const Entity spEntity);


	void OnRuntimeStart();
	void OnRuntimeStop();

	void OnSimulationStart();
	void OnSimulationStop();

	void OnUpdate(const Timestep& timeStep);
	void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
	void OnUpdateEditor(Timestep ts, EditorCamera& camera);
	void OnViewportResize(uint32_t width, uint32_t height);

	void DuplicateEntity(Entity entity);

	Entity FindEntityByName(std::string_view name);
	Entity GetEntityByUUID(UUID uuid);
	Entity GetMainCameraEntity();

	bool IsRunning() const { return m_IsRunning; }
	bool IsPaused() const { return m_IsPaused; }

	void SetPaused(bool paused) { m_IsPaused = paused; }

	void Step(int frames = 1);

	template<typename... Components>
	auto GetAllEntitiesWith()
	{
		return m_registry.view<Components...>();
	}

private:
	template<typename T>
	void OnComponentAdded(Entity entity, T& component);

	void OnPhysicsStart();
	void OnPhysicsStop();
	void OnPhysicsUpdate();

	void OnAudioStart();
	void OnAudioStop();
	void OnAudioUpdate();
	void RenderScene(EditorCamera& camera);

private:
	
	bool m_IsRunning = false;
	bool m_IsPaused = false;
	int m_StepFrames = 0;
	uint32_t m_width =0,m_height=0;
	entt::registry m_registry;
	std::unordered_map<UUID, entt::entity> m_entitymap;

	Ptr<PhysicsTimer> m_phystimer;

	PhysicsManager m_physicsManager;

	friend class Entity;
	friend class SceneSerializer;
	friend class SceneHierarchyPanel;
};



MT_NAMESPACE_END

