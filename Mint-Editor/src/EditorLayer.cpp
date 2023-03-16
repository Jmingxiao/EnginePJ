#include "EditorLayer.h"
#include <imgui/imgui.h>
#include "ImGuizmo.h"
#include "Mint/Math/Math.h"
#include <glm/gtc/type_ptr.hpp>

EditorLayer::EditorLayer()
	:Layer("Example")
{
}

void EditorLayer::OnAttach()
{
	defaulttextures.init();
	m_msfb.init();

	m_EditorScene = CreateRef<Scene>();
	m_ActiveScene = m_EditorScene;

	m_planeEntity = m_ActiveScene->CreateEntity("Plane");
	m_planeEntity.AddComponent<MeshRendererComponent>(Model::GetDefaultModelPath(DefualtModelType::Plane),"plane", BuiltinShaderType::pbr);
	m_planeEntity.AddComponent<RigidBodyComponent>(RigidBodyComponent::BodyType::Static);
	Box* box = new Box();
	box->m_half_extents = glm::vec3(10.0f,1.0,10.0f);
	m_planeEntity.AddComponent<ColliderComponent>(box);

	m_BoxEntity = m_ActiveScene->CreateEntity("Box");
	m_BoxEntity.AddComponent<MeshRendererComponent>(Model::GetDefaultModelPath(DefualtModelType::Box), "box", BuiltinShaderType::simple);
	auto& boxts = m_BoxEntity.GetComponent<TransformComponent>();
	boxts.Translation = glm::vec3(0.f, 5.0f, 0.0f);
	boxts.Rotation = glm::quat(glm::vec3(60,0,60));
	m_BoxEntity.AddComponent<RigidBodyComponent>(RigidBodyComponent::BodyType::Dynamic);
	Box* box1 = new Box();
	m_BoxEntity.AddComponent<ColliderComponent>(box1);
	m_BoxEntity.AddComponent<MusicComponent>("assets/musics/liz.mp3");

	m_spaceShip = m_ActiveScene->CreateEntity("space Ship");
	m_spaceShip.AddComponent<MeshRendererComponent>("assets/models/space-ship.obj", "spaceship", BuiltinShaderType::pbr);
	auto& sctrans = m_spaceShip.GetComponent<TransformComponent>();
	sctrans.Translation = glm::vec3(0.f, 5.0f, 3.0f);
	sctrans.Scale = glm::vec3(0.4f, 0.4f, 0.4f);

	m_editorCam = EditorCamera(45.0f, 1.778f, 0.1f, 1000.0f);
	m_editorCam.SetPosition({ 0.0f,5.0f,15.0f });

	m_ActiveScene->OnSimulationStart();

	m_sceneHierarchyPanel.SetContext(m_ActiveScene);
	m_selectEntity = {};
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(Timestep ts)
{

	m_data.timer += ts;
	if (m_data.timer >0.5f) {
		m_data.cpumain = ts.GetMilliseconds();
		m_data.framerate = static_cast<uint32_t>(1/ts.GetSeconds());
		m_data.timer = 0.0f;
	}

	m_ActiveScene->OnViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
	if (m_msfb.Resizeviewport(viewportSize))
	{
		m_editorCam.SetViewportSize(viewportSize.x, viewportSize.y);
	}
	
	Renderer3D::ResetStats();
	m_msfb.fbo->Bind();
	RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	RenderCommand::Clear();

	switch (m_SceneState)
	{
	case SceneState::Edit:
	{
		m_editorCam.SetControlActive(!isfocused);
		m_editorCam.OnUpdate(ts);
		m_ActiveScene->OnUpdateEditor(ts, m_editorCam);
		break;
	}
	case SceneState::Simulate:
	{
		m_editorCam.SetControlActive(!isfocused);
		m_editorCam.OnUpdate(ts);

		m_ActiveScene->OnUpdateSimulation(ts, m_editorCam);
		break;
	}
	case SceneState::Play:
	{
		m_ActiveScene->OnUpdate(ts);
		break;
	}
	}

	/*m_editorCam.SetControlActive(!isfocused);
	m_editorCam.OnUpdate(ts);
	m_ActiveScene->OnUpdateSimulation(ts, m_editorCam);*/
	m_msfb.fbo->Unbind();

	int pixelData = m_msfb.selectEntity(m_ViewportBounds[0], m_ViewportBounds[1]);
	if (pixelData != -1) {
		m_selectEntity = Entity((entt::entity)pixelData, m_ActiveScene.get());
	}

	m_msfb.blit();

}

void EditorLayer::OnImGuiRender()
{
	static bool opt_dockSpaceOpen = true;
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", &opt_dockSpaceOpen, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	float minWinSizeX = style.WindowMinSize.x;
	style.WindowMinSize.x = 370.f;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	style.WindowMinSize.x = minWinSizeX;
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open...", "Ctrl+O"))
				OpenScene();

			if (ImGui::MenuItem("New Scene", "Ctrl+N"))
				NewScene();

			if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
				SaveScene();

			if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
				SaveSceneAs();

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
				Application::Get().Close();

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::End();

	m_sceneHierarchyPanel.OnImGuiRender();
	m_contentPanel.OnImGuiRender();
	DrawSettings();

	//view port
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("View port");


	auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
	auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
	auto viewportOffset = ImGui::GetWindowPos();
	m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
	m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

	ishoved = ImGui::IsWindowHovered();
	isfocused = ImGui::IsWindowFocused();
	Application::Get().GetImGuiLayer()->BlockEvents(ishoved);
	auto m_vec2RenderViewPort = ImGui::GetContentRegionAvail();
	viewportSize = { m_vec2RenderViewPort.x,m_vec2RenderViewPort.y };
	uint64_t textureID = m_msfb.GetviewportTextureID();
	ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2(viewportSize.x,viewportSize.y), ImVec2(0, 1), ImVec2(1, 0));


	Entity selectedEntity = m_sceneHierarchyPanel.GetSelectedEntity();
	if (selectedEntity && m_GizmoType != -1)
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

		// Editor camera
		const glm::mat4& cameraProjection = m_editorCam.GetProjection();
		glm::mat4 cameraView = m_editorCam.GetViewMatrix();

		// Entity transform
		auto& tc = selectedEntity.GetComponent<TransformComponent>();
		glm::mat4 transform = tc.GetTransform();

		bool snap = Input::IsKeyPressed(Key::LeftControl);
		float snapValue = 0.5f; // Snap to 0.5m for translation/scale
		// Snap to 45 degrees for rotation
		if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
			snapValue = 45.0f;

		float snapValues[3] = { snapValue, snapValue, snapValue };

		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
			(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
			nullptr, snap ? snapValues : nullptr);

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 translation, rotation, scale;
			Math::DecomposeTransform(transform, translation, rotation, scale);

			tc.Translation = translation;
			tc.Rotation = glm::quat(rotation);
			tc.Scale = scale;
		}


	}
	ImGui::End();
	ImGui::PopStyleVar();
	UItoolBar();

}

void EditorLayer::OnEvent(Mint::Event& event)
{
	if (m_SceneState == SceneState::Edit)
	{
		m_editorCam.OnEvent(event);
		m_contentPanel.OnEvent(event);
	}
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<KeyPressedEvent>(MT_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	dispatcher.Dispatch<MouseButtonPressedEvent>(MT_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
}

bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
{
	return false;
}

bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	if (e.GetMouseButton() == (int)Mouse::Left && m_selectEntity)
	{
		m_sceneHierarchyPanel.SetSelectedEntity(m_selectEntity);
	}
	return false;
}

void EditorLayer::DrawSettings()
{
	ImGui::Begin("Settings");
	auto stats = Mint::Renderer3D::GetStats();
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Vertices: %d", stats.Verticies);
	ImGui::Text("FPS: %d", m_data.framerate);
	ImGui::Text("CPU Main: %f ms", m_data.cpumain);
	if (ImGui::Button("Gizmos Translation"))
		m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;

	if (ImGui::Button("Gizmos Rotation"))
		m_GizmoType = ImGuizmo::OPERATION::ROTATE;

	if (ImGui::Button("Gizmos Scale"))
		m_GizmoType = ImGuizmo::OPERATION::SCALE;

	ImGui::End();
}

void EditorLayer::UItoolBar()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	auto& colors = ImGui::GetStyle().Colors;
	const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
	const auto& buttonActive = colors[ImGuiCol_ButtonActive];
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

	ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	bool toolbarEnabled = (bool)m_ActiveScene;

	ImVec4 tintColor = ImVec4(1, 1, 1, 1);
	if (!toolbarEnabled)
		tintColor.w = 0.5f;

	float size = ImGui::GetWindowHeight() - 4.0f;
	ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

	bool hasPlayButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play;
	bool hasSimulateButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate;
	bool hasPauseButton = m_SceneState != SceneState::Edit;

	if (hasPlayButton)
	{
		Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? defaulttextures.m_IconPlay : defaulttextures.m_IconStop;
		if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
		{
			if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
				OnScenePlay();
			else if (m_SceneState == SceneState::Play)
				OnSceneStop();
		}
	}

	if (hasSimulateButton)
	{
		if (hasPlayButton)
			ImGui::SameLine();

		Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? defaulttextures.m_IconSimulate : defaulttextures.m_IconStop;
		if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
		{
			if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
				OnSceneSimulate();
			else if (m_SceneState == SceneState::Simulate)
				OnSceneStop();
		}
	}
	if (hasPauseButton)
	{
		bool isPaused = m_ActiveScene->IsPaused();
		ImGui::SameLine();
		{
			Ref<Texture2D> icon = defaulttextures.m_IconPause;
			if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				m_ActiveScene->SetPaused(!isPaused);
			}
		}

		// Step button
		if (isPaused)
		{
			ImGui::SameLine();
			{
				Ref<Texture2D> icon = defaulttextures.m_IconStep;
				bool isPaused = m_ActiveScene->IsPaused();
				if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
				{
					m_ActiveScene->Step();
				}
			}
		}
	}
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(3);
	ImGui::End();
}

void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
{
	SceneSerializer serializer(scene);
	serializer.Serialize(path.string());
}

void EditorLayer::OnScenePlay()
{
	if (m_SceneState == SceneState::Simulate)
		OnSceneStop();

	m_SceneState = SceneState::Play;

	m_ActiveScene = Scene::Copy(m_EditorScene);
	m_ActiveScene->OnRuntimeStart();

	m_sceneHierarchyPanel.SetContext(m_ActiveScene);
}

void EditorLayer::OnSceneSimulate()
{
	if (m_SceneState == SceneState::Play)
		OnSceneStop();

	m_SceneState = SceneState::Simulate;

	m_ActiveScene = Scene::Copy(m_EditorScene);
	m_ActiveScene->OnSimulationStart();

	m_sceneHierarchyPanel.SetContext(m_ActiveScene);
}

void EditorLayer::OnSceneStop()
{
	MT_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate);

	if (m_SceneState == SceneState::Play)
		m_ActiveScene->OnRuntimeStop();
	else if (m_SceneState == SceneState::Simulate)
		m_ActiveScene->OnSimulationStop();

	m_SceneState = SceneState::Edit;

	m_ActiveScene = m_EditorScene;

	m_sceneHierarchyPanel.SetContext(m_ActiveScene);
}

void EditorLayer::OnScenePause()
{
	if (m_SceneState == SceneState::Edit)
		return;

	m_ActiveScene->SetPaused(true);
}

void EditorLayer::NewScene()
{
	m_ActiveScene = CreateRef<Scene>();
	m_sceneHierarchyPanel.SetContext(m_ActiveScene);

	m_EditorScenePath = std::filesystem::path();
}

void EditorLayer::OpenScene()
{
	std::string filepath = FileDialogs::OpenFile("Mint Scene (*.mint)\0*.mint\0");
	if (!filepath.empty())
		OpenScene(filepath);
}

void EditorLayer::OpenScene(const std::filesystem::path& path)
{
	if (m_SceneState != SceneState::Edit)
		OnSceneStop();

	if (path.extension().string() != ".mint")
	{
		MT_WARN("Could not load {0} - not a scene file", path.filename().string());
		return;
	}

	Ref<Scene> newScene = CreateRef<Scene>();
	SceneSerializer serializer(newScene);
	if (serializer.Deserialize(path.string()))
	{
		m_EditorScene = newScene;
		m_sceneHierarchyPanel.SetContext(m_EditorScene);

		m_ActiveScene = m_EditorScene;
		m_EditorScenePath = path;
	}

}

void EditorLayer::SaveScene()
{
	if (!m_EditorScenePath.empty())
		SerializeScene(m_ActiveScene, m_EditorScenePath);
	else
		SaveSceneAs();
}

void EditorLayer::SaveSceneAs()
{
	std::string filepath = FileDialogs::SaveFile("Mint Scene (*.mint)\0*.mint\0");
	if (!filepath.empty())
	{
		SerializeScene(m_ActiveScene, filepath);
		m_EditorScenePath = filepath;
	}
}
