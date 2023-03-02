#pragma once
#include<Mint.h>
#include "Panels/SceneHieracyPanel.h"
#include "Panels/ContentPenal.h"
#include "Methods/MultisampleFramebuffer.h"
using namespace Mint;


struct IconTexture
{
	Ref<Texture2D> m_IconPlay;
	Ref<Texture2D> m_IconPause;
	Ref<Texture2D> m_IconSimulate;
	Ref<Texture2D> m_IconStep;
	Ref<Texture2D> m_IconStop;
	void init() 
	{
		m_IconPlay = Texture2D::Create("resources/Icons/PlayButton.png");
		m_IconPause = Texture2D::Create("resources/Icons/PauseButton.png");
		m_IconSimulate = Texture2D::Create("resources/Icons/SimulateButton.png");
		m_IconStep = Texture2D::Create("resources/Icons/StepButton.png");
		m_IconStop = Texture2D::Create("resources/Icons/StopButton.png");
	}
};


class EditorLayer : public Layer
{
	int m_GizmoType = -1;
	bool ishoved, isfocused;
	EditorCamera m_editorCam;

	MsFramebuffer m_msfb;


	Ref<Scene> m_ActiveScene;
	Ref<Scene> m_EditorScene;

	Entity m_planeEntity;
	Entity m_BoxEntity;
	Entity m_screenCam;

	Entity m_selectEntity;
	
	
	glm::vec2 viewportSize = { 0.0f, 0.0f };
	glm::vec2 m_ViewportBounds[2];


	enum class SceneState
	{
		Edit = 0, Play = 1, Simulate = 2
	};
	SceneState m_SceneState = SceneState::Edit;

	SceneHierarchyPanel m_sceneHierarchyPanel;
	ContentBrowserPanel m_contentPanel;
public:
	EditorLayer();
	~EditorLayer() =default;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(Timestep ts) override;

	void OnImGuiRender() override;

	void OnEvent(Event& event) override;

private:
	bool OnKeyPressed(KeyPressedEvent& e);
	bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
	void DrawSettings();
	void UItoolBar();

	void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

	void OnScenePlay();
	void OnSceneSimulate();
	void OnSceneStop();
	void OnScenePause();

	void NewScene();
	void OpenScene();
	void OpenScene(const std::filesystem::path& path);
	void SaveScene();
	void SaveSceneAs();


private:

	struct Statistic {
		uint32_t framerate{};
		float cpumain{};
		float timer{};
	};

	Statistic m_data;
	IconTexture defaulttextures;
	std::filesystem::path m_EditorScenePath;
};
