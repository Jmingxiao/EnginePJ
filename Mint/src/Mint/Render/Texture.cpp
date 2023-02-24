#include "pch.h"
#include "Texture.h"
#include "Mint/Render/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"


MT_NAMESPACE_BEGIN
Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, uint32_t m_components)
{
	switch (Renderer::GetAPI())
	{
	case RenderAPI::API::None:    MT_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RenderAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(width, height, m_components);
	}

	MT_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

Ref<Texture2D> Texture2D::Create(const std::string& path, uint32_t m_components)
{
	switch (Renderer::GetAPI())
	{
	case RenderAPI::API::None:    MT_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RenderAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(path, m_components);
	}

	MT_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

Ref<TextureHDR> TextureHDR::Create(const std::string& path)
{
	switch (Renderer::GetAPI())
	{
	case RenderAPI::API::None:    MT_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RenderAPI::API::OpenGL:  return CreateRef<OpenGLTextureHDR>(path);
	}

	MT_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

Ref<TextureHDR> TextureHDR::Create(const std::vector<std::string>& path)
{
	switch (Renderer::GetAPI())
	{
	case RenderAPI::API::None:    MT_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RenderAPI::API::OpenGL:  return CreateRef<OpenGLTextureHDR>(path);
	}

	MT_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

void TextureHDR::SaveHdrTexture(const std::string& path, uint32_t t_id)
{
	switch (Renderer::GetAPI())
	{
	case RenderAPI::API::None:    MT_ASSERT(false, "RendererAPI::None is currently not supported!"); return;
	case RenderAPI::API::OpenGL:  OpenGLTextureHDR::SaveHdrTexture(path, t_id) ;
	}

	MT_ASSERT(false, "Unknown RendererAPI!");

}

MT_NAMESPACE_END