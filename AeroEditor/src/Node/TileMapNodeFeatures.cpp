#include <Node/TileMapNodeFeatures.hpp>

namespace ag
{
	void TileMapNodeFeatures::tilemap_editor()
	{

	}

	void TileMapNodeFeatures::texture_selector_gui(const AG_ref<Texture2D>& texture, uint_rect& texture_rect)
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse;


		ImGui::Begin("Texture Selector", nullptr, flags);
		{
			UI::windowed_titlebar("Texture Selector");

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 50);
			ImVec2 image_size(texture->get_size().x, texture->get_size().y);
			ImGui::Image((void*)texture->get_texture_id(), image_size);

		}
		ImGui::End();
	}
}