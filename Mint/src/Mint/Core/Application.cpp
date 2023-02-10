#include "pch.h"
#include "Application.h"

#include "Log.h"

#include <glad/glad.h>
#include "Input.h"

namespace Mint {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_instance = nullptr;

	Application::Application()
	{
		MT_ASSERT(!s_instance, "Application already exist!");
		s_instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
		
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer) 
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnwindowClose));

		//MT_TRACE("{0}", e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}
	void Application::Run()
	{
		while (m_Running)
		{
			float time = Timestep::GetTime();
			Timestep timestep = time - m_lastFrameTime;
			m_lastFrameTime = time;

			for (Layer* layer : m_LayerStack) 
				layer->OnUpdate(timestep);

			m_ImGuiLayer->Begin();
			{
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}
	void Application::Close() {
		m_Running = false;
	}

	bool Application::OnwindowClose(WindowCloseEvent& e)
	{
		m_Running = false;

		return true;
	}
}


