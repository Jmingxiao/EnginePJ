#include "pch.h"
#include "GraphicContext.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>


Acg::GraphicContext::GraphicContext(GLFWwindow* windowhandle)
	:m_windowhandle(windowhandle)
{
	LOG_ASSERT(windowhandle, "window handle is null!");
}

void Acg::GraphicContext::Init()
{

	glfwMakeContextCurrent(m_windowhandle);
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	LOG_ASSERT(status, "Failed to initialize Glad!");

	LOG_INFO("OpenGL Info:");
	LOG_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
	LOG_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
	LOG_INFO("  Version: {0}", glGetString(GL_VERSION));
	LOG_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "ACG project requires at least OpenGL version 4.5!");
}

void Acg::GraphicContext::SwapBuffers()
{
	glfwSwapBuffers(m_windowhandle);
}
