#include "pch.h"
#include "Scene.h"
#include "Components.h"


MT_NAMESPACE_BEGIN
Scene::Scene()
	:m_registry(CreateRef<entt::registry>())
{
	MT_INFO("Scene Creat");
}

Scene::~Scene()
{
	MT_INFO("Scene Destroy");
}

Entity Scene::CreateEntity(const std::string& name)
{
	return CreateEntityWithUUID(UUID(), name);
}

Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
{
	Entity entity = Entity(m_registry);
	entity.AddComponent<IDComponent>(uuid);
	entity.AddComponent<TransformComponent>();
	auto& tag = entity.AddComponent<TagComponent>();
	tag.Tag = name.empty() ? "Entity" : name;

	m_entitymap[uuid] = entity;

	return entity;
}

void Scene::DestroyEntity(Entity entity)
{
	m_registry->destroy(entity);
	m_entitymap.erase(entity.GetUUID());
}

void Scene::OnUpdate(const Timestep& timeStep)
{
}

void Scene::OnViewPortResize(uint32_t uiWidth, uint32_t uiHeight)
{
}

const Ref<entt::registry>& Scene::Registry() const
{
	return m_registry;
}

Entity Scene::FindEntityByName(std::string_view name)
{
	auto view = m_registry->view<TagComponent>();
	for (auto entity : view)
	{
		const TagComponent& tc = view.get<TagComponent>(entity);
		if (tc.Tag == name)
			return Entity{m_registry,entity };
	}
	return {};
}

Entity Scene::GetEntityByUUID(UUID uuid)
{
	if (m_entitymap.find(uuid) != m_entitymap.end())
		return { m_registry,m_entitymap.at(uuid) };

	return {};
}

MT_NAMESPACE_END