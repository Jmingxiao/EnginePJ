#pragma once

#include "reactphysics3d/reactphysics3d.h"
#include "Mint/Core/UUID.h"
#include "Mint/Render/Camera/SceneCamera.h"
#include "Mint/Physics/BasicShapes.h"
#include "Mint/Render/Model.h"
#include "Mint/Audio/MusicBuffer.h"

#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>



namespace Mint {

	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag{};

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translation{};
		glm::quat Rotation{ glm::quat(1,0,0,0)};
		glm::vec3 Scale{ glm::vec3(1.f) };

		operator glm::mat4() const
		{
			return GetTransform();
		}

		glm::mat4 GetTransform() const {

			glm::mat4 mat4Translation = glm::translate(glm::mat4(1.f), Translation);

			glm::mat4 mat4Rotation = GetRotation();

			glm::mat4 mat4Scale = glm::scale(glm::mat4(1.f), Scale);

			return mat4Translation * mat4Rotation * mat4Scale;
		}

		glm::mat4 GetRotation() const {
			return glm::toMat4(Rotation);
		}

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent& operator =(const TransformComponent&) = default;

		static void decomposeMtx(const glm::mat4& m, glm::vec3& pos, glm::quat& rot, glm::vec3& scale)
		{
			pos = m[3];
			for (int i = 0; i < 3; i++)
				scale[i] = glm::length(glm::vec3(m[i]));
			const glm::mat3 rotMtx(
				glm::vec3(m[0]) / scale[0],
				glm::vec3(m[1]) / scale[1],
				glm::vec3(m[2]) / scale[2]);
			rot = glm::toQuat(rotMtx);
		}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; // TODO: think about moving to Scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent& operator =(const CameraComponent&) = default;

	};


	struct MeshRendererComponent
	{	
		BuiltinShaderType s_type;

		Model* model;
		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;
		MeshRendererComponent( const std::string&p, const std::string name, BuiltinShaderType type= BuiltinShaderType::simple)
		{
			model = Model::loadModelFromOBJ(p);
			model->m_name = name;
			s_type = type;
		}
		std::vector<Material> GetMaterial() 
		{
			return model->m_materials;
		}
	};


	struct ScriptComponent
	{
		std::string ClassName;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	// Forward declaration
	class ScriptableEntity;

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	struct RigidBodyComponent
	{
		enum class BodyType
		{
			Static = 0, Dynamic, Kinematic
		};
		BodyType type = BodyType::Static;
		rp3d::RigidBody* m_rigidbody;
		RigidBodyComponent() = default;
		RigidBodyComponent(const RigidBodyComponent&) = default;
		RigidBodyComponent& operator =(const RigidBodyComponent&) = default;
		RigidBodyComponent(BodyType t):type(t){}
	};

	struct CollisionBodyComponent
	{
		rp3d::CollisionBody* m_collisionbody;
		CollisionBodyComponent() = default;
		CollisionBodyComponent(const CollisionBodyComponent&) = default;
		CollisionBodyComponent& operator =(const CollisionBodyComponent&) = default;
	};


	struct ColliderComponent
	{
		Geometry* m_geometry{};
		rp3d::Collider* m_collider;
		ColliderComponent() = default;
		ColliderComponent(const ColliderComponent&) = default;
		ColliderComponent& operator =(const ColliderComponent&) = default;
		ColliderComponent(Geometry* g) : m_geometry(g),m_collider(nullptr) {};
	};



	struct MusicComponent
	{
		MusicBuffer music;

		MusicComponent() = default;
		MusicComponent(const MusicComponent&) = default;
		MusicComponent(const std::string& path):
			music(MusicBuffer(path.c_str()))
		{
		}
	};

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent, MeshRendererComponent, ScriptComponent,
		NativeScriptComponent, CameraComponent,RigidBodyComponent, 
		CollisionBodyComponent,ColliderComponent,MusicComponent >;

}