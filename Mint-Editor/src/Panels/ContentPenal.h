#pragma once


#include <filesystem>
#include "Mint/Events/Event.h"
#include "Mint/Events/MouseEvent.h"


namespace Mint {

	class Texture2D;
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
		void OnEvent(Event& event);
	private:
		bool OnMouseScroll(MouseScrolledEvent& e);
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
		float padding = 20.0f;
		float thumbnailSize = 100.0f;
		bool  isFocused = false;
	};

}