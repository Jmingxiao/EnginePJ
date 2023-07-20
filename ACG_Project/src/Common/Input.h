#pragma once
#include "Keycode.h"
#include <glm/glm.hpp>

namespace Acg {

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