#include "Mint/Render/Texture.h"
#include "ContentPenal.h"
#include <imgui/imgui.h>
#include "Mint/Core/Input.h"


namespace Mint {

extern const std::filesystem::path g_AssetPath = "assets";

ContentBrowserPanel::ContentBrowserPanel()
	: m_CurrentDirectory(g_AssetPath)
{
	m_DirectoryIcon = Texture2D::Create("resources/Icons/folder.png",4);
	m_FileIcon = Texture2D::Create("resources/Icons/file.png",4);
}

void ContentBrowserPanel::OnImGuiRender()
{
	ImGui::Begin("Content Browser",nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))
	{
		if (ImGui::Button("<-"))
		{
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}
	}

	float cellSize = thumbnailSize + padding;

	ImGui::SameLine(ImGui::GetWindowWidth() - 300);
	ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 64, 128);
	isFocused = ImGui::IsWindowFocused();

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;

	ImGui::Columns(columnCount, 0, false);

	for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
	{
		const auto& path = directoryEntry.path();
		std::string filenameString = path.filename().string();

		ImGui::PushID(filenameString.c_str());
		Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::ImageButton((void*)(size_t)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropSource())
		{
			auto relativePath = std::filesystem::relative(path, g_AssetPath);
			const wchar_t* itemPath = relativePath.c_str();
			ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
			ImGui::EndDragDropSource();
		}

		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (directoryEntry.is_directory())
				m_CurrentDirectory /= path.filename();

		}
		ImGui::TextWrapped(filenameString.c_str());

		ImGui::NextColumn();

		ImGui::PopID();
	}

	ImGui::Columns(1);

	
	//ImGui::SliderFloat("Padding", &padding, 0, 32);

	// TODO: status bar
	ImGui::End();
}

void ContentBrowserPanel::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseScrolledEvent>(MT_BIND_EVENT_FN(ContentBrowserPanel::OnMouseScroll));
}

bool ContentBrowserPanel::OnMouseScroll(MouseScrolledEvent& e)
{
	if (isFocused && Input::IsKeyPressed(KeyCode::LeftControl)) {
		float delta = e.GetYOffset() * 0.5f;
		thumbnailSize -= delta;
		thumbnailSize = std::max(thumbnailSize, 64.0f);
		thumbnailSize = std::min(thumbnailSize, 128.0f);
	}
	return false;
}

}