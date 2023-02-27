#include "pch.h"
#include "Renderer.h"
#include "Renderer3D.h"
MT_NAMESPACE_BEGIN


Ptr<Renderer::SceneData> Renderer::s_SceneData = CreatePtr<Renderer::SceneData>();

void Renderer::Init()
{
	RenderCommand::Init();
	Renderer3D::Init();
}

void Renderer::Shutdown()
{
	Renderer3D::Shutdown();
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
	RenderCommand::SetViewport(0, 0, width, height);
}

void Mint::Renderer::BeginScene(Camera& camera)
{
	s_SceneData->viewProjMatrix = camera.GetViewProjectionMatrix();
	s_SceneData->viewMatrix = camera.GetViewMatrix();
	s_SceneData->projMatrix = camera.GetProjection();
	s_SceneData->cameraPos = camera.GetPosition();
}

void Renderer::EndScene()
{

}

void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
{
	shader->Bind();
	shader->SetMat4("projMatrix", s_SceneData->projMatrix);
	shader->SetMat4("viewMatrix", s_SceneData->viewMatrix);
	shader->SetMat4("modelMatrix", transform);

	vertexArray->Bind();
	RenderCommand::DrawIndexed(vertexArray);
}

void Mint::Renderer::Submit(const Ref<Shader>& shader, Model* model,bool submitmat, const glm::mat4& transform)
{
	shader->Bind();
	shader->SetMat4("projMatrix", s_SceneData->projMatrix);
	shader->SetMat4("viewMatrix", s_SceneData->viewMatrix);
	shader->SetMat4("modelMatrix", transform);

	Model::render(model, shader, submitmat);
}

void Mint::Renderer::DrawBackGround(const Ref<Shader>& shader, const Ref<TextureHDR>& hdr)
{
	hdr->Bind(6);
	shader->Bind();
	shader->SetMat4("projMatrix", s_SceneData->projMatrix);
	shader->SetMat4("inv_PV", glm::inverse(s_SceneData->projMatrix*s_SceneData->viewMatrix));
	shader->SetFloat3("camera_pos", s_SceneData->cameraPos);
	RenderCommand::DrawFullscreenQuad();
}

void Mint::Renderer::DrawFullscreenQuad()
{
	RenderCommand::DrawFullscreenQuad();
}

MT_NAMESPACE_END