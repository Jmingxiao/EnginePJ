#include "EditorLayer.h"
#include <imgui/imgui.h>
//#include <ImGuizmo.h>

EditorLayer::EditorLayer()
	:Layer("Example")
{
}

void EditorLayer::OnAttach()
{
	FBSpecification spec;
	spec.Attachments = { FBTextureFormat::RGBA8, FBTextureFormat::RED_INTEGER, FBTextureFormat::Depth };
	spec.Width = Application::Get().GetWindow().GetWidth();
	spec.Height = Application::Get().GetWindow().GetHeight();
	spec.Samples = 4;
	fbo = Framebuffer::Create(spec);

	FBSpecification specinter;
	specinter.Attachments = { FBTextureFormat::RGBA8};
	specinter.Width = Application::Get().GetWindow().GetWidth();
	specinter.Height = Application::Get().GetWindow().GetHeight();
	intermidefbo = Framebuffer::Create(specinter);



	m_EditorScene = CreateRef<Scene>();
	m_ActiveScene = m_EditorScene;

	m_planeEntity = m_ActiveScene->CreateEntity();
	m_planeEntity.AddComponent<MeshRendererComponent>(Model::GetDefaultModelPath(DefualtModelType::Plane),"plane");
	m_planeEntity.AddComponent<RigidBodyComponent>(RigidBodyComponent::BodyType::Static);
	Box* box = new Box();
	box->m_half_extents = glm::vec3(10.0f,1.0,10.0f);
	m_planeEntity.AddComponent<ColliderComponent>(box,Gshape::box);

	m_BoxEntity = m_ActiveScene->CreateEntity();
	m_BoxEntity.AddComponent<MeshRendererComponent>(Model::GetDefaultModelPath(DefualtModelType::Box), "box");
	auto& boxts = m_BoxEntity.GetComponent<TransformComponent>();
	boxts.Translation = glm::vec3(0.f, 5.0f, 0.0f);
	boxts.Rotation = glm::quat(glm::vec3(60,0,60));
	m_BoxEntity.AddComponent<RigidBodyComponent>(RigidBodyComponent::BodyType::Dynamic);
	Box* box1 = new Box();
	m_BoxEntity.AddComponent<ColliderComponent>(box1, Gshape::box);

	editorCam = EditorCamera(45.0f, 1.778f, 0.1f, 1000.0f);
	editorCam.SetPosition({ 0.0f,5.0f,15.0f });

	m_ActiveScene->OnSimulationStart();
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(Timestep ts)
{
	//music.UpdateBufferStream();

	if (FBSpecification spec = fbo->GetSpecification();
		viewportSize.x > 0.0f && viewportSize.y > 0.0f && // zero sized framebuffer is invalid
		(spec.Width != viewportSize.x || spec.Height != viewportSize.y))
	{
		fbo->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		intermidefbo->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		editorCam.SetViewportSize(viewportSize.x, viewportSize.y);
	}
	
	Renderer3D::ResetStats();
	fbo->Bind();
	RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	RenderCommand::Clear();
	editorCam.SetControlActive(!isfocused);
	editorCam.OnUpdate(ts);
	m_ActiveScene->OnUpdateSimulation(ts, editorCam);
	auto& boxts = m_planeEntity.GetComponent<ColliderComponent>();
	
	//Renderer::BeginScene(editorCam);
	//Renderer::Submit(shader_t, planeModel,false);
	fbo->Unbind();

	intermidefbo->Blit(fbo);

	intermidefbo->Unbind();

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
			if (ImGui::MenuItem("Exit")) { Application::Get().Close(); }
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::End();


	m_sceneHierarchyPanel.OnImGuiRender();
	//engine states 
	ImGui::Begin("Settings");
	auto stats = Mint::Renderer3D::GetStats();
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);

	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("View port");
	ishoved = ImGui::IsWindowHovered();
	isfocused = ImGui::IsWindowFocused();
	Application::Get().GetImGuiLayer()->BlockEvents(!ishoved);
	auto m_vec2RenderViewPort = ImGui::GetContentRegionAvail();
	viewportSize = { m_vec2RenderViewPort.x,m_vec2RenderViewPort.y };
	uint64_t textureID = intermidefbo->GetColorAttachmentRendererID();
	ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2(viewportSize.x,viewportSize.y), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
	ImGui::PopStyleVar();
}

void EditorLayer::OnEvent(Mint::Event& event)
{
	if (m_SceneState == SceneState::Edit)
	{
		editorCam.OnEvent(event);
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
	return false;
}
