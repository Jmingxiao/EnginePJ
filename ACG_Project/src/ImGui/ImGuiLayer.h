#pragma once

#include "Common/Layer.h"

namespace Acg {

	class  ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() ;
		virtual void OnDetach() ;
		virtual void OnImGuiRender();
		virtual void OnEvent(Event& event);

		void Begin();
		void End();

		void SetupImGuiDarkTheme();

	private:

	private:
		float m_Time = 0.0f;

	};
}