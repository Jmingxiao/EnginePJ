#pragma once
#include"glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct Transform
{
	glm::vec3 Translation = glm::vec3(0.f);
	glm::vec3 Rotation = glm::vec3(0.f);
	glm::vec3 Scale = glm::vec3(1.f);
	Transform() = default;
	Transform(const Transform&) = default;
	Transform& operator =(const Transform&) = default;
	Transform(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale) :
		Translation(translation),
		Rotation(rotation),
		Scale(scale)
	{}

	operator glm::mat4()
	{
		glm::mat4 mat4Translation = glm::translate(glm::mat4(1.f), Translation);

		glm::mat4 mat4Rotation = glm::rotate(glm::mat4(1.f), Rotation.x, glm::vec3(1.0, 0.0, 0.0)) *
			glm::rotate(glm::mat4(1.f), Rotation.y, glm::vec3(0.0, 1.0, 0.0)) *
			glm::rotate(glm::mat4(1.f), Rotation.z, glm::vec3(0.0, 0.0, 1.0));

		glm::mat4 mat4Scale = glm::scale(glm::mat4(1.f), Scale);

		return mat4Translation * mat4Rotation * mat4Scale;
	}
};