#pragma once

MT_NAMESPACE_BEGIN

class Renderer3D
{
public:
	static void Init();
	static void Shutdown();

	struct Statistics
	{
		uint32_t DrawCalls = 0;
		uint32_t Verticies = 0;

		uint32_t GetTotalVertexCount() const { return Verticies; }
	};

	static void ResetStats();
	static Statistics GetStats();
};

MT_NAMESPACE_END
