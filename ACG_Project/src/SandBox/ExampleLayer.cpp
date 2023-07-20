#include "pch.h"
#include "ExampleLayer.h"
#include "glm/glm.hpp"
#include <glm/gtx/transform.hpp>
#include "OpenGL/Renderer.h"
#include <imgui.h>
#include "Sampler/DataStructures.h"


ExampleLayer::ExampleLayer()
{
	using namespace Acg;
	
	std::string path = "assets/textures/envmaps/lovelySky/";
	m_scene.LoadScene(true,path);
	testSH_scene.LoadScene(false);

	m_scene.PrecomputeEnvmaps(false);

	auto trans = glm::scale(glm::mat4(1), glm::vec3(0.1f, 0.1f, 0.1f));
	//m_scene.AddModel("assets/models/sponza/sponza.obj", "sponza",trans);
	m_scene.AddModel("assets/models/space-ship.obj", "spaceship", glm::translate(1.0f * glm::vec3(0.0f,3.0f,0.0f)));
	m_scene.AddModel("assets/models/landingpad.obj", "landingpad");//,glm::scale(glm::vec3(15.0,1.0,15.0)));
	coefs = Sampler::ReadFloatFromFile(path+"lighting.txt");

	testSH_scene.AddModel("assets/models/Sphere.obj", "mtt");

	camera = CreateRef<Camera>();
	camera->SetViewportSize(1600, 900);
	Acg::RendererCommand::Init();

	ShaderInit();
	FramebufferInit();
	
}

void ExampleLayer::OnUpdate(float dt)
{
	camera->OnUpdate(dt);

	Acg::RendererCommand::Clear();

	m_scene.DrawCascadeShadowMap(camera);
	
	glDisable(GL_BLEND);
	UpdateGbuffer();
	if(ssr_on)
		UpdatessrOutput();
	glEnable(GL_BLEND);

	
	Acg::RendererCommand::Clear();
	Acg::RendererCommand::SetViewport(0, 0, camera->m_viewportWidth, camera->m_viewportHeight);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_scene.csm->GetshaowMapID());
	gbuffer->BindColorTexture(0, 0);
	gbuffer->BindColorTexture(1, 1);
	gbuffer->BindColorTexture(2, 2);
	gbuffer->BindColorTexture(4, 4);
	if (ssr_on)
		if (ssrblur_on)
			ssrBlurFB->BindColorTexture(3);
		else
			ssrColor->BindColorTexture(3);

	Mixture->Bind();
	Mixture->SetBool("ssr_on", ssr_on);
	Mixture->SetInt("cascadeCount", static_cast<int>(m_scene.csm->GetLevelSize()));
	Mixture->SetMat4("viewInverse", glm::inverse(camera->GetViewMatrix()));
	for (size_t i = 0; i < m_scene.csm->GetLevelSize(); ++i)
	{
		Mixture->SetFloat("cascadePlaneDistances[" + std::to_string(i) + "]", m_scene.csm->GetLevel(i));
	}
	auto lightdirvs = camera->GetViewMatrix() * glm::vec4(m_scene.GetLight().direction, 0.0f);
	Mixture->SetFloat3("lightDirection", lightdirvs);
	Acg::RendererCommand::DrawFullScreenQuad();

	switch (debugmode)
	{
	case DebugMode::DEBUG_CSM:
		DrawCSMDebug();
		break;
	case DebugMode::DEBUG_SH:
		DrawSHDebug();
		break;
	case DebugMode::DEBUG_SSR:
		if(ssr_on)
			DrawssrOutput();
		break;
	}
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Menu");
	ImGui::SliderInt("depth layer count", &depthLayer, 0, 3);
	ImGui::SliderFloat3("Light Direction", &m_scene.GetLight().direction[0], -1.0f, 1.0f);
	ImGui::Checkbox("ssr on", & ssr_on);
	ImGui::Checkbox("ssr blur on", &ssrblur_on);
	ImGui::Checkbox("ssao on", &ssao_on);

	std::vector<std::string> debugmodes = { "Debug CSM","Debug SH","Debug SSR","None" };

	if (ImGui::BeginCombo("Debug Setting", "Select Debug Mode"))
	{
		for (int i = 0; i < debugmodes.size(); i++) {

			bool is_selected = (debugmode == (DebugMode)i);
			if (ImGui::Selectable(debugmodes[i].c_str(), is_selected)) {
				debugmode = (DebugMode)i;
			}
			if (is_selected)
					ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
	ImGui::End();

}

void ExampleLayer::OnEvent(Acg::Event& event)
{
	camera->OnEvent(event);
}

void ExampleLayer::DrawSkybox()
{
	m_scene.Bindbackground(6);
	background->Bind();
	background->SetMat4("projMatrix", camera->GetProjection());
	background->SetMat4("inv_PV", glm::inverse(camera->GetViewProjection()));
	background->SetFloat3("camera_pos", camera->GetPosition());
	Acg::RendererCommand::DrawFullScreenQuad();
}

void ExampleLayer::DrawCSMDebug()
{
	debugcsm->Bind();
	debugcsm->SetInt("layer", depthLayer);
	debugcsm->SetInt("shadowMap", 10);
	Acg::RendererCommand::DrawPartScreenQuad();


	simple->Bind();
	simple->SetMat4("view", camera->GetViewMatrix());
	simple->SetMat4("projection", camera->GetProjection());
	m_scene.csm->DebugLightFrustume(depthLayer);


}

void ExampleLayer::DrawSHDebug()
{
	int degree = (int)sqrt(coefs.size()) - 1;
	debugSh->Bind();
	debugSh->SetInt("degree_", degree);
	m_scene.Bindbackground(0);
	for (size_t i = 0; i < coefs.size(); ++i)
	{
		debugSh->SetFloat3("coefs[" + std::to_string(i) + "]", coefs[i]);
	}
	Acg::RendererCommand::DrawPartScreenQuad();
}

void ExampleLayer::DrawssrOutput()
{
	copyTexShader->Bind();
	if(ssrblur_on){ ssrBlurFB->BindColorTexture(); }
	else{ssrColor->BindColorTexture(); }
	Acg::RendererCommand::DrawFullScreenQuad();
}

void ExampleLayer::UpdatessrOutput()
{
	frameIndex = frameIndex++ % 1000;
	gbuffer->BindColorTexture(0,0);
	gbuffer->BindColorTexture(1, 1);
	gbuffer->BindColorTexture(2,2);
	gbuffer->BindColorTexture(3, 3);
	Acg::RendererCommand::Clear();
	Acg::RendererCommand::SetViewport(0, 0, camera->m_viewportWidth, camera->m_viewportHeight);
	if (Acg::FBSpecification spec = ssrColor->GetSpecification();
		(spec.Width != camera->m_viewportWidth || spec.Height != camera->m_viewportHeight))
	{
		ssrColor->Resize(camera->m_viewportWidth, camera->m_viewportHeight);
	}
	ssrColor->Bind();
	ssrOutput->Bind();
	ssrOutput->SetMat4("inverseProjectionMatrix", glm::inverse(camera->GetProjection()));
	ssrOutput->SetMat4( "projMatrix", camera->GetProjection());
	ssrOutput->SetMat4("projviewMatrix", camera->GetViewProjection());
	ssrOutput->SetMat4("viewInverse", glm::inverse(camera->GetViewMatrix()));
	ssrOutput->SetMat4("viewMatrix", camera->GetViewMatrix());
	ssrOutput->SetFloat("zfar", camera->GetFarPlane());
	ssrOutput->SetFloat("znear", camera->GetNearPlane());
	Acg::RendererCommand::DrawFullScreenQuad();
	ssrColor->Unbind();

	if (ssrblur_on) {

		if (Acg::FBSpecification spec = ssrBlurFB->GetSpecification();
			(spec.Width != camera->m_viewportWidth || spec.Height != camera->m_viewportHeight))
		{
			ssrBlurFB->Resize(camera->m_viewportWidth, camera->m_viewportHeight);
		}
		Acg::RendererCommand::Clear();
		Acg::RendererCommand::SetViewport(0, 0, camera->m_viewportWidth, camera->m_viewportHeight);
		ssrBlurFB->Bind();
		ssrblur->Bind();
		ssrColor->BindColorTexture();
		Acg::RendererCommand::DrawFullScreenQuad();
		ssrBlurFB->Unbind();
	}
}

void ExampleLayer::UpdateGbuffer()
{
	if (Acg::FBSpecification spec = gbuffer->GetSpecification();
		(spec.Width != camera->m_viewportWidth || spec.Height != camera->m_viewportHeight))
	{
		gbuffer->Resize(camera->m_viewportWidth, camera->m_viewportHeight);
	}
	gbuffer->Bind();
	Acg::RendererCommand::Clear();
	DrawSkybox();
	Acg::RendererCommand::SetViewport(0, 0, camera->m_viewportWidth, camera->m_viewportHeight);
	int degree = (int)sqrt(coefs.size()) - 1;
	gbufferInput->Bind();
	gbufferInput->SetInt("degree_", degree);
	m_scene.BindEnvmaps(6, 7);
	for (size_t i = 0; i < coefs.size(); ++i)
	{
		gbufferInput->SetFloat3("coefs[" + std::to_string(i) + "]", coefs[i]);
	}
	m_scene.DrawScene(gbufferInput, camera);
	gbuffer->Unbind();

}

void ExampleLayer::ShaderInit()
{
	using namespace Acg;
	std::vector<ShaderInfo> m_shaderInfo
	{
		{ShaderType::VETEX_SHADER,"assets/shaders/shader.vert"},
		{ShaderType::FRAGMENT_SHADER,"assets/shaders/shader.frag"}
	};
	m_shader = CreateRef<Shader>(m_shaderInfo, "m_shader");

	std::vector<ShaderInfo> backgroundShaderInfo
	{
		{ShaderType::VETEX_SHADER,"assets/shaders/background.vert"},
		{ShaderType::FRAGMENT_SHADER,"assets/shaders/background.frag"}
	};
	background = CreateRef<Shader>(backgroundShaderInfo, "bg");

	std::vector<ShaderInfo> debugcsminfo
	{
		{ShaderType::VETEX_SHADER,"assets/shaders/background.vert"},
		{ShaderType::FRAGMENT_SHADER,"assets/shaders/cascadeSM/copytexturearray.frag"},
	};
	debugcsm = CreateRef<Shader>(debugcsminfo, "debugCsmShader");

	std::vector<ShaderInfo> simpleInfo
	{
		{ShaderType::VETEX_SHADER,"assets/shaders/cascadeSM/simple.vert"},
		{ShaderType::FRAGMENT_SHADER,"assets/shaders/cascadeSM/simple.frag"}
	};
	simple = CreateRef<Shader>(simpleInfo, "simple");

	std::vector<ShaderInfo> debugshInfo
	{
		{ShaderType::VETEX_SHADER,"assets/shaders/background.vert"},
		{ShaderType::FRAGMENT_SHADER,"assets/shaders/SH/SHdebug.frag"}
	};
	debugSh = CreateRef<Shader>(debugshInfo, "debugsh");

	std::vector<ShaderInfo> gbufferInputInfo
	{
		{ShaderType::VETEX_SHADER,"assets/shaders/SSR/ssrInput.vert"},
		{ShaderType::FRAGMENT_SHADER,"assets/shaders/SSR/ssrInput.frag"}
	};
	gbufferInput = CreateRef<Shader>(gbufferInputInfo, "gbufferInput");

	std::vector<ShaderInfo> ssrOutputInfo
	{
		{ShaderType::VETEX_SHADER,"assets/shaders/background.vert"},
		{ShaderType::FRAGMENT_SHADER,"assets/shaders/SSR/ssrOutput.frag"}
	};
	ssrOutput = CreateRef<Shader>(ssrOutputInfo, "ssrOutput");

	std::vector<ShaderInfo> copuInfo
	{
		{ShaderType::VETEX_SHADER,"assets/shaders/background.vert"},
		{ShaderType::FRAGMENT_SHADER,"assets/shaders/copytexture.frag"}
	};
	copyTexShader = CreateRef<Shader>(copuInfo, "copyTexShader");

	std::vector<ShaderInfo> ssrblurInfo
	{
		{ShaderType::VETEX_SHADER,"assets/shaders/background.vert"},
		{ShaderType::FRAGMENT_SHADER,"assets/shaders/SSR/bilateral.frag"}
	};
	ssrblur = CreateRef<Shader>(ssrblurInfo, "ssrblur");


	std::vector<ShaderInfo> mixInfo
	{
		{ShaderType::VETEX_SHADER,"assets/shaders/background.vert"},
		{ShaderType::FRAGMENT_SHADER,"assets/shaders/mixing.frag"}
	};
	Mixture = CreateRef<Shader>(mixInfo, "Mixture");

}

void ExampleLayer::FramebufferInit()
{
	using namespace Acg;
	FBSpecification gbufferspec;
	gbufferspec.Attachments = { FBTextureFormat::RGBA16,FBTextureFormat::RGBA32, FBTextureFormat::RGBA32, 
	FBTextureFormat::RGBA16,FBTextureFormat::RGBA16,FBTextureFormat::Depth};
	gbuffer = CreateRef<FrameBuffer>(gbufferspec);

	FBSpecification ssrcolorspec;
	ssrcolorspec.Attachments = { FBTextureFormat::RGBA32 };
	ssrColor = CreateRef<FrameBuffer>(ssrcolorspec);

	ssrBlurFB = CreateRef<FrameBuffer>(ssrcolorspec);
}
