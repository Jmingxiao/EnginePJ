#include "pch.h"
#include "Renderer3D.h"
#include "Mint/Render/Camera/Camera.h"
#include "Mint/Render/Camera/EditorCamera.h"
#include "Mint/Render/Model.h"
#include "Mint/Render/VertexArray.h"
#include "Mint/Render/Shader.h"
#include "Renderer.h"


MT_NAMESPACE_BEGIN


struct LineVertex
{
	glm::vec3 Position;
	glm::vec4 Color;

	// Editor-only
	int EntityID;
};
struct DefaultSceneInfo {
	float environment_multiplier = 1.0f;
	glm::vec3 lightDir = glm::vec3(0.5f, 1.0f, 0.0f);
};

struct Renderer3DData
{

	Ref<VertexArray> LineVertexArray;
	Ref<VertexBuffer> LineVertexBuffer;
	Ref<Shader> LineShader;

	ShaderLibrary m_shaderLibrary;
	std::vector<MeshRendererComponent> meshs;
	Ref<TextureHDR> m_defautHdri;
	Ref<TextureHDR> m_defaultIrr;
	Ref<TextureHDR> m_defaultReflection;

	float environment_multiplier = 1.0f;
	float lightIntensity = 2.0f;
	glm::vec3 lightDir = glm::vec3(0.5f, 1.0f, 0.0f);
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	Renderer3D::Statistics Stats;

	Camera m_camera;
};


static Renderer3DData s_Data;

void Renderer3D::Init()
{
	s_Data.m_shaderLibrary.Init();
	s_Data.m_defautHdri = TextureHDR::Create("assets/textures/hdri/default.hdr");
	s_Data.m_defaultIrr = TextureHDR::Create("assets/textures/hdri/default_irr.hdr");
	std::string envmap_base_name = "default_reflectmap";
	const int roughnesses = 8;
	std::vector<std::string> filenames;
	for (int i = 0; i < roughnesses; i++)
		filenames.push_back("assets/textures/hdri/" + envmap_base_name+ std::to_string(i) + ".hdr");
	s_Data.m_defaultReflection = TextureHDR::Create(filenames);
}

void Renderer3D::Shutdown()
{
}

void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform)
{
	s_Data.m_camera = camera;
	s_Data.m_camera.SetViewMatrix(transform);
	Renderer::BeginScene(s_Data.m_camera);
	s_Data.m_defautHdri->Bind(6);
	s_Data.m_defaultIrr->Bind(7);
	s_Data.m_defaultReflection->Bind(8);
}

void Renderer3D::BeginScene(const EditorCamera& camera)
{
	s_Data.m_camera = camera;
	Renderer::BeginScene(s_Data.m_camera);
	s_Data.m_defautHdri->Bind(6);
	s_Data.m_defaultIrr->Bind(7);
	s_Data.m_defaultReflection->Bind(8);
}

void Renderer3D::EndScene()
{
	
}

void Renderer3D::Flush()
{

}


void Renderer3D::DrawModel(const glm::mat4& transform, MeshRendererComponent& src, int entityId)
{
	Ref<Shader> shader;
	switch (src.s_type) {
	case BuiltinShaderType::custom: 
	{
		shader = s_Data.m_shaderLibrary.Get(src.model->m_name);
	break; 
	}
	case BuiltinShaderType::background:
	case BuiltinShaderType::simple:
	{
		shader = s_Data.m_shaderLibrary.GetDefaultShader(src.s_type);
		break;
	}
	case BuiltinShaderType::pbr:
	{
		shader = s_Data.m_shaderLibrary.GetDefaultShader(src.s_type);
		shader->Bind();
		shader->SetFloat("environment_multiplier", s_Data.environment_multiplier);
		shader->SetFloat("lightIntensity", s_Data.lightIntensity);
		shader->SetFloat3("lightDirection", s_Data.lightDir);
		shader->SetFloat3("lightcolor", s_Data.lightColor);
		break;
	}
	}
	Renderer::Submit(shader, src.model, true, transform);
	s_Data.Stats.DrawCalls += (uint32_t)src.model->m_meshes.size();
	s_Data.Stats.Verticies += (uint32_t)src.model->m_positions.size();
}

void Renderer3D::DrawBackground(const std::string& s)
{
	if (s.empty()) {
		Renderer::DrawBackGround(
			s_Data.m_shaderLibrary.GetDefaultShader(BuiltinShaderType::background),s_Data.m_defautHdri);
	}
}

void Renderer3D::ResetStats()
{
	memset(&s_Data.Stats, 0, sizeof(Statistics));
}

Renderer3D::Statistics Renderer3D::GetStats()
{
	return s_Data.Stats;
}


MT_NAMESPACE_END
