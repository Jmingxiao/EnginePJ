#pragma once

#include "Hazel/Core.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/LayerStack.h"
#include "Window.h"


namespace Hazel {

	class HAZEL_API Application
	{
	public:
		Application();
		virtual ~Application();
		
		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& Get() { return *s_instance; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		bool OnwindowClose( WindowCloseEvent& e);
		std::unique_ptr<Window> m_Window;
		bool m_Running =true;
		LayerStack m_LayerStack;
	private:
		static Application* s_instance;
	};
	
	//To be defined in client 
	Application* CreateApplication();

}

