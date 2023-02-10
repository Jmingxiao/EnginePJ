#include "pch.h"
#include "VertexArray.h"

#include "Mint/Render/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
namespace Mint {


	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    MT_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return CreateRef<OpenGLVertexArray>();
		}

		MT_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}



}