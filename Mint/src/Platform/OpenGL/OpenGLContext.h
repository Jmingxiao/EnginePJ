#pragma once
#include"Mint/Render/GraphicContext.h"

struct GLFWwindow;

namespace Mint {
	class OpenGLContext :public GraphicContext 
	{
	public :

		OpenGLContext(GLFWwindow* windowhandle);

		virtual	void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_windowhandle;

	};




}