#pragma once

#include <reactphysics3d/mathematics/mathematics.h>
#include<glm/gtc/quaternion.hpp>
#include "Mint/Physics/BasicShapes.h"

namespace Mint {

	using reactphysics3d::Vector3;
	using reactphysics3d::Vector2;
	using reactphysics3d::Quaternion;
	using reactphysics3d::Matrix3x3;


	struct TransformComponent;

	inline Vector2 toVec2(const glm::vec2& v) { return { v.x, v.y }; }
	inline glm::vec2 toVec2(const Vector2& v) { return { v.x, v.y }; }

	inline Vector3 toVec3(const glm::vec3& v) { return { v.x,v.y,v.z }; }
	inline glm::vec3 toVec3(Vector3 v) { return { v.x, v.y, v.z }; }


	inline Quaternion toQuat(glm::quat q) { return { q.x, q.y, q.z, q.w }; }
	inline glm::quat toQuat(Quaternion q) { return { q.w, q.x, q.y, q.z }; }

	Matrix3x3 toMat3(const glm::mat3& m);

	glm::mat3 toMat3(const Matrix3x3& m);
	
	rp3d::Transform toTrans(const TransformComponent& trans);

	glm::vec3 boxScale(const Box&box, const glm::vec3& scale);
	float sphereScale(const Sphere& sphere, const glm::vec3& scale);
	glm::vec2 capsuleScale(const Capsule& capsule, const glm::vec3& scale);

}