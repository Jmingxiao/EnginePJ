#pragma once
#include "Mint/Math/Transform.h"
#include "Mint/Core/UUID.h"

namespace Mint {

	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};
	struct TagComponent
	{
		std::string Tag = "Entity";
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent& operator =(const TagComponent&) = default;
		TagComponent(const std::string& sTag) :Tag(sTag)
		{}
	};

	struct TransformComponent
	{
		Transform transform;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent& operator =(const TransformComponent&) = default;

		operator glm::mat4() {
			return transform;
		}
	};
}