#pragma once

#include "Mint/Core/Layer.h"

namespace Mint {

	class  ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
		void SetDarkThemeColors();
		void SetupImGuiDarkTheme();

	private:

	private:
		float m_Time = 0.0f;
		bool m_BlockEvents = true;

	};
}