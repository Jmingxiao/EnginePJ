#pragma once
#include "OpenGL/GraphicContext.h"
#include "Event.h"
#include <GLFW/glfw3.h>

namespace Acg {

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "ACG Project",
			uint32_t width = 1600,
			uint32_t height = 900)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window(const WindowProps& props = WindowProps());
		 ~Window();

		void OnUpdate() ;

		unsigned int GetWidth() const  { return m_Data.Width; }
		unsigned int GetHeight() const  { return m_Data.Height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback)  { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) ;
		bool IsVSync() const ;

		 void* GetNativeWindow() const { return m_Window; }
	private:
		 void Init(const WindowProps& props);
		 void Shutdown();
	private:
		GLFWwindow* m_Window;
		GraphicContext* m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;
			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};



}