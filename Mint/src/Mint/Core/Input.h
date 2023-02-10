#pragma once
#include "Mint/Core/KeyCodes.h"
#include <glm/glm.hpp>

namespace Mint {

	class  Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);
		
		static bool IsMouseButtonPressed(Mouse button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();

	};
}