#include"pch.h"
#include "Model.h"
#include "glad/glad.h"
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include <tiny_obj_loader.h>
#include <iomanip>

#include <filesystem>
#include <imgui.h>
#include <imgui_internal.h>

MT_NAMESPACE_BEGIN



Model* Model::loadModelFromOBJ(const std::string& path)
{
	std::string filename, extension, directory;

	filename = file::normalise(path);
	directory = file::parent_path(path);
	filename = file::file_stem(path);
	extension = file::file_extension(path);

	if (extension != ".obj")
	{
		MT_ERROR("Fatal: loadModelFromOBJ(): Expecting filename ending in '.obj'");
		exit(1);
	}

	///////////////////////////////////////////////////////////////////////
	// Parse the OBJ file using tinyobj
	///////////////////////////////////////////////////////////////////////
	MT_TRACE( "Loading " +path +"..." );
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	// Expect '.mtl' file in the same directory and triangulate meshes
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err,
		(directory + filename + extension).c_str(), directory.c_str(), true);

	MT_ASSERT(err.empty(), err);
	std::cerr << err << std::endl;
	if (!ret)
	{
		exit(1);
	}
	Model* model = new Model;
	model->m_name = filename;
	model->m_filename = path;

	///////////////////////////////////////////////////////////////////////
	// Transform all materials into our datastructure
	///////////////////////////////////////////////////////////////////////
	for (const auto& m : materials)
	{
		Material material;
		material.m_name = m.name;
		material.m_color = glm::vec3(m.diffuse[0], m.diffuse[1], m.diffuse[2]);
		if (m.diffuse_texname != "")
		{
			auto p = file::normalise(directory) + file::normalise(m.diffuse_texname);
			material.m_color_texture = Texture2D::Create(p, 4);
		}
		material.m_metalness = m.metallic;
		if (m.metallic_texname != "")
		{
			auto p = file::normalise(directory) + file::normalise(m.metallic_texname);
			material.m_metalness_texture = Texture2D::Create(p, 1);
		}
		material.m_fresnel = m.specular[0];
		if (m.specular_texname != "")
		{
			auto p = file::normalise(directory) + file::normalise(m.specular_texname);
			material.m_fresnel_texture = Texture2D::Create(p, 1);
		}
		material.m_shininess = m.roughness;
		if (m.roughness_texname != "")
		{
			auto p = file::normalise(directory) + file::normalise(m.roughness_texname);
			material.m_shininess_texture = Texture2D::Create(p, 1);
		}
		material.m_emission = glm::vec3(m.emission[0], m.emission[1], m.emission[2]);
		if (m.emissive_texname != "")
		{
			auto p = file::normalise(directory) + file::normalise(m.emissive_texname);
			material.m_emission_texture = Texture2D::Create(p, 4);
		}
		material.m_transparency = m.transmittance[0];
		material.m_ior = m.ior;
		model->m_materials.push_back(material);
	}

///////////////////////////////////////////////////////////////////////
// A vertex in the OBJ file may have different indices for position,
// normal and texture coordinate. We will not even attempt to use
// indexed lookups, but will store a simple vertex stream per mesh.
	///////////////////////////////////////////////////////////////////////
	uint64_t number_of_vertices = 0;
	for (const auto& shape : shapes)
	{
		number_of_vertices += shape.mesh.indices.size();
	}
	model->m_positions.resize(number_of_vertices);
	model->m_normals.resize(number_of_vertices);
	model->m_texture_coordinates.resize(number_of_vertices);

	///////////////////////////////////////////////////////////////////////
	// For each vertex _position_ auto generate a normal that will be used
	// if no normal is supplied.
	///////////////////////////////////////////////////////////////////////
	std::vector<glm::vec4> auto_normals(attrib.vertices.size() / 3);
	for (const auto& shape : shapes)
	{
		for (int face = 0; face < int(shape.mesh.indices.size()) / 3; face++)
		{
			glm::vec3 v0 = glm::vec3(attrib.vertices[shape.mesh.indices[face * 3 + 0].vertex_index * 3 + 0],
				attrib.vertices[shape.mesh.indices[face * 3 + 0].vertex_index * 3 + 1],
				attrib.vertices[shape.mesh.indices[face * 3 + 0].vertex_index * 3 + 2]);
			glm::vec3 v1 = glm::vec3(attrib.vertices[shape.mesh.indices[face * 3 + 1].vertex_index * 3 + 0],
				attrib.vertices[shape.mesh.indices[face * 3 + 1].vertex_index * 3 + 1],
				attrib.vertices[shape.mesh.indices[face * 3 + 1].vertex_index * 3 + 2]);
			glm::vec3 v2 = glm::vec3(attrib.vertices[shape.mesh.indices[face * 3 + 2].vertex_index * 3 + 0],
				attrib.vertices[shape.mesh.indices[face * 3 + 2].vertex_index * 3 + 1],
				attrib.vertices[shape.mesh.indices[face * 3 + 2].vertex_index * 3 + 2]);

			glm::vec3 e0 = glm::normalize(v1 - v0);
			glm::vec3 e1 = glm::normalize(v2 - v0);
			glm::vec3 face_normal = cross(e0, e1);

			auto_normals[shape.mesh.indices[face * 3 + 0].vertex_index] += glm::vec4(face_normal, 1.0f);
			auto_normals[shape.mesh.indices[face * 3 + 1].vertex_index] += glm::vec4(face_normal, 1.0f);
			auto_normals[shape.mesh.indices[face * 3 + 2].vertex_index] += glm::vec4(face_normal, 1.0f);
		}
	}
	for (auto& normal : auto_normals)
	{
		normal = (1.0f / normal.w) * normal;
	}

	///////////////////////////////////////////////////////////////////////
	// Now we will turn all shapes into Meshes. A shape that has several
	// materials will be split into several meshes with unique names
	///////////////////////////////////////////////////////////////////////
	int vertices_so_far = 0;
	for (int s = 0; s < shapes.size(); ++s)
	{
		const auto& shape = shapes[s];
		///////////////////////////////////////////////////////////////////
		// The shapes in an OBJ file may several different materials.
		// If so, we will split the shape into one Mesh per Material
		///////////////////////////////////////////////////////////////////
		int next_material_index = shape.mesh.material_ids[0];
		int next_material_starting_face = 0;
		std::vector<bool> finished_materials(materials.size(), false);
		int number_of_materials_in_shape = 0;
		while (next_material_index != -1)
		{
			int current_material_index = next_material_index;
			int current_material_starting_face = next_material_starting_face;
			next_material_index = -1;
			next_material_starting_face = -1;
			// Process a new Mesh with a unique material
			Mesh mesh;
			mesh.m_name = shape.name + "_" + materials[current_material_index].name;
			mesh.m_material_idx = current_material_index;
			mesh.m_start_index = vertices_so_far;
			number_of_materials_in_shape += 1;

			uint64_t number_of_faces = shape.mesh.indices.size() / 3;
			for (int i = current_material_starting_face; i < number_of_faces; i++)
			{
				if (shape.mesh.material_ids[i] != current_material_index)
				{
					if (next_material_index >= 0)
						continue;
					else if (finished_materials[shape.mesh.material_ids[i]])
						continue;
					else
					{ // Found a new material that we have not processed.
						next_material_index = shape.mesh.material_ids[i];
						next_material_starting_face = i;
					}
				}
				else
				{
					///////////////////////////////////////////////////////
					// Now we generate the vertices
					///////////////////////////////////////////////////////
					for (int j = 0; j < 3; j++)
					{
						int v = shape.mesh.indices[i * 3 + j].vertex_index;
						model->m_positions[vertices_so_far + j] =
							glm::vec3(attrib.vertices[shape.mesh.indices[i * 3 + j].vertex_index * 3 + 0],
								attrib.vertices[shape.mesh.indices[i * 3 + j].vertex_index * 3 + 1],
								attrib.vertices[shape.mesh.indices[i * 3 + j].vertex_index * 3 + 2]);
						if (shape.mesh.indices[i * 3 + j].normal_index == -1)
						{
							// No normal, use the autogenerated
							model->m_normals[vertices_so_far + j] = glm::vec3(
								auto_normals[shape.mesh.indices[i * 3 + j].vertex_index]);
						}
						else
						{
							model->m_normals[vertices_so_far + j] =
								glm::vec3(attrib.normals[shape.mesh.indices[i * 3 + j].normal_index * 3 + 0],
									attrib.normals[shape.mesh.indices[i * 3 + j].normal_index * 3 + 1],
									attrib.normals[shape.mesh.indices[i * 3 + j].normal_index * 3 + 2]);
						}
						if (shape.mesh.indices[i * 3 + j].texcoord_index == -1)
						{
							// No UV coordinates. Use null.
							model->m_texture_coordinates[vertices_so_far + j] = glm::vec2(0.0f);
						}
						else
						{
							model->m_texture_coordinates[vertices_so_far + j] = glm::vec2(
								attrib.texcoords[shape.mesh.indices[i * 3 + j].texcoord_index * 2 + 0],
								attrib.texcoords[shape.mesh.indices[i * 3 + j].texcoord_index * 2 + 1]);
						}
					}
					vertices_so_far += 3;
				}
			}
			///////////////////////////////////////////////////////////////
			// Finalize and push this mesh to the list
			///////////////////////////////////////////////////////////////
			mesh.m_number_of_vertices = vertices_so_far - mesh.m_start_index;
			model->m_meshes.push_back(mesh);
			finished_materials[current_material_index] = true;
		}
		if (number_of_materials_in_shape == 1)
		{
			// If there's only one material, we don't need the material name in the mesh name
			model->m_meshes.back().m_name = shape.name;
		}
	}

	std::sort(model->m_meshes.begin(), model->m_meshes.end(),
		[](const Mesh& a, const Mesh& b) { return a.m_name < b.m_name; });

	///////////////////////////////////////////////////////////////////////
	// Upload to GPU
	///////////////////////////////////////////////////////////////////////
	glGenVertexArrays(1, &model->m_vaob);
	glBindVertexArray(model->m_vaob);
	glGenBuffers(1, &model->m_positions_bo);
	glBindBuffer(GL_ARRAY_BUFFER, model->m_positions_bo);
	glBufferData(GL_ARRAY_BUFFER, model->m_positions.size() * sizeof(glm::vec3), &model->m_positions[0].x,
		GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glGenBuffers(1, &model->m_normals_bo);
	glBindBuffer(GL_ARRAY_BUFFER, model->m_normals_bo);
	glBufferData(GL_ARRAY_BUFFER, model->m_normals.size() * sizeof(glm::vec3), &model->m_normals[0].x,
		GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(1);
	glGenBuffers(1, &model->m_texture_coordinates_bo);
	glBindBuffer(GL_ARRAY_BUFFER, model->m_texture_coordinates_bo);
	glBufferData(GL_ARRAY_BUFFER, model->m_texture_coordinates.size() * sizeof(glm::vec2),
		&model->m_texture_coordinates[0].x, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	MT_TRACE("Done!!!");
	return model;
}

void Model::saveModelToOBJ(Model* model, std::string filename)
{
	std::string directory;
	filename = file::normalise(filename);
	directory = file::parent_path(filename);
	filename = file::file_stem(filename);

	///////////////////////////////////////////////////////////////////////
	// Save Materials
	///////////////////////////////////////////////////////////////////////
	saveModelMaterialsToMTL(model, directory + filename + ".mtl");

	///////////////////////////////////////////////////////////////////////
	// Save geometry
	///////////////////////////////////////////////////////////////////////
	std::ofstream obj_file(directory + filename + ".obj");
	if (!obj_file.is_open())
	{
		std::cout << "Could not open file " << filename << " for writing.\n";
		return;
	}
	obj_file << "# Exported by Chalmers Graphics Group\n";
	obj_file << "mtllib " << filename << ".mtl\n";
	int vertex_counter = 1;
	for (auto mesh : model->m_meshes)
	{
		obj_file << "o " << mesh.m_name << "\n";
		obj_file << "g " << mesh.m_name << "\n";
		obj_file << "usemtl " << model->m_materials[mesh.m_material_idx].m_name << "\n";
		for (uint32_t i = mesh.m_start_index; i < mesh.m_start_index + mesh.m_number_of_vertices; i++)
		{
			obj_file << "v " << model->m_positions[i].x << " " << model->m_positions[i].y << " "
				<< model->m_positions[i].z << "\n";
		}
		for (uint32_t i = mesh.m_start_index; i < mesh.m_start_index + mesh.m_number_of_vertices; i++)
		{
			obj_file << "vn " << model->m_normals[i].x << " " << model->m_normals[i].y << " "
				<< model->m_normals[i].z << "\n";
		}
		for (uint32_t i = mesh.m_start_index; i < mesh.m_start_index + mesh.m_number_of_vertices; i++)
		{
			obj_file << "vt " << model->m_texture_coordinates[i].x << " " << model->m_texture_coordinates[i].y
				<< "\n";
		}
		int number_of_faces = mesh.m_number_of_vertices / 3;
		for (int i = 0; i < number_of_faces; i++)
		{
			obj_file << "f " << vertex_counter << "/" << vertex_counter << "/" << vertex_counter << " "
				<< vertex_counter + 1 << "/" << vertex_counter + 1 << "/" << vertex_counter + 1 << " "
				<< vertex_counter + 2 << "/" << vertex_counter + 2 << "/" << vertex_counter + 2 << "\n";
			vertex_counter += 3;
		}
	}
}

void Model::saveModelMaterialsToMTL(Model* model, std::string filename)
{
	std::ofstream mat_file(filename);
	if (!mat_file.is_open())
	{
		std::cout << "Could not open file " << filename << " for writing.\n";
		return;
	}
	mat_file << "# Exported by Chalmers Graphics Group\n";
	for (const auto& mat : model->m_materials)
	{
		mat_file << "newmtl " << mat.m_name << "\n";
		mat_file << "Kd " << mat.m_color.x << " " << mat.m_color.y << " " << mat.m_color.z << "\n";
		mat_file << "Ks " << mat.m_fresnel << " " << mat.m_fresnel << " " << mat.m_fresnel << "\n";
		mat_file << "Pm " << mat.m_metalness << "\n";
		mat_file << "Pr " << mat.m_shininess << "\n";
		mat_file << "Ke " << mat.m_emission.x << " " << mat.m_emission.y << " " << mat.m_emission.z << "\n";
		mat_file << "Tf " << mat.m_transparency << " " << mat.m_transparency << " " << mat.m_transparency
			<< "\n";
		if (mat.m_ior != 1.f)
		{
			mat_file << "Ni " << mat.m_ior << "\n";
		}
		if (mat.m_color_texture->IsLoaded())
			mat_file << "map_Kd " << mat.m_color_texture->GetFileName() << "\n";
		if (mat.m_metalness_texture->IsLoaded())
			mat_file << "map_Pm " << mat.m_metalness_texture->GetFileName() << "\n";
		if (mat.m_fresnel_texture->IsLoaded())
			mat_file << "map_Ks " << mat.m_fresnel_texture->GetFileName() << "\n";
		if (mat.m_shininess_texture->IsLoaded())
			mat_file << "map_Pr " << mat.m_shininess_texture->GetFileName() << "\n";
		if (mat.m_emission_texture->IsLoaded())
			mat_file << "map_Ke " << mat.m_emission_texture->GetFileName() << "\n";
		mat_file << "\n";
	}
	mat_file.close();
}

void Model::freeModel(Model* model)
{
	if (model != nullptr)
		delete model;
}

void Model::render(const Model* model, const Ref<Shader>& current_program, const bool submitMaterials, bool ssr)
{
	current_program->Bind();
	glBindVertexArray(model->m_vaob);
	for (auto& mesh : model->m_meshes)
	{
		if (submitMaterials)
		{
			const Material& material = model->m_materials[mesh.m_material_idx];

			bool has_color_texture = material.m_color_texture.get();
			bool has_metalness_texture = material.m_metalness_texture.get();
			bool has_fresnel_texture = material.m_fresnel_texture.get();
			bool has_shininess_texture = material.m_shininess_texture.get();
			bool has_emission_texture = material.m_emission_texture.get();
			if (has_color_texture)
			{
				material.m_color_texture->Bind();
			}
			if ( has_metalness_texture )
			{
				material.m_metalness_texture->Bind(2);
			}
			if ( has_fresnel_texture )
			{
				material.m_fresnel_texture->Bind(3);
			}
			if ( has_shininess_texture )
			{
				material.m_shininess_texture->Bind(4);
			}
			if (has_emission_texture)
			{
				material.m_emission_texture->Bind(5);
			}
			current_program->SetBool("has_color_texture", has_color_texture);
			current_program->SetBool("has_emission_texture", has_emission_texture);

			current_program->SetFloat3("material_color", material.m_color);
			current_program->SetFloat("material_metalness", material.m_metalness);
			if (ssr) {
				current_program->SetFloat("material_fresnel", 1.0f);
			}
			else {
				current_program->SetFloat("material_fresnel", material.m_fresnel);
			}
			current_program->SetFloat("material_shininess", material.m_shininess);
			current_program->SetFloat3("material_emission", material.m_emission);

			// Actually unused in the labs
			/*
			setUniformSlow( current_program, "has_metalness_texture", has_metalness_texture );
			setUniformSlow( current_program, "has_fresnel_texture", has_fresnel_texture );
			setUniformSlow( current_program, "has_shininess_texture", has_shininess_texture );
			*/
		}
		glDrawArrays(GL_TRIANGLES, mesh.m_start_index, (GLsizei)mesh.m_number_of_vertices);
	}
	glBindVertexArray(0);
}
extern const std::filesystem::path g_AssetPath;

static void DragdropTexture(const std::string& str,uint32_t compnum,Ref<Texture2D>& p_texture ) {
	
	ImGui::Button(str.c_str(), ImVec2(150.0f, 50.0f));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::filesystem::path texturePath = std::filesystem::path("assets") / path;
			Ref<Texture2D> texture = Texture2D::Create(texturePath.string(), compnum);
			if (texture->IsLoaded())
				p_texture = texture;
			else
				MT_WARN("Could not load texture {0}", texturePath.filename().string());
		}
		ImGui::EndDragDropTarget();
	}
}

void Model::OnImGuiRender(Model* model)
{
	
	size_t i = 0;
	for (auto& material:model->m_materials)
	{
		ImGuiTreeNodeFlags flags = 0 | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)i, flags, material.m_name.c_str());
		if(opened)
		{
			bool has_color_texture = material.m_color_texture.get();
			bool has_metalness_texture = material.m_metalness_texture.get();
			bool has_fresnel_texture = material.m_fresnel_texture.get();
			bool has_shininess_texture = material.m_shininess_texture.get();
			bool has_emission_texture = material.m_emission_texture.get();

			if (has_color_texture)
				DragdropTexture("Color texture", 4, material.m_color_texture);

			if (has_metalness_texture)
				DragdropTexture("Metalness texture", 1, material.m_metalness_texture);

			if (has_fresnel_texture)
				DragdropTexture("fresnel texture", 1, material.m_fresnel_texture);

			if (has_shininess_texture)
				DragdropTexture("Shininess texture", 1, material.m_shininess_texture);

			if (has_emission_texture)
				DragdropTexture("Emission texture", 4, material.m_emission_texture);

			ImGui::ColorEdit3("Color", &material.m_color.x);
			ImGui::SliderFloat("Metalness", &material.m_metalness, 0.0f, 1.0f);
			ImGui::SliderFloat("Fresnel", &material.m_fresnel, 0.0f, 1.0f);
			ImGui::SliderFloat("Shininess", &material.m_shininess, 0.0f, 5000.0f, "%.3f");
			ImGui::ColorEdit3("Emission", &material.m_emission.x);

			ImGui::TreePop();
		}
		i++;
	}
}


MT_NAMESPACE_END


