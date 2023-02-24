#pragma once

#include "Mint/Core/Base.h"
#include "Mint/Scene/Scene.h"
#include "Mint/Scene/Entity.h"
#include <imgui/imgui.h>

MT_NAMESPACE_BEGIN

class SceneHierarchyPanel
{
public:
	SceneHierarchyPanel() = default;
	SceneHierarchyPanel(const Ref<Scene>& scene);

	void SetContext(const Ref<Scene>& scene);

	void OnImGuiRender();

	Entity GetSelectedEntity() const { return m_selectedContext; }
	void SetSelectedEntity(Entity entity);
private:
	template<typename T>
	void DisplayAddComponentEntry(const std::string& entryName);

	void DrawEntityNode(Entity entity);
	void DrawComponents(Entity entity);
private:
	Ref<Scene> m_context;
	Entity m_selectedContext;
};







MT_NAMESPACE_END


