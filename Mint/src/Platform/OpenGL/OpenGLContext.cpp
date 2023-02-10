#include "pch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Mint
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowhandle)
		:m_windowhandle(windowhandle)
	{
		MT_ASSERT(windowhandle, "window handle is null!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_windowhandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		MT_ASSERT(status, "Failed to initialize Glad!");

		MT_INFO("OpenGL Info:");
		MT_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		MT_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		MT_INFO("  Version: {0}", glGetString(GL_VERSION));
		MT_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Hazel requires at least OpenGL version 4.5!");

	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_windowhandle);
	}

}