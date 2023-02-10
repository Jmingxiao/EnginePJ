#pragma once

#include "Mint/Core/Base.h"
#include "Mint/Events/Event.h"
#include "Mint/Events/ApplicationEvent.h"
#include "Mint/Core/LayerStack.h"
#include "Window.h"
#include "Mint/Core/Timestep.h"

#include "Mint/ImGui/ImGuiLayer.h"

//render 
#include"Mint/Render/VertexArray.h"
#include"Mint/Render/Shader.h"
#include"Mint/Render/Buffer.h"

namespace Mint {

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
		bool OnwindowClose( WindowCloseEvent& e);
		std::unique_ptr<Window> m_Window;
		bool m_Running =true;
		float m_lastFrameTime = 0.0f;

		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;
	private:
		static Application* s_instance;
		
	};
	
	//To be defined in client 
	Application* CreateApplication();

}
