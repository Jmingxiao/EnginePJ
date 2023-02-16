#pragma once
#include<Mint.h>
#include "Panels/SceneHieracyPanel.h"

using namespace Mint;
class EditorLayer : public Layer
{
	int sound = -1;
	bool ishoved, isfocused;
	Model* planeModel = nullptr;
	EditorCamera editorCam;


	Ref<Shader> shader_t;
	Ref<Framebuffer> fbo;
	Ref<Framebuffer> intermidefbo;

	Ref<Texture2D> texture;
	Ref<Scene> m_ActiveScene;
	Ref<Scene> m_EditorScene;

	Entity m_planeEntity;

	Entity m_BoxEntity;

	glm::vec2 viewportSize = { 0.0f, 0.0f };

	enum class SceneState
	{
		Edit = 0, Play = 1, Simulate = 2
	};
	SceneState m_SceneState = SceneState::Edit;

	SceneHierarchyPanel m_sceneHierarchyPanel;

public:
	EditorLayer();
	~EditorLayer() =default;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(Timestep ts) override;

	void OnImGuiRender() override;

	void OnEvent(Mint::Event& event) override;

private:
	bool OnKeyPressed(KeyPressedEvent& e);
	bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
};
