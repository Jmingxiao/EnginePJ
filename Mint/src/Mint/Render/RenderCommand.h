#pragma once
#include "Mint/Render/RenderAPI.h"

MT_NAMESPACE_BEGIN

class RenderCommand
{
public:
	static void Init()
	{
		s_RenderAPI->Init();
	}

	static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		s_RenderAPI->SetViewport(x, y, width, height);
	}

	static void SetClearColor(const glm::vec4& color)
	{
		s_RenderAPI->SetClearColor(color);
	}

	static void Clear()
	{
		s_RenderAPI->Clear();
	}

	static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
	{
		s_RenderAPI->DrawIndexed(vertexArray, indexCount);
	}

	static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		s_RenderAPI->DrawLines(vertexArray, vertexCount);
	}

	static void SetLineWidth(float width)
	{
		s_RenderAPI->SetLineWidth(width);
	}
private:
	static Ptr<RenderAPI> s_RenderAPI;
};

MT_NAMESPACE_END