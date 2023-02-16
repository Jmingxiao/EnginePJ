#include "pch.h"
#include "Renderer.h"

MT_NAMESPACE_BEGIN


Ptr<Renderer::SceneData> Renderer::s_SceneData = CreatePtr<Renderer::SceneData>();

void Renderer::Init()
{
	RenderCommand::Init();
}

void Renderer::Shutdown()
{
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
	RenderCommand::SetViewport(0, 0, width, height);
}

void Mint::Renderer::BeginScene(Camera& camera)
{
	s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}

void Renderer::EndScene()
{

}

void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
{
	shader->Bind();
	shader->SetMat4("u_viewproj", s_SceneData->ViewProjectionMatrix);
	shader->SetMat4("u_transform", transform);

	vertexArray->Bind();
	RenderCommand::DrawIndexed(vertexArray);
}

void Mint::Renderer::Submit(const Ref<Shader>& shader, Model* model,bool submitmat, const glm::mat4& transform)
{
	shader->Bind();
	shader->SetMat4("u_viewproj", s_SceneData->ViewProjectionMatrix);
	shader->SetMat4("u_transform", transform);
	Model::render(model, shader, submitmat);

}

MT_NAMESPACE_END