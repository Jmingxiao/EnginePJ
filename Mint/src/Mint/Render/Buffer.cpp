#pragma once
#include "pch.h"
#include "Buffer.h"

#include "Mint/Render/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"


MT_NAMESPACE_BEGIN

Ref<VertexBuffer> Mint::VertexBuffer::Create(uint32_t size)
{
	switch (Renderer::GetAPI())
	{
	case RenderAPI::API::None:    MT_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RenderAPI::API::OpenGL:  return CreateRef<OpenGLVertexBuffer>(size);
	}

	MT_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

Ref<VertexBuffer> Mint::VertexBuffer::Create(float* vertices, uint32_t size)
{
	switch (Renderer::GetAPI())
	{
	case RenderAPI::API::None:    MT_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RenderAPI::API::OpenGL:  return CreateRef<OpenGLVertexBuffer>(vertices,size);
	}

	MT_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

Ref<IndexBuffer> Mint::IndexBuffer::Create(uint32_t* indices, uint32_t count)
{
	switch (Renderer::GetAPI())
	{
	case RenderAPI::API::None:    MT_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RenderAPI::API::OpenGL:  return CreateRef<OpenGLIndexBuffer>(indices, count);
	}

	MT_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

MT_NAMESPACE_END