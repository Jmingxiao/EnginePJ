#include "MultisampleFramebuffer.h"
#include <imgui/imgui.h>

void Mint::MsFramebuffer::init()
{
	FBSpecification spec;
	spec.Attachments = { FBTextureFormat::RGBA8, FBTextureFormat::RED_INTEGER, FBTextureFormat::Depth };
	spec.Width = Application::Get().GetWindow().GetWidth();
	spec.Height = Application::Get().GetWindow().GetHeight();
	spec.Samples = 4;
	fbo = Framebuffer::Create(spec);

	FBSpecification msSpec;
	msSpec.Attachments = { FBTextureFormat::RED_INTEGER };
	msSpec.Width = Application::Get().GetWindow().GetWidth();
	msSpec.Height = Application::Get().GetWindow().GetHeight();
	msfbo = Framebuffer::Create(msSpec);

	std::vector<ShaderInfo> copytextShaderInfo
	{
		{ShaderType::VETEX_SHADER,"assets/shaders/background.vert"},
		{ShaderType::FRAGMENT_SHADER,"assets/shaders/copyEntitytexture.frag"}
	};
	copytextureShader = Shader::Create(copytextShaderInfo, "copytexture");


	FBSpecification specinter;
	specinter.Attachments = { FBTextureFormat::RGBA8 };
	specinter.Width = Application::Get().GetWindow().GetWidth();
	specinter.Height = Application::Get().GetWindow().GetHeight();
	intermidefbo = Framebuffer::Create(specinter);
}

void Mint::MsFramebuffer::blit()
{
	intermidefbo->Blit(fbo);
	intermidefbo->Unbind();
}

bool Mint::MsFramebuffer::Resizeviewport(const glm::vec2& viewportSize)
{
	if (FBSpecification spec = fbo->GetSpecification();
		viewportSize.x > 0.0f && viewportSize.y > 0.0f && // zero sized framebuffer is invalid
		(spec.Width != viewportSize.x || spec.Height != viewportSize.y))
	{
		fbo->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		msfbo->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		intermidefbo->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		return true;
	}
	return false;
}

uint32_t Mint::MsFramebuffer::GetviewportTextureID()
{
	return intermidefbo->GetColorAttachmentRendererID();
}

int Mint::MsFramebuffer::selectEntity(const glm::vec2& m_ViewportBounds0, const glm::vec2& m_ViewportBounds1)
{
	int pixelData{-1};
	msfbo->Bind();
	RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	RenderCommand::Clear();

	fbo->BindColorTexture(0, 1);
	copytextureShader->Bind();
	Renderer::DrawFullscreenQuad();

	auto [mx, my] = ImGui::GetMousePos();
	mx -= m_ViewportBounds0.x;
	my -= m_ViewportBounds0.y;
	glm::vec2 viewportSize = m_ViewportBounds1 - m_ViewportBounds0;
	my = viewportSize.y - my;
	int mouseX = (int)mx;
	int mouseY = (int)my;
	if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
	{
		pixelData = msfbo->ReadPixel(0, mouseX, mouseY);
	}
	msfbo->Unbind();
	return pixelData;
}
