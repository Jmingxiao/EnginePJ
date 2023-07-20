#pragma once

struct GLFWwindow;
namespace Acg
{
	class GraphicContext
	{
	public:
		GraphicContext(GLFWwindow* windowhandle);

		void Init() ;
		void SwapBuffers() ;

	private:
		GLFWwindow* m_windowhandle;
	};

}