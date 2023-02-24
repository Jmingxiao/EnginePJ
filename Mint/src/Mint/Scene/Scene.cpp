#include "pch.h"
#include "Scene.h"
#include "Components.h"
#include "Entity.h"
#include"Mint/Util/React3dUtil.h"
#include "Mint/Physics/PhysicsTimer.h"
#include "Mint/Render/Renderer3D.h"

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
	Renderer3D::Init();
}

Scene::~Scene()
{
	MT_INFO("Scene Destroy");
}

Ref<Scene> Scene::Copy(Ref<Scene> other)
{
	return Ref<Scene>();
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
		m_world->destroyRigidBody(rb.m_rigidbody);
	}
	if (entity.HasComponent<CollisionBodyComponent>()) {
		auto& cb = entity.GetComponent<CollisionBodyComponent>();
		m_world->destroyCollisionBody(cb.m_collidionbody);
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
			
		}

		// Physics
		{

			m_phystimer->Update();
			while (m_phystimer->GetEnoughtime())
			{
				m_world->update(m_phystimer->m_timestep);
				m_phystimer->UpdateAccumulator();
			}
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
		}
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
		//physics 
		{
			m_phystimer->Update();
			while (m_phystimer->GetEnoughtime())
			{
				m_world->update(m_phystimer->m_timestep);
				m_phystimer->UpdateAccumulator();
			}
			auto view = m_registry.view<RigidBodyComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb = entity.GetComponent<RigidBodyComponent>();
				
				rp3d::Transform prevTrans = toTrans(transform);
				rp3d::Transform currentTrans= rb.m_rigidbody->getTransform();
				rp3d::Transform interpolatedTrans = rp3d::Transform::
					interpolateTransforms(prevTrans, currentTrans,m_phystimer->Getfactor());
				
				transform.Translation = toVec3(interpolatedTrans.getPosition());
				transform.Rotation = toQuat(interpolatedTrans.getOrientation());
			}
		}
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
	rp3d::PhysicsWorld::WorldSettings settings;
	settings.defaultVelocitySolverNbIterations = 20;
	settings.isSleepingEnabled = false;
	settings.gravity = rp3d::Vector3(0.0f, -9.81f, 0.0f);

	m_world = m_physicsCommon.createPhysicsWorld(settings);

	auto rbview = m_registry.view<RigidBodyComponent>();
	for (auto e : rbview) {

		Entity entity = { e,this };
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rb = entity.GetComponent<RigidBodyComponent>();
		auto rp3dtrans = toTrans(transform);

		rb.m_rigidbody = m_world->createRigidBody(rp3dtrans);
		rb.m_rigidbody->setType(RigidbodyTypeToRp3dBody(rb.type));

		if (entity.HasComponent<ColliderComponent>()) {
			auto& collider = entity.GetComponent<ColliderComponent>();
			Gshape type = collider.m_geometry->GetShape();
			switch (type)
			{
			case Mint::Gshape::none:
			{
				MT_ERROR("Wrong colliderType")
				break;
			}
			case Mint::Gshape::box:
			{
				Box* box = (Box*)collider.m_geometry;
				rp3d::BoxShape* cs = m_physicsCommon.createBoxShape(toVec3(boxScale(*box, transform.Scale)));
				collider.m_collider = rb.m_rigidbody->addCollider(cs, rp3d::Transform());
				break;
			}
			case Mint::Gshape::sphere:
			{

				Sphere* sp = (Sphere*)collider.m_geometry;
				rp3d::SphereShape* cs = m_physicsCommon.createSphereShape(sphereScale(*sp, transform.Scale));
				collider.m_collider = rb.m_rigidbody->addCollider(cs, rp3d::Transform());
				break;
			}
			case Mint::Gshape::capsule:
			{
				Capsule* cp = (Capsule*)collider.m_geometry;
				auto rp3dcapsule = capsuleScale(*cp, transform.Scale);
				rp3d::CapsuleShape* cs = m_physicsCommon.createCapsuleShape(rp3dcapsule.x, rp3dcapsule.y);
				collider.m_collider = rb.m_rigidbody->addCollider(cs, rp3d::Transform());
				break;
			}
			default: 
			{
				MT_ERROR("Can not find the colliderType")
				break;
			}
			}
		}
	}

	auto cbview = m_registry.view<CollisionBodyComponent>();
	for (auto e : cbview) {

		Entity entity = { e,this };
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& cb = entity.GetComponent<CollisionBodyComponent>();
		auto rp3dtrans = toTrans(transform);

		cb.m_collidionbody = m_world->createCollisionBody(rp3dtrans);

		if (entity.HasComponent<ColliderComponent>()) {
			auto& collider = entity.GetComponent<ColliderComponent>();
			Gshape type = collider.m_geometry->GetShape();
			switch (type)
			{
			case Mint::Gshape::none:
				MT_ERROR("Wrong colliderType")
				break;
			case Mint::Gshape::box:
			{
				Box* box = (Box*)collider.m_geometry;
				rp3d::BoxShape* cs = m_physicsCommon.createBoxShape(toVec3(boxScale(*box, transform.Scale)));
				collider.m_collider = cb.m_collidionbody->addCollider(cs, rp3d::Transform());
				break;
			}
			case Mint::Gshape::sphere:
			{
				Sphere* sp = (Sphere*)collider.m_geometry;
				rp3d::SphereShape* cs = m_physicsCommon.createSphereShape(sphereScale(*sp, transform.Scale));
				collider.m_collider = cb.m_collidionbody->addCollider(cs, rp3d::Transform());
				break;
			}
			case Mint::Gshape::capsule:
			{
				Capsule* cp = (Capsule*)collider.m_geometry;
				auto rp3dcapsule = capsuleScale(*cp, transform.Scale);
				rp3d::CapsuleShape* cs = m_physicsCommon.createCapsuleShape(rp3dcapsule.x, rp3dcapsule.y);
				collider.m_collider = cb.m_collidionbody->addCollider(cs, rp3d::Transform());
				break;
			}
			default:
				MT_ERROR("Can not find the colliderType")
					break;
			}
		}

	}

}

void Scene::OnPhysicsStop()
{
	m_physicsCommon.destroyPhysicsWorld(m_world);
	m_world = nullptr;
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

MT_NAMESPACE_END