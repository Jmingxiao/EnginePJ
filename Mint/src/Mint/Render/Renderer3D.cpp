#include "pch.h"
#include "Renderer3D.h"
#include "Mint/Render/Camera/Camera.h"
#include "Mint/Render/Camera/EditorCamera.h"
#include "Mint/Render/Model.h"
#include "Mint/Render/VertexArray.h"
#include "Mint/Render/Shader.h"
#include "glad/glad.h"
#include "Renderer.h"


MT_NAMESPACE_BEGIN


struct LineVertex
{
	glm::vec3 Position;
	glm::vec4 Color;

	// Editor-only
	int EntityID;
};

struct Renderer3DData
{

	Ref<VertexArray> LineVertexArray;
	Ref<VertexBuffer> LineVertexBuffer;
	Ref<Shader> LineShader;

	ShaderLibrary m_shaderLibrary;
	std::vector<MeshRendererComponent> meshs;
	
	Renderer3D::Statistics Stats;

	Camera m_camera;
};


static Renderer3DData s_Data;

void Renderer3D::Init()
{
	s_Data.m_shaderLibrary.Init();
}

void Renderer3D::Shutdown()
{
}

void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform)
{
	s_Data.m_camera = camera;
	s_Data.m_camera.SetViewMatrix(transform);
	Renderer::BeginScene(s_Data.m_camera);
}

void Renderer3D::BeginScene(const EditorCamera& camera)
{
	s_Data.m_camera = camera;
	Renderer::BeginScene(s_Data.m_camera);
}

void Renderer3D::EndScene()
{
	
}

void Renderer3D::Flush()
{
}

void Renderer3D::DrawFullscreenQuad(const Ref<Shader>& s)
{
	GLboolean previous_depth_state;
	glGetBooleanv(GL_DEPTH_TEST, &previous_depth_state);
	glDisable(GL_DEPTH_TEST);

	static Ref<VertexArray> va;
	static Ref<VertexBuffer> vb;

	if (!va.get()) {
		static float positions[] = {  -1.0f, -1.0f , 1.0f, -1.0f ,  1.0f, 1.0f ,
									-1.0f, -1.0f ,  1.0f, 1.0f ,   -1.0f, 1.0f  };
		BufferLayout layout = { {DataType::Float2,"a_position"}};
		vb = VertexBuffer::Create(positions, sizeof(positions));
		vb->SetLayout(layout);
		va->AddVertexBuffer(vb);
	}

	Renderer::Submit(s, va);
	if (previous_depth_state)
		glEnable(GL_DEPTH_TEST);

}

void Renderer3D::DrawModel(const glm::mat4& transform, MeshRendererComponent& src, int entityId)
{
	Ref<Shader> shader;
	if (src.s_type == BuiltinShaderType::custom)
		shader = s_Data.m_shaderLibrary.Get(src.model->m_name);
	else
		shader = s_Data.m_shaderLibrary.GetDefaultShader(src.s_type);
	Renderer::Submit(shader, src.model, true, transform);
	s_Data.Stats.DrawCalls += (uint32_t)src.model->m_meshes.size();
}

void Renderer3D::ResetStats()
{
	memset(&s_Data.Stats, 0, sizeof(Statistics));
}

Renderer3D::Statistics Renderer3D::GetStats()
{
	return Statistics();
}


MT_NAMESPACE_END
