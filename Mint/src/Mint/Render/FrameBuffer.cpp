#include "pch.h"
#include "FrameBuffer.h"
#include "Mint/Render/Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"


MT_NAMESPACE_BEGIN
Ref<Framebuffer> Framebuffer::Create(const FBSpecification& spec)
{
	switch (Renderer::GetAPI())
	{
	case RenderAPI::API::None:    MT_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RenderAPI::API::OpenGL:  return CreateRef<OpenGLFramebuffer>(spec);
	}
	MT_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

MT_NAMESPACE_END