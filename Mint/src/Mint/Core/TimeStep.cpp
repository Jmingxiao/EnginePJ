#include "pch.h"
#include "TimeStep.h"
#include <GLFW/glfw3.h>

float Mint::Timestep::GetTime()
{
    return static_cast<float>(glfwGetTime());
}
