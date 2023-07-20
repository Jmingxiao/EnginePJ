#pragma once
#include "OpenGL/model.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/FrameBuffer.h"
#include "GLFW/glfw3.h"
//utilities 
#include "Window.h"
#include "Layer.h"
#include "Event.h"
#include "LayerStack.h"
#include "ImGui/ImGuiLayer.h"


namespace Acg {
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void Close();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& Get() { return *s_instance; }
		inline Window& GetWindow() { return *m_Window; }
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
	private:
		bool OnwindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		bool m_Minimized = false;
		float m_lastFrameTime = 0.0f;

		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;

	private:
		static Application* s_instance;

	};

	//To be defined in client 
	Application* CreateApplication();


}