#pragma once
#include"Mint/Render/Texture.h"
#include "Mint/Render/Shader.h"
MT_NAMESPACE_BEGIN

static const std::string modelPath = "assets/models/";
static const std::string dftModelPaths[] = 
{"Box.obj","Sphere.obj","Capsule.obj","Plane.obj"};

enum class DefualtModelType {
	Box = 0,Sphere,Capsule,Plane
};

struct Material
{
	std::string m_name;
	glm::vec3 m_color;
	float m_shininess;
	float m_metalness;
	float m_fresnel;
	glm::vec3 m_emission;
	float m_transparency;
	float m_ior;
	Ref<Texture2D> m_color_texture;		
	Ref<Texture2D> m_shininess_texture;
	Ref<Texture2D> m_metalness_texture;
	Ref<Texture2D> m_fresnel_texture;
	Ref<Texture2D> m_emission_texture;
};

struct Mesh
{
	std::string m_name;
	uint32_t m_material_idx;
	// Where this Mesh's vertices start
	uint32_t m_start_index;
	uint32_t m_number_of_vertices;
};

class Model
{
public:
	~Model();
	// The name of the whole model
	std::string m_name;
	// The filename of this model
	std::string m_filename;
	// The materials
	std::vector<Material> m_materials;
	// A model will contain one or more "Meshes"
	std::vector<Mesh> m_meshes;
	// Buffers on CPU
	std::vector<glm::vec3> m_positions;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec2> m_texture_coordinates;
	// Buffers on GPU
	uint32_t m_positions_bo;
	uint32_t m_normals_bo;
	uint32_t m_texture_coordinates_bo;
	// Vertex Array Object
	uint32_t m_vaob;

	static Model* loadModelFromOBJ(const std::string& path);
	static void saveModelToOBJ(Model* model, std::string filename);
	static void saveModelMaterialsToMTL(Model* model, std::string filename);
	static void freeModel(Model* model);
	static void render(const Model* model, const Ref<Shader>& shader, const bool submitMaterials = true, bool ssr = false);
	static const std::string GetDefaultModelPath(DefualtModelType type)
	{
		return modelPath+dftModelPaths[(size_t)type];
	}
};

MT_NAMESPACE_END