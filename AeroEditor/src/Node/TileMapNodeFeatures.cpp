#include <Node/TileMapNodeFeatures.hpp>

namespace ag
{
  struct TextureRegister
  {
    vec2u size;
    vec2u offset;

  };

	void TileMapNodeFeatures::tilemap_editor()
	{

	}

  void TileMapNodeFeatures::register_tile(Entity entity)
  {
    TextureRegister texture_register;

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking |
      ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Tile Register", nullptr, flags);
    {
      UI::windowed_titlebar("Texture Selector");

      if (ImGui::BeginTable("TileEditorTable", 2, ImGuiTableFlags_BordersInner | ImGuiTableFlags_SizingFixedFit))
      {
        float window_width = ImGui::GetContentRegionAvail().x;

        ImGui::TableSetupColumn("Left", ImGuiTableColumnFlags_WidthFixed, window_width * 0.7f);
        ImGui::TableSetupColumn("Right", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextColumn();
        ImGui::Text("Tileset Selector");

        auto& props = entity.get_component<TileMapNode::TileMapProp>();
        auto& texture = props.texture;

        ImVec2 image_size(texture->get_size().x, texture->get_size().y);
        ImGui::Image((void*)texture->get_texture_id(), image_size);

        // ======================
        //  TILE SELECTION LOGIC
        // ======================

        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        ImVec2 mousePos = ImGui::GetMousePos();

        bool mouseOverImage =
          mousePos.x >= min.x && mousePos.x <= max.x &&
          mousePos.y >= min.y && mousePos.y <= max.y;

        if (mouseOverImage && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
          /*int localX = (int)(mousePos.x - min.x);
          int localY = (int)(mousePos.y - min.y);

          props.selectedX = localX / texture_register.size;
          props.selectedY = localY / texture_register.size;

          uint_rect rect;
          rect.position.x = props.selectedX * props.tileWidth;
          rect.position.y = props.selectedY * props.tileHeight;
          rect.size.x = props.tileWidth;
          rect.size.y = props.tileHeight;

          TileTexture texture_entry;
          texture_entry.texture_pos = { props.selectedX, props.selectedY };
          texture_entry.texture_rect = rect;

          props.tile_textures[{props.selectedX, props.selectedY}] = texture_entry;*/
        }

        // ======================
        //  HIGHLIGHT SELECTED TILE
        // ======================
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        /*drawList->AddRect(
          ImVec2(min.x + props.selectedX * props.tileWidth,
            min.y + props.selectedY * props.tileWidth),
          ImVec2(min.x + (props.selectedX + 1) * props.tileWidth,
            min.y + (props.selectedY + 1) * props.tileHeight),
          IM_COL32(255, 0, 0, 255),   
          0.0f,
          0,
          2.0f                  
        );*/

        // ======================
        //  RIGHT PANEL
        // ======================

        ImGui::TableNextColumn();
        ImGui::Text("Tile Map / Properties");
        ImGui::Separator();

       /* ImGui::Text("Selected Tile:");
        ImGui::Text("Grid: (%d, %d)", props.selectedX, props.selectedY);
        ImGui::Text("Rect: %d %d %d %d",
          props.selectedRect.left,
          props.selectedRect.top,
          props.selectedRect.width,
          props.selectedRect.height);*/

        ImGui::EndTable();
      }
    }
    ImGui::End();
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