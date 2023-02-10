#pragma once


#include "Mint/Render/RenderCommand.h"
#include "Mint/Render/Shader.h"
#include "Mint/Render/Camera/Camera.h"
#include "Mint/Render/Model.h"

namespace Mint {

	class Renderer {
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(Camera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		static void Submit(const Ref<Shader>& shader,Model* model, const glm::mat4& transform = glm::mat4(1.0f));

		static RenderAPI::API GetAPI() { return RenderAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static Ptr<SceneData> s_SceneData;
	};

}