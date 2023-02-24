#pragma once
#include <Mint/Scene/Components.h>

MT_NAMESPACE_BEGIN

class Camera;
class EditorCamera;
class Shader;


class Renderer3D
{

public:
	static void Init();
	static void Shutdown();

	static void BeginScene(const Camera& camera, const glm::mat4& transform);
	static void BeginScene(const EditorCamera& camera);
	static void EndScene();
	static void Flush();

	static void DrawModel(const glm::mat4& transform,MeshRendererComponent&src,int entityId);
	static void DrawBackground(const std::string& = std::string());

	struct Statistics
	{
		uint32_t DrawCalls = 0;
		uint32_t Verticies = 0;

		uint32_t GetTotalVertexCount() const { return Verticies; }
	};

	static void ResetStats();
	static Statistics GetStats();
private:
};

MT_NAMESPACE_END
