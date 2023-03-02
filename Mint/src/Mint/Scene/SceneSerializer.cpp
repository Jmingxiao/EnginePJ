#include "pch.h"
#include "SceneSerializer.h"
#include "Mint/Core/UUID.h"
#include "Mint/Scene/Entity.h"

#include <fstream>
#pragma warning(push)
#pragma warning( disable: 4251 )
#pragma warning( disable: 4275 )
#include <yaml-cpp/yaml.h>
#pragma warning(pop)

namespace YAML {
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::quat>
	{
		static Node encode(const glm::quat& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::quat& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};


	template<>
	struct convert<Mint::UUID>
	{
		static Node encode(const Mint::UUID& uuid)
		{
			Node node;
			node.push_back((uint64_t)uuid);
			return node;
		}

		static bool decode(const Node& node, Mint::UUID& uuid)
		{
			uuid = node.as<uint64_t>();
			return true;
		}
	};


}


namespace Mint {

#define WRITE_SCRIPT_FIELD(FieldType, Type)           \
			case ScriptFieldType::FieldType:          \
				out << scriptField.GetValue<Type>();  \
				break

#define READ_SCRIPT_FIELD(FieldType, Type)             \
	case ScriptFieldType::FieldType:                   \
	{                                                  \
		Type data = scriptField["Data"].as<Type>();    \
		fieldInstance.SetValue(data);                  \
		break;                                         \
	}


	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::quat& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}



SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
	: m_Scene(scene)
{
}

static void SerializeEntity(YAML::Emitter& out, Entity entity) {
	MT_ASSERT(entity.HasComponent<IDComponent>());
	out << YAML::BeginMap; // Entity
	out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

	if (entity.HasComponent<TagComponent>())
	{
		out << YAML::Key << "TagComponent";
		out << YAML::BeginMap; // TagComponent

		auto& tag = entity.GetComponent<TagComponent>().Tag;
		out << YAML::Key << "Tag" << YAML::Value << tag;

		out << YAML::EndMap; // TagComponent
	}

	if (entity.HasComponent<TransformComponent>())
	{
		out << YAML::Key << "TransformComponent";
		out << YAML::BeginMap; // TransformComponent

		auto& tc = entity.GetComponent<TransformComponent>();
		out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
		out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
		out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

		out << YAML::EndMap; // TransformComponent
	}

	if (entity.HasComponent<CameraComponent>())
	{
		out << YAML::Key << "CameraComponent";
		out << YAML::BeginMap; // CameraComponent

		auto& cameraComponent = entity.GetComponent<CameraComponent>();
		auto& camera = cameraComponent.Camera;

		out << YAML::Key << "Camera" << YAML::Value;
		out << YAML::BeginMap; // Camera
		out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
		out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
		out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
		out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
		out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
		out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
		out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
		out << YAML::EndMap; // Camera

		out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
		out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

		out << YAML::EndMap; // CameraComponent
	}

	//MeshComponent
	if (entity.HasComponent<MeshRendererComponent>())
	{
		out << YAML::Key << "MeshComponent";
		out << YAML::BeginMap;

		auto& meshComponent = entity.GetComponent<MeshRendererComponent>();
		if (meshComponent.model) {
			out << "ModelPath" << YAML::Value << meshComponent.m_path;
			out << "ModelName" << YAML::Value << meshComponent.model->m_name;
		}
		out << "ModelShaderType" << YAML::Value << (int)meshComponent.s_type;
			
		out << YAML::EndMap; // MeshComponent
	}// MeshComponent

	if (entity.HasComponent<RigidBodyComponent>())
	{
		out << YAML::Key << "RigidBodyComponent";
		out << YAML::BeginMap;

		auto& rbcomponent = entity.GetComponent<RigidBodyComponent>();
		out << "RigidBodyType" << YAML::Value << static_cast<int>(rbcomponent.type);

		out << YAML::EndMap; // RigidBodyComponent
	}// RigidBodyComponent


	if (entity.HasComponent<CollisionBodyComponent>())
	{
		out << YAML::Key << "CollisionBodyComponent";
		out << YAML::BeginMap;

		out << YAML::EndMap; // CollisionBodyComponent
	}// CollisionBodyComponent

	if (entity.HasComponent<ColliderComponent>())
	{
		out << YAML::Key << "ColliderComponent";
		out << YAML::BeginMap;

		auto& collider = entity.GetComponent<ColliderComponent>();
		auto shape=collider.m_geometry->GetShape();
		out << "ColliderType" << YAML::Value << (int)shape;
		switch (shape) {
		case Mint::Gshape::box:
		{
			Box* box = (Box*)collider.m_geometry;
			out << "HalfExtent"<< YAML::Value << box->m_half_extents;
			break;
		}
		case Mint::Gshape::sphere:
		{
			Sphere* sp = (Sphere*)collider.m_geometry;
			out << "Radius" << YAML::Value << sp->m_radius;
			break;
		}
		case Mint::Gshape::capsule:
		{
			Capsule* cp = (Capsule*)collider.m_geometry;
			out << "Radius" << YAML::Value << cp->m_radius;
			out << "HalfHeight" << YAML::Value << cp->m_half_height;
			break;
		}
		}

		out << YAML::EndMap; // ColliderComponent
	}// ColliderComponent


	if (entity.HasComponent<MusicComponent>())
	{
		out << YAML::Key << "MusicComponent";
		out << YAML::BeginMap;
		auto& music = entity.GetComponent<MusicComponent>();
		out << "MusicPath" << YAML::Value << music.m_path;
		out << YAML::EndMap; // ColliderComponent
	}// musicComponent

	   
	out << YAML::EndMap; // Entity

}

void SceneSerializer::Serialize(const std::string& filepath)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Untitled";
	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
	m_Scene->m_registry.each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene.get() };
	if (!entity)
		return;

	SerializeEntity(out, entity);
		});
	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();
}

void SceneSerializer::SerializeRuntime(const std::string& filepath)
{
	//not implemented 
}
bool SceneSerializer::Deserialize(const std::string& filepath)
{
	YAML::Node data;
	try
	{
		data = YAML::LoadFile(filepath);
	}
	catch (YAML::ParserException e)
	{
		MT_ERROR("Failed to load .mint file '{0}'\n     {1}", filepath, e.what());
		return false;
	}

	if (!data["Scene"])
		return false;

	std::string sceneName = data["Scene"].as<std::string>();
	MT_TRACE("Deserializing scene '{0}'", sceneName);

	auto entities = data["Entities"];
	if (entities)
	{
		for (auto entity : entities)
		{
			uint64_t uuid = entity["Entity"].as<uint64_t>();

			std::string name;
			auto tagComponent = entity["TagComponent"];
			if (tagComponent)
				name = tagComponent["Tag"].as<std::string>();

			MT_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

			Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

			auto transformComponent = entity["TransformComponent"];
			if (transformComponent)
			{
				// Entities always have transforms
				auto& tc = deserializedEntity.GetComponent<TransformComponent>();
				tc.Translation = transformComponent["Translation"].as<glm::vec3>();
				tc.Rotation = transformComponent["Rotation"].as<glm::quat>();
				tc.Scale = transformComponent["Scale"].as<glm::vec3>();
			}

			YAML::Node cameraComponent = entity["CameraComponent"];
			if (cameraComponent)
			{
				auto& cc = deserializedEntity.AddComponent<CameraComponent>();

				auto& cameraProps = cameraComponent["Camera"];
				cc.Camera.SetProjectionType((ProjectionType)cameraProps["ProjectionType"].as<int>());

				cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
				cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
				cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

				cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
				cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
				cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

				cc.Primary = cameraComponent["Primary"].as<bool>();
				cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
			}

			auto meshComponent = entity["MeshComponent"];
			if (meshComponent) {
				auto& meshc = deserializedEntity.AddComponent<MeshRendererComponent>();
				std::string path = meshComponent["ModelPath"].as<std::string>();
				meshc.model = Model::loadModelFromOBJ(path);
				meshc.m_path = path;
				meshc.model->m_name = meshComponent["ModelName"].as<std::string>();
				meshc.s_type = static_cast<BuiltinShaderType>(meshComponent["ModelShaderType"].as<int>());
			}

			auto rigidbodyComponent = entity["RigidBodyComponent"];
			if (rigidbodyComponent) {
				auto& rbc = deserializedEntity.AddComponent<RigidBodyComponent>();
				rbc.type = static_cast<RigidBodyComponent::BodyType>(rigidbodyComponent["RigidBodyType"].as<int>());
			}

			auto collisionbodyComponent = entity["CollisionBodyComponent"];
			if (collisionbodyComponent) {
				auto& cbc = deserializedEntity.AddComponent<CollisionBodyComponent>();
			}

			auto colliderComponent = entity["ColliderComponent"];
			if (colliderComponent) {
				auto& cc = deserializedEntity.AddComponent<ColliderComponent>();
				auto shape =static_cast<Gshape>(colliderComponent["ColliderType"].as<int>());
				switch (shape)
				{
				case Mint::Gshape::box:
				{
					Box* box = new Box();
					box->m_half_extents = colliderComponent["HalfExtent"].as<glm::vec3>();
					cc.m_geometry = box;
					break;
				}
				case Mint::Gshape::sphere:
				{
					Sphere* sp = new Sphere();
					sp->m_radius = colliderComponent["Radius"].as<float>();
					break;
				}
				case Mint::Gshape::capsule:
				{
					Capsule* cp = new Capsule();
					cp->m_radius = colliderComponent["Radius"].as<float>();
					cp->m_half_height = colliderComponent["HalfHeight"].as<float>();
					break;
				}
				}
			}
			auto musicComponent = entity["MusicComponent"];
			if (musicComponent) {
				auto& mc = deserializedEntity.AddComponent<MusicComponent>();
				mc.m_path = musicComponent["MusicPath"].as<std::string>();
				mc.music = CreateRef<MusicBuffer>(mc.m_path.c_str());
			}
		}
	}
	return true;
}

bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
{
	//not implemented
	return false;
}

}