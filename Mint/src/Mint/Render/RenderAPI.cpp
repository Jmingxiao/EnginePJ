#include "pch.h"
#include "RenderAPI.h"
#include "Platform/OpenGL/OpenGLRenderAPI.h"


namespace Mint {
    
    RenderAPI::API RenderAPI::s_API = RenderAPI::API::OpenGL;

    Ptr<RenderAPI>RenderAPI::Create()
    {
		switch (s_API)
		{
		case RenderAPI::API::None:    MT_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return CreatePtr<OpenGLRenderAPI>();
		}

		MT_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
    }
}
