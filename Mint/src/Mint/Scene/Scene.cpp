#include "pch.h"
#include "Scene.h"
#include "Components.h"
#include "Entity.h"
#include "Mint/Util/React3dUtil.h"
#include "Mint/Physics/PhysicsTimer.h"
#include "Mint/Render/Renderer3D.h"
#include "ScriptableEntity.h"
#include "Mint/Scripting/ScriptEngine.h"

MT_NAMESPACE_BEGIN

static rp3d::BodyType RigidbodyTypeToRp3dBody(RigidBodyComponent::BodyType bodyType)
{
	switch (bodyType)
	{
	case RigidBodyComponent::BodyType::Static:    return rp3d::BodyType::STATIC;
	case RigidBodyComponent::BodyType::Dynamic:   return rp3d::BodyType::DYNAMIC;
	case RigidBodyComponent::BodyType::Kinematic: return rp3d::BodyType::KINEMATIC;
	}

	MT_ASSERT(false, "Unknown body type");
	return rp3d::BodyType::STATIC;
}


Scene::Scene()
{
	MT_INFO("Scene Creat");
	m_phystimer = CreatePtr<PhysicsTimer>();
}

Scene::~Scene()
{
	MT_INFO("Scene Destroy");
}

template<typename... Component>
static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
{
	([&]()
		{
			auto view = src.view<Component>();
	for (auto srcEntity : view)
	{
		entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

		auto& srcComponent = src.get<Component>(srcEntity);
		dst.emplace_or_replace<Component>(dstEntity, srcComponent);
	}
		}(), ...);
}

template<typename... Component>
static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
{
	CopyComponent<Component...>(dst, src, enttMap);
}

template<typename... Component>
static void CopyComponentIfExists(Entity dst, Entity src)
{
	([&]()
		{
			if (src.HasComponent<Component>())
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}(), ...);
}

template<typename... Component>
static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
{
	CopyComponentIfExists<Component...>(dst, src);
}

Ref<Scene> Scene::Copy(Ref<Scene> other)
{
	Ref<Scene> newScene = CreateRef<Scene>();

	newScene->m_width = other->m_width;
	newScene->m_height = other->m_height;

	auto& srcSceneRegistry = other->m_registry;
	auto& dstSceneRegistry = newScene->m_registry;
	std::unordered_map<UUID, entt::entity> enttMap;

	// Create entities in new scene
	auto idView = srcSceneRegistry.view<IDComponent>();
	for (auto e : idView)
	{
		UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
		const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
		Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
		enttMap[uuid] = (entt::entity)newEntity;
	}

	// Copy components (except IDComponent and TagComponent)
	CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

	return newScene;
}

Entity Scene::CreateEntity(const std::string& name)
{
	return CreateEntityWithUUID(UUID(), name);
}

Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
{
	Entity entity = { m_registry.create(), this };
	entity.AddComponent<IDComponent>(uuid);
	entity.AddComponent<TransformComponent>();
	auto& tag = entity.AddComponent<TagComponent>();
	tag.Tag = name.empty() ? "Entity" : name;

	m_entitymap[uuid] = entity;

	return entity;
}

void Scene::DestroyEntity(Entity entity)
{
	if (entity.HasComponent<RigidBodyComponent>()) {
		auto& rb = entity.GetComponent<RigidBodyComponent>();
		m_physicsManager.GetWorld()->destroyRigidBody(rb.m_rigidbody);
	}
	if (entity.HasComponent<CollisionBodyComponent>()) {
		auto& cb = entity.GetComponent<CollisionBodyComponent>();
		m_physicsManager.GetWorld()->destroyCollisionBody(cb.m_collisionbody);
	}
	m_entitymap.erase(entity.GetUUID());
	m_registry.destroy(entity);
}

void Scene::OnRuntimeStart()
{
	m_IsRunning = true;

	OnPhysicsStart();


}
void Scene::OnRuntimeStop()
{
	m_IsRunning = false;

	OnPhysicsStop();
}
void Scene::OnSimulationStart()
{
	OnPhysicsStart();
}

void Scene::OnSimulationStop()
{
	OnPhysicsStop();
}

void Scene::OnUpdate(const Timestep& timeStep)
{
	if (!m_IsPaused || m_StepFrames-- > 0)
	{
		// Update scripts
		{

			m_registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
				{
					// TODO: Move to Scene::OnScenePlay
					if (!nsc.Instance)
					{
						nsc.Instance = nsc.InstantiateScript();
						nsc.Instance->m_entity = Entity{ entity, this };
						nsc.Instance->OnCreate();
					}

					nsc.Instance->OnUpdate(timeStep);
				});
		}

		// Physics
		OnphysicsUpdate();
	}

	// Render 3D
	Camera* mainCamera = nullptr;
	glm::mat4 cameraTransform;
	{
		auto view = m_registry.view<TransformComponent, CameraComponent>();
		for (auto entity : view)
		{
			auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

			if (camera.Primary)
			{
				mainCamera = &camera.Camera;
				cameraTransform = transform.GetTransform();
				break;
			}
		}
	}

	if (mainCamera)
	{
		Renderer3D::BeginScene(*mainCamera, cameraTransform);

		// Draw model;
		{
			auto group = m_registry.group<TransformComponent>(entt::get<MeshRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, model] = group.get<TransformComponent, MeshRendererComponent>(entity);
				Renderer3D::DrawModel(transform.GetTransform(), model, (int)entity);
			}
		}

		Renderer3D::EndScene();
	}
}

void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& camera)
{
	if (!m_IsPaused || m_StepFrames-- > 0)
	{	
		OnphysicsUpdate();
	}
	RenderScene(camera);
}

void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
{
	RenderScene(camera);
}
void Scene::RenderScene(EditorCamera& camera)
{
	Renderer3D::BeginScene(camera);
	{
		Renderer3D::DrawBackground();
		auto group = m_registry.group<TransformComponent>(entt::get<MeshRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, model] = group.get<TransformComponent, MeshRendererComponent>(entity);

			Renderer3D::DrawModel(transform.GetTransform(), model, (int)entity);
		}
	}
	Renderer3D::EndScene();
}

void Scene::OnPhysicsStart()
{

	m_physicsManager.Init();

	auto rbview = m_registry.view<RigidBodyComponent>();
	for (auto e : rbview) {

		Entity entity = { e,this };
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rb = entity.GetComponent<RigidBodyComponent>();
		
		rb.m_rigidbody = m_physicsManager.CreateRigidBody(transform, entity);
		rb.m_rigidbody->setType(RigidbodyTypeToRp3dBody(rb.type));

		if (entity.HasComponent<ColliderComponent>()) {
			auto& collider = entity.GetComponent<ColliderComponent>();
			collider.m_collider = m_physicsManager.CreateCollider(rb.m_rigidbody, collider.m_geometry, transform.Scale);
		}
	}

	auto cbview = m_registry.view<CollisionBodyComponent>();
	for (auto e : cbview) {

		Entity entity = { e,this };
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& cb = entity.GetComponent<CollisionBodyComponent>();
		
		cb.m_collisionbody = m_physicsManager.CreateCollisionBody(transform, entity);

		if (entity.HasComponent<ColliderComponent>()) {
			auto& collider = entity.GetComponent<ColliderComponent>();
			collider.m_collider = m_physicsManager.CreateCollider(cb.m_collisionbody, collider.m_geometry, transform.Scale);
		}

	}

}

void Scene::OnPhysicsStop()
{
	m_physicsManager.Destroy();
}

void Scene::OnphysicsUpdate()
{
	m_phystimer->Update();
	while (m_phystimer->GetEnoughtime())
	{
		m_physicsManager.UpdateWorld(m_phystimer->m_timestep);
		m_phystimer->UpdateAccumulator();
	}
	m_physicsManager.UpdateContactedEntity();
	m_physicsManager.UpdateTriggeredEntity();

	auto view = m_registry.view<RigidBodyComponent>();
	for (auto e : view)
	{
		Entity entity = { e, this };
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rb = entity.GetComponent<RigidBodyComponent>();

		rp3d::Transform prevTrans = toTrans(transform);
		rp3d::Transform currentTrans = rb.m_rigidbody->getTransform();
		rp3d::Transform interpolatedTrans = rp3d::Transform::
			interpolateTransforms(prevTrans, currentTrans, m_phystimer->Getfactor());

		transform.Translation = toVec3(interpolatedTrans.getPosition());
		transform.Rotation = toQuat(interpolatedTrans.getOrientation());
	}
	auto view0 = m_registry.view<CollisionBodyComponent>();
	for (auto e : view0)
	{
		Entity entity = { e, this };
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& cb = entity.GetComponent<CollisionBodyComponent>();

		cb.m_collisionbody->setTransform(toTrans(transform));
	}
}

void Scene::Step(int frames)
{
	m_StepFrames = frames;
}

void Scene::OnViewportResize(uint32_t width, uint32_t height)
{
	if (m_width == width && m_height == height)
		return;

	m_width = width;
	m_height = height;

	// Resize our non-FixedAspectRatio cameras
	auto view = m_registry.view<CameraComponent>();
	for (auto entity : view)
	{
		auto& cameraComponent = view.get<CameraComponent>(entity);
		if (!cameraComponent.FixedAspectRatio)
			cameraComponent.Camera.SetViewportSize(width, height);
	}
}

Entity Scene::FindEntityByName(std::string_view name)
{
	auto view = m_registry.view<TagComponent>();
	for (auto entity : view)
	{
		const TagComponent& tc = view.get<TagComponent>(entity);
		if (tc.Tag == name)
			return Entity{entity,this };
	}
	return {};
}

Entity Scene::GetEntityByUUID(UUID uuid)
{
	if (m_entitymap.find(uuid) != m_entitymap.end())
		return { m_entitymap.at(uuid),this };

	return {};
}

Entity Scene::GetMainCameraEntity()
{
	auto view = m_registry.view<CameraComponent>();
	for (auto entity : view)
	{
		const auto& camera = view.get<CameraComponent>(entity);
		if (camera.Primary)
			return Entity{ entity, this };
	}
	return {};
}


template<typename T>
void Scene::OnComponentAdded(Entity entity, T& component)
{
	static_assert(sizeof(T) ==0);
}

template<>
void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
{
}

template<>
void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
{
}

template<>
void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
{
}

template<>
void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
{
	if (m_width > 0 && m_height > 0)
		component.Camera.SetViewportSize(m_width, m_height);
}
template<>
void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
{
}
template<>
void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
{
}
template<>
void Scene::OnComponentAdded<RigidBodyComponent>(Entity entity, RigidBodyComponent& component)
{

}

template<>
void Scene::OnComponentAdded<CollisionBodyComponent>(Entity entity, CollisionBodyComponent& component)
{

}

template<>
void Scene::OnComponentAdded<ColliderComponent>(Entity entity, ColliderComponent& component)
{

}

template<>
void Scene::OnComponentAdded<MeshRendererComponent>(Entity entity, MeshRendererComponent& component)
{

}
template<>
void Scene::OnComponentAdded<MusicComponent>(Entity entity, MusicComponent& component)
{

}

MT_NAMESPACE_END