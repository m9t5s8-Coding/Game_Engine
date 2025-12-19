#include <UI/UI.hpp>
#include <Aero.hpp>
#include <Application/EditorLayer.hpp>
#include <Panels/ScenePanel.hpp>

namespace ag::UI
{
	void draw_menu_bar()
	{
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
        if (ImGui::MenuItem("New Scene", "Ctrl + N")) 
        {
          EditorLayer::get().create_new_scene();
        }
        if (ImGui::MenuItem("Open Scene", "Ctrl + O")) 
        {
          EditorLayer::get().open_scene();
        }
        if (ImGui::MenuItem("Save Scene", "Ctrl + S"))
        {
          EditorLayer::get().save_scene();
        }
        if (ImGui::MenuItem("Save Scene As", "Ctrl + Shift + S"))
        {

        }
        ImGui::Separator();
        if (ImGui::MenuItem("Exit")) {}
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Edit"))
      {
        ImGui::MenuItem("Undo");
        ImGui::MenuItem("Redo");
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("View"))
      {
        ImGui::MenuItem("Scene");
        ImGui::MenuItem("Inspector");
        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }
	}

  bool texture_selector(const AG_ref<Texture>& texture, uint_rect& texture_rect)
  {
    static bool selecting = false;
    static bool has_selection = false;
    static vec2f select_start, select_end;
    static vec2f drag_start, drag_current;
    static bool dragging = false;
    
    bool selected = false;

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::Begin("Texture Selector", nullptr, flags);
    {
      // Zoom slider
      static float zoom = 1.0f;
      float zoom_speed = 0.3f;
      if (ImGui::IsWindowHovered())
      {
        zoom += ImGui::GetIO().MouseWheel * zoom_speed;
        zoom = std::clamp(zoom, 0.1f, 10.0f);;
      }

      // Compute image size & centering offset
      vec2f image_size(texture->get_size().x * zoom, texture->get_size().y * zoom);
      vec2f available_size(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
      static vec2f center = available_size / 2;
      if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
      {
        dragging = true;
        drag_start = ImGui::GetMousePos();
        drag_current = drag_start;
      }
      if (dragging)
      {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Middle) && ImGui::IsWindowHovered())
        {
          drag_current = ImGui::GetMousePos();
          vec2f delta = drag_current - drag_start;

          center += delta;
          drag_start = drag_current;
        }
      }
      else
      {
        dragging = false;
      }


      vec2f offset = center - (image_size) * 0.5f;

      ImGui::SetCursorPos(offset.to_imvec2());

      // Compute image screen rectangle
      vec2f image_min;
      image_min.to_vec2(ImGui::GetCursorScreenPos());
      vec2f image_max = image_min + image_size;

      ImDrawList* dl = ImGui::GetWindowDrawList();

      // Background
      dl->AddRectFilled(image_min.to_imvec2(), image_max.to_imvec2(), IM_COL32(45, 45, 45, 255));
      dl->AddRect(image_min.to_imvec2(), image_max.to_imvec2(), IM_COL32(90, 90, 90, 255));

      // Draw texture
      ImGui::Image((void*)texture->get_texture_id(), image_size.to_imvec2());

      // Handle mouse selection
      if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
      {
        selecting = true;
        has_selection = false;
        select_start.to_vec2(ImGui::GetMousePos());
        select_end = select_start;
      }

      if (selecting)
      {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
          select_end.to_vec2(ImGui::GetMousePos());
        }
        else
        {
          selecting = false;
          has_selection = true;
        }

        dl->AddRect(select_start.to_imvec2(), select_end.to_imvec2(), IM_COL32(255, 200, 0, 255), 0.0f, 0, 2.0f);
      }

      // Convert selection to texture pixels
      const uint32_t min_size = 2; // minimum size in pixels
      if (has_selection)
      {
        vec2f sel_min(std::min(select_start.x, select_end.x), std::min(select_start.y, select_end.y));
        vec2f sel_max(std::max(select_start.x, select_end.x), std::max(select_start.y, select_end.y));

        // Clamp to image bounds
        sel_min.x = std::clamp(sel_min.x, image_min.x, image_max.x);
        sel_min.y = std::clamp(sel_min.y, image_min.y, image_max.y);
        sel_max.x = std::clamp(sel_max.x, image_min.x, image_max.x);
        sel_max.y = std::clamp(sel_max.y, image_min.y, image_max.y);

        // Convert to pixel coordinates
        vec2f pixel_min = (sel_min - image_min) / zoom;
        vec2f pixel_max = (sel_max - image_min) / zoom;

        // Pixel-perfect rounding
        uint32_t x0 = (uint32_t)std::floor(pixel_min.x);
        uint32_t y0 = (uint32_t)std::floor(pixel_min.y);
        uint32_t x1 = (uint32_t)std::ceil(pixel_max.x);
        uint32_t y1 = (uint32_t)std::ceil(pixel_max.y);

        uint32_t width = x1 - x0;
        uint32_t height = y1 - y0;

        if (width >= min_size && height >= min_size)
        {
          texture_rect.position.x = x0;
          texture_rect.position.y = y0;
          texture_rect.size.x = width;
          texture_rect.size.y = height;
          selected = true;
          select_start = vec2f();
          select_end = vec2f();
        }
      }
    }
    ImGui::End();

    return selected;
  }

}