#include "SandboxLayerEg.h"
#include <imgui/imgui.h>

ExampleLayer::ExampleLayer()
	:Layer("Example"), music(MusicBuffer("assets/musics/liz.mp3"))
{
	camera.SetPosition(glm::vec3(0, 5, 20));
	va = VertexArray::Create();
	std::vector<float> vertices{
	-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
	 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
	 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
	};

	Ref<VertexBuffer> vb = VertexBuffer::Create(&vertices[0], static_cast<uint32_t>(vertices.size()) * sizeof(float));
	BufferLayout layout = {
		{DataType::Float3,"a_position"},
		{DataType::Float4,"a_color"}
	};
	vb->SetLayout(layout);
	va->AddVertexBuffer(vb);

	uint32_t indices[3] = { 0, 1, 2 };
	Ref<IndexBuffer> ib = IndexBuffer::Create(indices, 3);
	va->SetIndexBuffer(ib);

	//Shader	
	std::vector<ShaderInfo> vecColorShaderInfo
	{
		{ShaderType::VETEX_SHADER,"assets/shaders/color.vert"},
		{ShaderType::FRAGMENT_SHADER,"assets/shaders/color.frag"}
	};
	shader_t = Shader::Create(vecColorShaderInfo, "testshader");

	model = Model::loadModelFromOBJ("assets/models/wheatley.obj");
	AudioListener::Init();

	//music.Play();
	sound = SoundEffectsLibrary::Get()->Load("assets/sounds/sand.ogg");
	//soundplayer.SetLooping(true);
	//soundplayer.Play(sound);
	FBSpecification spec;
	spec.Attachments = { FBTextureFormat::RGBA8, FBTextureFormat::RED_INTEGER, FBTextureFormat::Depth };
	spec.Width = Application::Get().GetWindow().GetWidth();
	spec.Height = Application::Get().GetWindow().GetHeight();
	fbo = Framebuffer::Create(spec);

}

void ExampleLayer::OnUpdate(Timestep ts)
{
	//music.UpdateBufferStream();

	if (FBSpecification spec = fbo->GetSpecification();
		viewportSize.x > 0.0f && viewportSize.y > 0.0f && // zero sized framebuffer is invalid
		(spec.Width != viewportSize.x || spec.Height != viewportSize.y))
	{
		fbo->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		camera.SetViewportSize(viewportSize.x, viewportSize.y);
	}

	fbo->Bind();
	RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	RenderCommand::Clear();
	camera.SetControlActive(!isfocused);
	camera.OnUpdate(ts);
	Renderer::BeginScene(camera);
	Renderer::Submit(shader_t, model);
	fbo->Unbind();



}

void ExampleLayer::OnImGuiRender()
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
	ImGui::Begin("DockSpace Demo", &opt_dockSpaceOpen, window_flags);
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



	ImGui::Begin("Settings");
	auto stats = Mint::Renderer3D::GetStats();
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);

	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("viewport");
	ishoved = ImGui::IsWindowHovered();
	isfocused = ImGui::IsWindowFocused();
	Application::Get().GetImGuiLayer()->BlockEvents(!ishoved);
	auto m_vec2RenderViewPort = ImGui::GetContentRegionAvail();
	viewportSize = { m_vec2RenderViewPort.x,m_vec2RenderViewPort.y };
	uint64_t textureID = fbo->GetColorAttachmentRendererID();
	ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2(viewportSize.x,viewportSize.y), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
	ImGui::PopStyleVar();
}

void ExampleLayer::OnEvent(Mint::Event& event)
{
	camera.OnEvent(event);

}
