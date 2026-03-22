#pragma once

#include <imgui.h>

#include <Core/Application.hpp>
#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Scene/Scene.hpp>
#include <string>
#include <type_traits>
#include <UI/Icons.hpp>

namespace ag
{

struct PropertyStyle
{
  float       min_width          = 100.0f;
  float       speed              = 1.0f;
  const char* format             = "%.3f";
  bool        show_reset_buttons = true;
  bool        show_labels        = true;
  bool        drag_enabled       = true;
  bool        slider_enabled     = false;
  float       slider_min         = 0.0f;
  float       slider_max         = 1.0f;
};
struct Panels
{
  bool properties_panel       = true;
  bool texture_selector       = false;
  bool scene_panel            = true;
  bool tilemap_selector       = false;
  bool console_panel          = true;
  bool animation_selector     = false;
  bool tilemap_register       = false;
  bool auto_tiling_register   = false;
  bool save_changes_panel     = false;
  bool create_new_scene       = false;
  bool create_new_script      = false;
  bool project_settings_panel = false;
};

struct GUI_Button
{
  std::string label;
  bool        enabled = true;
  bool        active  = false;
  vec2f       size;
};
struct Script_Component;
class UI
{
public:
  static void draw_texture(Entity entity);
  static void draw_window();
  static void draw_animation(Entity entity);
  static void draw_frame_selector(Entity entity, const std::string& current_animation);

  static void        draw_tilemap_register(Entity entity);
  static void        draw_autotiling_register(Entity entity);
  static bool        texture_selector(Entity entity, uint_rect& texture_rect);
  static void        content_browser();
  static bool        is_right_file(const std::filesystem::path& path);
  static const char* get_file_icon(const std::string& extension);
  static bool        is_image(const std::filesystem::path& path);
  static void        draw_folder_node(const std::filesystem::path& directory, int depth = 0);

  static void draw_script_selector(Entity entity);
  static void draw_loaded_script_panel(Entity entity, Script_Component& props);
  static void draw_script_drop_zone(Entity entity, Script_Component& props);
  static void draw_script_actions(Entity entity, Script_Component& props, bool has_script);
  static void open_in_vscode(const std::string& script_path);
  static void show_in_explorer(const std::string& file_path);
  static void script_drag_drop(std::string& path);

  static bool
  draw_tilemap_selector(Entity entity, vec2u& id, std::string& set_name, bool& use_autotile);

  static void create_new_scene();
  static void create_new_script(Entity entity);
  static void save_changes();
  static void popup_functions();
  static void project_settings();

  static void draw_project_settings_general();
  static void draw_project_settings_graphics();
  static void draw_project_settings_audio();
  static void draw_project_settings_physics();
  static void draw_project_settings_scripting();
  static void draw_project_settings_networking();

  static void draw_settings_title(const char* title);

  static bool draw_button(const GUI_Button& btn);
  static void test_popup(Entity entity);
  static void custom_popup(const std::string&    popup_id,
                           const std::string&    popup_name,
                           std::function<void()> draw_content,
                           std::function<void()> close);
  static void draw_menu_bar();
  static void run_current_scene();
  static void draw_console();

  static Panels& get_uistate_panels()
  {
    return s_show_panels;
  }

  static void draw_title(const char* label)
  {
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::SeparatorText(label);
    ImGui::Spacing();
  }

  // Helper functions
  inline static void
  begin_property(const char* label, const char* tooltip = nullptr, bool same_line = true)
  {
    const float label_width = s_property_style.min_width;

    ImGui::PushID(label);

    if (s_property_style.show_labels)
    {
      ImGui::Columns(2, nullptr, false);
      ImGui::SetColumnWidth(0, label_width);

      // Better vertical alignment
      ImGui::AlignTextToFramePadding();
      // ImGui::TextColored(s_property_style.label_color, "%s", label);
      ImGui::Text("%s", label);

      if (tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
      {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(tooltip);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
      }

      ImGui::NextColumn();
      ImGui::PushItemWidth(-1);
    }
  }

  inline static void end_property()
  {
    if (s_property_style.show_labels)
    {
      ImGui::PopItemWidth();
      ImGui::Columns(1);
    }
    ImGui::PopID();
  }

  inline static bool draw_reset_button(const char* label)
  {
    const float  line_height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
    const ImVec2 button_size = {line_height + 3.0f, line_height};
    const bool   clicked     = ImGui::Button(label, button_size);
    return clicked;
  }

  template <typename T>
  inline static bool draw_drag_scalar(const char*   id,
                                      T*            value,
                                      ImGuiDataType data_type,
                                      float         speed  = 1.0f,
                                      const char*   format = "%.3f")
  {
    if (s_property_style.drag_enabled)
    {
      return ImGui::DragScalar(id, data_type, value, speed, nullptr, nullptr, format);
    }
    else
    {
      return ImGui::InputScalar(id, data_type, value, nullptr, nullptr, format);
    }
  }

  // Main property widgets
  template <typename T>
  static bool draw_vec2(const char*  label,
                        T&           vec,
                        const vec2f& reset_value = vec2f(0, 0),
                        const char*  tooltip     = nullptr)
  {
    bool changed = false;
    begin_property(label, tooltip);

    const float  total_width = ImGui::CalcItemWidth();
    const float  line_height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
    const ImVec2 button_size = {line_height, line_height};
    const float  spacing     = ImGui::GetStyle().ItemSpacing.x;

    float input_width = total_width;
    if (s_property_style.show_reset_buttons)
    {
      input_width = (total_width - (button_size.x * 2.0f) - spacing * 3.0f) / 2.0f;
    }
    else
    {
      input_width = (total_width - spacing) / 2.0f;
    }

    ImGuiDataType data_type;
    const char*   format = nullptr;

    if constexpr (std::is_same_v<T, vec2f>)
    {
      data_type = ImGuiDataType_Float;
      format    = "%.3f";
    }
    else if constexpr (std::is_same_v<T, vec2i>)
    {
      data_type = ImGuiDataType_S32;
      format    = "%d";
    }
    else if constexpr (std::is_same_v<T, vec2u>)
    {
      data_type = ImGuiDataType_U32;
      format    = "%u";
    }
    else
    {
      static_assert(!sizeof(T*), "Unsupported vec2 type");
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{spacing * 0.5f, 0});

    if (s_property_style.show_reset_buttons)
    {
      if (draw_reset_button("X"))
      {
        vec.x   = reset_value.x;
        changed = true;
      }
      ImGui::SameLine();
    }

    ImGui::PushItemWidth(input_width);
    ImGui::SameLine(0, 2);

    if (ImGui::InputScalar("##X", data_type, &vec.x, nullptr, nullptr, format))
    {
      changed = true;
    }

    if (ImGui::IsItemHovered())
    {
      ImGui::SetTooltip("X: %.3f", (float)vec.x);
    }

    ImGui::PopItemWidth();

    ImGui::SameLine();
    if (s_property_style.show_reset_buttons)
    {
      if (draw_reset_button("Y"))
      {
        vec.y   = reset_value.y;
        changed = true;
      }
      ImGui::SameLine();
    }

    ImGui::PushItemWidth(input_width);

    ImGui::SameLine(0, 2);

    if (ImGui::InputScalar("##Y", data_type, &vec.y, nullptr, nullptr, format))
    {
      changed = true;
    }

    if (ImGui::IsItemHovered())
    {
      ImGui::SetTooltip("Y: %.3f", (float)vec.y);
    }

    ImGui::PopItemWidth();

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
    {
      ImGui::OpenPopup("vec2_context_menu");
    }

    if (ImGui::BeginPopup("vec2_context_menu"))
    {
      ImGui::SetWindowSize(ImVec2(400, 0));
      ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Vector Operations");
      ImGui::Separator();

      if (ImGui::MenuItem("Copy", "Ctrl+C"))
      {
        ImGui::SetClipboardText(fmt::format("{}, {}", vec.x, vec.y).c_str());
      }

      if (ImGui::MenuItem("Paste", "Ctrl+V"))
      {
        const char* clipboard = ImGui::GetClipboardText();
        if (clipboard)
        {
          std::string str(clipboard);
          size_t      comma = str.find(',');
          if (comma != std::string::npos)
          {
            try
            {
              vec.x   = static_cast<decltype(vec.x)>(std::stof(str.substr(0, comma)));
              vec.y   = static_cast<decltype(vec.y)>(std::stof(str.substr(comma + 1)));
              changed = true;
            }
            catch (...)
            {
            }
          }
        }
      }

      ImGui::Separator();

      if constexpr (std::is_same_v<T, vec2f>)
      {
        if (ImGui::MenuItem("Normalize"))
        {
          vec     = vec.normalized();
          changed = true;
        }
      }

      if (ImGui::MenuItem("Zero", "0"))
      {
        vec.x = vec.y = 0;
        changed       = true;
      }

      if (ImGui::MenuItem("Reset"))
      {
        vec.x   = reset_value.x;
        vec.y   = reset_value.y;
        changed = true;
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Swap X/Y"))
      {
        std::swap(vec.x, vec.y);
        changed = true;
      }

      if (ImGui::MenuItem("Negate"))
      {
        vec.x   = -vec.x;
        vec.y   = -vec.y;
        changed = true;
      }

      ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
    end_property();

    if (changed)
      Scene::get_active_scene()->set_save_required();

    return changed;
  }

  template <typename T>
  static bool draw_vec3(const char*  label,
                        T&           vec,
                        const vec3f& reset_value = vec3f(0, 0, 0),
                        const char*  tooltip     = nullptr)
  {
    bool changed = false;
    begin_property(label, tooltip);

    const float  total_width = ImGui::CalcItemWidth();
    const float  line_height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
    const ImVec2 button_size = {line_height + 3.0f, line_height};
    const float  spacing     = ImGui::GetStyle().ItemSpacing.x;

    float input_width = total_width;
    if (s_property_style.show_reset_buttons)
    {
      input_width = (total_width - (button_size.x * 3.0f) - spacing * 4.0f) / 3.0f;
    }
    else
    {
      input_width = (total_width - spacing * 2.0f) / 3.0f;
    }

    ImGuiDataType data_type;
    const char*   format = nullptr;

    if constexpr (std::is_same_v<T, vec3f>)
    {
      data_type = ImGuiDataType_Float;
      format    = "%.3f";
    }
    else if constexpr (std::is_same_v<T, vec3i>)
    {
      data_type = ImGuiDataType_S32;
      format    = "%d";
    }
    else if constexpr (std::is_same_v<T, vec3u>)
    {
      data_type = ImGuiDataType_U32;
      format    = "%u";
    }
    else
    {
      static_assert(!sizeof(T*), "Unsupported vec3 type");
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{spacing * 0.5f, 0});

    if (s_property_style.show_reset_buttons)
    {
      if (draw_reset_button("X"))
      {
        vec.x   = static_cast<decltype(vec.x)>(reset_value.x);
        changed = true;
      }
      ImGui::SameLine();
    }

    ImGui::PushItemWidth(input_width);
    if (DrawDragScalar("##X", &vec.x, data_type, s_property_style.speed, format))
    {
      changed = true;
    }
    if (ImGui::IsItemHovered())
    {
      ImGui::SetTooltip("X: %s", format_value(vec.x).c_str());
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();
    if (s_property_style.show_reset_buttons)
    {
      if (draw_reset_button("Y"))
      {
        vec.y   = static_cast<decltype(vec.y)>(reset_value.y);
        changed = true;
      }
      ImGui::SameLine();
    }

    ImGui::PushItemWidth(input_width);
    if (draw_drag_scalar("##Y", &vec.y, data_type, s_property_style.speed, format))
    {
      changed = true;
    }
    if (ImGui::IsItemHovered())
    {
      ImGui::SetTooltip("Y: %s", format_value(vec.y).c_str());
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();
    if (s_property_style.show_reset_buttons)
    {
      if (draw_reset_button("Z"))
      {
        vec.z   = static_cast<decltype(vec.z)>(reset_value.z);
        changed = true;
      }
      ImGui::SameLine();
    }

    ImGui::PushItemWidth(input_width);
    if (draw_drag_scalar("##Z", &vec.z, data_type, s_property_style.speed, format))
    {
      changed = true;
    }
    if (ImGui::IsItemHovered())
    {
      ImGui::SetTooltip("Z: %s", format_value(vec.z).c_str());
    }
    ImGui::PopItemWidth();

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
    {
      ImGui::OpenPopup("vec3_context_menu");
    }

    if (ImGui::BeginPopup("vec3_context_menu"))
    {
      ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Vector3 Operations");
      ImGui::Separator();

      if (ImGui::MenuItem("Copy", "Ctrl+C"))
      {
        ImGui::SetClipboardText(fmt::format("{}, {}, {}", vec.x, vec.y, vec.z).c_str());
      }
      if (ImGui::MenuItem("Paste", "Ctrl+V"))
      {
        const char* clipboard = ImGui::GetClipboardText();
        if (clipboard)
        {
          std::string              str(clipboard);
          std::vector<std::string> parts;
          size_t                   pos = 0;
          while ((pos = str.find(',')) != std::string::npos)
          {
            parts.push_back(str.substr(0, pos));
            str.erase(0, pos + 1);
          }
          parts.push_back(str);

          if (parts.size() >= 3)
          {
            try
            {
              vec.x   = static_cast<decltype(vec.x)>(std::stof(parts[0]));
              vec.y   = static_cast<decltype(vec.y)>(std::stof(parts[1]));
              vec.z   = static_cast<decltype(vec.z)>(std::stof(parts[2]));
              changed = true;
            }
            catch (...)
            {
            }
          }
        }
      }

      ImGui::Separator();

      if constexpr (std::is_same_v<T, vec3f>)
      {
        if (ImGui::MenuItem("Normalize"))
        {
          vec     = vec.normalized();
          changed = true;
        }
      }
      if (ImGui::MenuItem("Zero"))
      {
        vec.x = vec.y = vec.z = 0;
        changed               = true;
      }
      if (ImGui::MenuItem("Reset"))
      {
        vec.x   = reset_value.x;
        vec.y   = reset_value.y;
        vec.z   = reset_value.z;
        changed = true;
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Negate"))
      {
        vec.x   = -vec.x;
        vec.y   = -vec.y;
        vec.z   = -vec.z;
        changed = true;
      }
      if (ImGui::MenuItem("Abs"))
      {
        vec.x   = std::abs(vec.x);
        vec.y   = std::abs(vec.y);
        vec.z   = std::abs(vec.z);
        changed = true;
      }

      ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
    end_property();

    if (changed)
      Scene::get_active_scene()->set_save_required();

    return changed;
  }

  template <typename T>
  static bool draw_vec4(const char*  label,
                        T&           vec,
                        const vec4f& reset_value = vec4f(0, 0, 0, 0),
                        const char*  tooltip     = nullptr)
  {
    bool changed = false;
    begin_property(label, tooltip);

    const float  total_width = ImGui::CalcItemWidth();
    const float  line_height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
    const ImVec2 button_size = {line_height + 3.0f, line_height};
    const float  spacing     = ImGui::GetStyle().ItemSpacing.x;

    float input_width = s_property_style.show_reset_buttons
                            ? (total_width - (button_size.x * 2.0f) - spacing * 3.0f) / 2.0f
                            : (total_width - spacing) / 2.0f;

    ImGuiDataType data_type;
    const char*   format = nullptr;

    if constexpr (std::is_same_v<T, vec4f>)
    {
      data_type = ImGuiDataType_Float;
      format    = "%.3f";
    }
    else if constexpr (std::is_same_v<T, vec4i>)
    {
      data_type = ImGuiDataType_S32;
      format    = "%d";
    }
    else if constexpr (std::is_same_v<T, vec4u>)
    {
      data_type = ImGuiDataType_U32;
      format    = "%u";
    }
    else
    {
      static_assert(!sizeof(T*), "Unsupported vec4 type");
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{spacing * 0.5f, spacing * 0.5f});

    // First row (X, Y)
    if (s_property_style.show_reset_buttons)
    {
      if (draw_reset_button("X"))
      {
        vec.x   = static_cast<decltype(vec.x)>(reset_value.x);
        changed = true;
      }
      ImGui::SameLine();
    }

    ImGui::PushItemWidth(input_width);
    if (draw_drag_scalar("##X", &vec.x, data_type, s_property_style.speed, format))
    {
      changed = true;
    }
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("X: %s", format_value(vec.x).c_str());
    ImGui::PopItemWidth();

    ImGui::SameLine();
    if (s_property_style.show_reset_buttons)
    {
      if (draw_reset_button("Y"))
      {
        vec.y   = static_cast<decltype(vec.y)>(reset_value.y);
        changed = true;
      }
      ImGui::SameLine();
    }

    ImGui::PushItemWidth(input_width);
    if (draw_drag_scalar("##Y", &vec.y, data_type, s_property_style.speed, format))
    {
      changed = true;
    }
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Y: %s", format_value(vec.y).c_str());
    ImGui::PopItemWidth();

    // Second row (Z, W)
    if (s_property_style.show_reset_buttons)
    {
      if (draw_reset_button("Z"))
      {
        vec.z   = static_cast<decltype(vec.z)>(reset_value.z);
        changed = true;
      }
      ImGui::SameLine();
    }

    ImGui::PushItemWidth(input_width);
    if (draw_drag_scalar("##Z", &vec.z, data_type, s_property_style.speed, format))
    {
      changed = true;
    }
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Z: %s", format_value(vec.z).c_str());
    ImGui::PopItemWidth();

    ImGui::SameLine();
    if (s_property_style.show_reset_buttons)
    {
      if (draw_reset_button("W"))
      {
        vec.w   = static_cast<decltype(vec.w)>(reset_value.w);
        changed = true;
      }
      ImGui::SameLine();
    }

    ImGui::PushItemWidth(input_width);
    if (draw_drag_scalar("##W", &vec.w, data_type, s_property_style.speed, format))
    {
      changed = true;
    }
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("W: %s", format_value(vec.w).c_str());
    ImGui::PopItemWidth();

    // Context menu
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
    {
      ImGui::OpenPopup("vec4_context_menu");
    }

    if (ImGui::BeginPopup("vec4_context_menu"))
    {
      ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Vector4 Operations");
      ImGui::Separator();

      if (ImGui::MenuItem("Copy"))
      {
        ImGui::SetClipboardText(fmt::format("{}, {}, {}, {}", vec.x, vec.y, vec.z, vec.w).c_str());
      }
      if (ImGui::MenuItem("Zero"))
      {
        vec.x = vec.y = vec.z = vec.w = 0;
        changed                       = true;
      }
      if (ImGui::MenuItem("Reset"))
      {
        vec     = reset_value;
        changed = true;
      }
      ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
    end_property();

    if (changed)
      Scene::get_active_scene()->set_save_required();

    return changed;
  }

  template <typename T>
  static bool draw_value(const char* label,
                         T&          value,
                         T           min     = std::numeric_limits<T>::lowest(),
                         T           max     = std::numeric_limits<T>::max(),
                         const char* tooltip = nullptr)
  {
    bool changed = false;
    begin_property(label, tooltip);

    ImGuiDataType data_type;
    const char*   format = s_property_style.format;

    if constexpr (std::is_same_v<T, float>)
    {
      data_type = ImGuiDataType_Float;
    }
    else if constexpr (std::is_same_v<T, double>)
    {
      data_type = ImGuiDataType_Double;
      format    = "%.6f";
    }
    else if constexpr (std::is_same_v<T, int>)
    {
      data_type = ImGuiDataType_S32;
      format    = "%d";
    }
    else if constexpr (std::is_same_v<T, unsigned int>)
    {
      data_type = ImGuiDataType_U32;
      format    = "%u";
    }
    else if constexpr (std::is_same_v<T, int8_t>)
    {
      data_type = ImGuiDataType_S8;
      format    = "%d";
    }
    else if constexpr (std::is_same_v<T, uint8_t>)
    {
      data_type = ImGuiDataType_U8;
      format    = "%u";
    }
    else if constexpr (std::is_same_v<T, int16_t>)
    {
      data_type = ImGuiDataType_S16;
      format    = "%d";
    }
    else if constexpr (std::is_same_v<T, uint16_t>)
    {
      data_type = ImGuiDataType_U16;
      format    = "%u";
    }
    else if constexpr (std::is_same_v<T, int64_t>)
    {
      data_type = ImGuiDataType_S64;
      format    = "%lld";
    }
    else if constexpr (std::is_same_v<T, uint64_t>)
    {
      data_type = ImGuiDataType_U64;
      format    = "%llu";
    }
    else
    {
      static_assert(!sizeof(T*), "Unsupported value type");
    }

    if (s_property_style.slider_enabled && std::is_floating_point_v<T>)
    {
      float float_value = static_cast<float>(value);
      if (ImGui::SliderFloat("##value",
                             &float_value,
                             static_cast<float>(min),
                             static_cast<float>(max),
                             format))
      {
        value   = static_cast<T>(float_value);
        changed = true;
      }
    }
    else if (s_property_style.drag_enabled)
    {
      if (ImGui::DragScalar("##value",
                            data_type,
                            &value,
                            static_cast<float>(s_property_style.speed),
                            &min,
                            &max,
                            format))
      {
        changed = true;
      }
    }
    else
    {
      if (ImGui::InputScalar("##value", data_type, &value, nullptr, nullptr, format))
      {
        changed = true;
      }
    }

    // Context menu
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
    {
      ImGui::OpenPopup("value_context_menu");
    }

    if (ImGui::BeginPopup("value_context_menu"))
    {
      ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Value Operations");
      ImGui::Separator();

      if (ImGui::MenuItem("Copy", "Ctrl+C"))
      {
        ImGui::SetClipboardText(fmt::format("{}", value).c_str());
      }
      if (ImGui::MenuItem("Paste", "Ctrl+V"))
      {
        const char* clipboard = ImGui::GetClipboardText();
        if (clipboard)
        {
          try
          {
            if constexpr (std::is_floating_point_v<T>)
            {
              value = static_cast<T>(std::stod(clipboard));
            }
            else
            {
              value = static_cast<T>(std::stoll(clipboard));
            }
            changed = true;
          }
          catch (...)
          {
          }
        }
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Zero"))
      {
        value   = T{0};
        changed = true;
      }
      if (ImGui::MenuItem("Min"))
      {
        value   = min;
        changed = true;
      }
      if (ImGui::MenuItem("Max"))
      {
        value   = max;
        changed = true;
      }

      ImGui::EndPopup();
    }

    end_property();

    if (changed)
      Scene::get_active_scene()->set_save_required();

    return changed;
  }

  static bool draw_color(const char*         label,
                         Color&              color,
                         ImGuiColorEditFlags flags   = ImGuiColorEditFlags_None,
                         const char*         tooltip = nullptr)
  {
    bool changed = false;
    begin_property(label, tooltip);

    vec4f normalized = {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{4, 4});

    if (ImGui::ColorEdit4("##color",
                          &normalized.x,
                          flags | ImGuiColorEditFlags_NoLabel |
                              ImGuiColorEditFlags_AlphaPreviewHalf))
    {
      color.r = static_cast<uint8_t>(normalized.x * 255.0f);
      color.g = static_cast<uint8_t>(normalized.y * 255.0f);
      color.b = static_cast<uint8_t>(normalized.z * 255.0f);
      color.a = static_cast<uint8_t>(normalized.w * 255.0f);
      changed = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Pick", ImVec2(50, 0)))
    {
      ImGui::OpenPopup("color_picker_popup");
    }

    if (ImGui::BeginPopup("color_picker_popup"))
    {
      ImGui::ColorPicker4("##picker",
                          &normalized.x,
                          flags | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHSV |
                              ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_AlphaBar);

      if (ImGui::Button("Apply", ImVec2(80, 0)))
      {
        color.r = static_cast<uint8_t>(normalized.x * 255.0f);
        color.g = static_cast<uint8_t>(normalized.y * 255.0f);
        color.b = static_cast<uint8_t>(normalized.z * 255.0f);
        color.a = static_cast<uint8_t>(normalized.w * 255.0f);
        changed = true;
        ImGui::CloseCurrentPopup();
      }

      ImGui::SameLine();
      if (ImGui::Button("Cancel", ImVec2(80, 0)))
      {
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }

    // Enhanced context menu
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
    {
      ImGui::OpenPopup("color_context_menu");
    }

    if (ImGui::BeginPopup("color_context_menu"))
    {
      ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Color Presets");
      ImGui::Separator();

      if (ImGui::MenuItem("Copy as Hex"))
      {
        ImGui::SetClipboardText(
            fmt::format("#{:02X}{:02X}{:02X}{:02X}", color.r, color.g, color.b, color.a).c_str());
      }
      if (ImGui::MenuItem("Copy as RGB"))
      {
        ImGui::SetClipboardText(fmt::format("rgb({}, {}, {})", color.r, color.g, color.b).c_str());
      }
      if (ImGui::MenuItem("Copy as RGBA"))
      {
        ImGui::SetClipboardText(
            fmt::format("rgba({}, {}, {}, {:.2f})", color.r, color.g, color.b, color.a / 255.0f)
                .c_str());
      }

      ImGui::Separator();
      ImGui::Text("Quick Colors");
      ImGui::Separator();

      if (ImGui::MenuItem("Black"))
      {
        color   = Color::Black;
        changed = true;
      }
      if (ImGui::MenuItem("White"))
      {
        color   = Color::White;
        changed = true;
      }
      if (ImGui::MenuItem("Red"))
      {
        color   = Color::Red;
        changed = true;
      }
      if (ImGui::MenuItem("Green"))
      {
        color   = Color::Green;
        changed = true;
      }
      if (ImGui::MenuItem("Blue"))
      {
        color   = Color::Blue;
        changed = true;
      }
      if (ImGui::MenuItem("Yellow"))
      {
        color   = Color(255, 255, 0);
        changed = true;
      }
      if (ImGui::MenuItem("Cyan"))
      {
        color   = Color(0, 255, 255);
        changed = true;
      }
      if (ImGui::MenuItem("Magenta"))
      {
        color   = Color(255, 0, 255);
        changed = true;
      }

      ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
    end_property();

    if (changed)
      Scene::get_active_scene()->set_save_required();

    return changed;
  }

  static bool draw_bool(const char* label, bool& value, const char* tooltip = nullptr)
  {
    bool changed = false;
    begin_property(label, tooltip);

    if (ImGui::Checkbox("##value", &value))
    {
      changed = true;
    }

    // Visual feedback on hover
    if (ImGui::IsItemHovered())
    {
      ImGui::SetTooltip("%s", value ? "Enabled" : "Disabled");
    }

    end_property();

    if (changed)
      Scene::get_active_scene()->set_save_required();

    return changed;
  }

  static bool draw_string(const char*  label,
                          std::string& value,
                          size_t       buffer_size = 256,
                          const char*  tooltip     = nullptr)
  {
    bool changed = false;
    begin_property(label, tooltip);

    static std::unordered_map<std::string, std::vector<char>> buffers;

    // Stable key per property
    std::string key    = std::string(label) + "##string";
    auto&       buffer = buffers[key];

    if (buffer.empty() || std::string(buffer.data()) != value)
    {
      buffer.resize(buffer_size);
      std::strncpy(buffer.data(), value.c_str(), buffer_size - 1);
      buffer[buffer_size - 1] = '\0';
    }

    if (ImGui::InputText("##value", buffer.data(), buffer_size))
    {
      value   = std::string(buffer.data());
      changed = true;
    }

    // Character count on hover
    if (ImGui::IsItemHovered())
    {
      ImGui::SetTooltip("Characters: %zu / %zu", value.length(), buffer_size - 1);
    }

    // Context menu
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
    {
      ImGui::OpenPopup("string_context_menu");
    }

    if (ImGui::BeginPopup("string_context_menu"))
    {
      ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Text Operations");
      ImGui::Separator();

      if (ImGui::MenuItem("Copy", "Ctrl+C"))
      {
        ImGui::SetClipboardText(value.c_str());
      }

      if (ImGui::MenuItem("Paste", "Ctrl+V"))
      {
        const char* clipboard = ImGui::GetClipboardText();
        if (clipboard)
        {
          value = std::string(clipboard);
          std::strncpy(buffer.data(), value.c_str(), buffer_size - 1);
          buffer[buffer_size - 1] = '\0';
          changed                 = true;
        }
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Clear"))
      {
        value.clear();
        buffer[0] = '\0';
        changed   = true;
      }

      if (ImGui::MenuItem("Trim"))
      {
        size_t start = value.find_first_not_of(" \t");
        size_t end   = value.find_last_not_of(" \t");
        if (start != std::string::npos && end != std::string::npos)
        {
          value = value.substr(start, end - start + 1);
          std::strncpy(buffer.data(), value.c_str(), buffer_size - 1);
          changed = true;
        }
      }

      ImGui::Separator();

      if (ImGui::MenuItem("To Uppercase"))
      {
        std::transform(value.begin(), value.end(), value.begin(), ::toupper);
        std::strncpy(buffer.data(), value.c_str(), buffer_size - 1);
        changed = true;
      }

      if (ImGui::MenuItem("To Lowercase"))
      {
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        std::strncpy(buffer.data(), value.c_str(), buffer_size - 1);
        changed = true;
      }

      ImGui::EndPopup();
    }

    end_property();

    if (changed)
      Scene::get_active_scene()->set_save_required();

    return changed;
  }

  template <typename T>
  static std::string format_value(const T& value)
  {
    if constexpr (std::is_floating_point_v<T>)
    {
      return fmt::format("{:.3f}", value);
    }
    else
    {
      return fmt::format("{}", value);
    }
  }

  static bool draw_string_multiline(const char*  label,
                                    std::string& value,
                                    size_t       buffer_size = 1024,
                                    ImVec2       box_size    = ImVec2(0, 100),
                                    const char*  tooltip     = nullptr)
  {
    bool changed = false;
    begin_property(label, tooltip);

    static std::unordered_map<std::string, std::vector<char>> buffers;

    // Stable key per property
    std::string key    = std::string(label) + "##multiline";
    auto&       buffer = buffers[key];

    if (buffer.empty() || std::string(buffer.data()) != value)
    {
      buffer.resize(buffer_size);
      std::strncpy(buffer.data(), value.c_str(), buffer_size - 1);
      buffer[buffer_size - 1] = '\0';
    }

    if (ImGui::InputTextMultiline("##value",
                                  buffer.data(),
                                  buffer_size,
                                  box_size,
                                  ImGuiInputTextFlags_AllowTabInput))
    {
      value   = std::string(buffer.data());
      changed = true;
    }

    // Character count indicator
    if (ImGui::IsItemHovered())
    {
      ImGui::SetTooltip("Characters: %zu / %zu", value.length(), buffer_size - 1);
    }

    // Enhanced context menu
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
    {
      ImGui::OpenPopup("multiline_context_menu");
    }

    if (ImGui::BeginPopup("multiline_context_menu"))
    {
      ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Text Operations");
      ImGui::Separator();

      if (ImGui::MenuItem("Copy All", "Ctrl+C"))
      {
        ImGui::SetClipboardText(value.c_str());
      }

      if (ImGui::MenuItem("Paste", "Ctrl+V"))
      {
        const char* clipboard = ImGui::GetClipboardText();
        if (clipboard)
        {
          value = std::string(clipboard);
          std::strncpy(buffer.data(), value.c_str(), buffer_size - 1);
          buffer[buffer_size - 1] = '\0';
          changed                 = true;
        }
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Clear"))
      {
        value.clear();
        buffer[0] = '\0';
        changed   = true;
      }

      if (ImGui::MenuItem("Trim Whitespace"))
      {
        // Trim leading and trailing whitespace
        size_t start = value.find_first_not_of(" \t\n\r");
        size_t end   = value.find_last_not_of(" \t\n\r");
        if (start != std::string::npos && end != std::string::npos)
        {
          value = value.substr(start, end - start + 1);
          std::strncpy(buffer.data(), value.c_str(), buffer_size - 1);
          changed = true;
        }
      }

      ImGui::Separator();

      ImGui::Text("Info: %zu chars, %zu lines",
                  value.length(),
                  std::count(value.begin(), value.end(), '\n') + 1);

      ImGui::EndPopup();
    }

    end_property();

    if (changed)
      Scene::get_active_scene()->set_save_required();

    return changed;
  }

  template <typename Enum>
  static bool draw_enum(const char*                     label,
                        Enum&                           value,
                        const std::vector<std::string>& options,
                        const char*                     tooltip = nullptr)
  {
    bool changed = false;
    begin_property(label, tooltip);

    int                current_item = static_cast<int>(value);
    const std::string& current_text = options[current_item];

    if (ImGui::BeginCombo("##value", current_text.c_str()))
    {
      for (int i = 0; i < static_cast<int>(options.size()); i++)
      {
        const bool is_selected = (current_item == i);

        if (is_selected)
        {
          ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.26f, 0.59f, 0.98f, 1.0f));
        }

        if (ImGui::Selectable(options[i].c_str(), is_selected))
        {
          value   = static_cast<Enum>(i);
          changed = true;
        }

        if (is_selected)
        {
          ImGui::PopStyleColor();
          ImGui::SetItemDefaultFocus();
        }

        // Show index number on hover
        if (ImGui::IsItemHovered())
        {
          ImGui::SetTooltip("Index: %d", i);
        }
      }
      ImGui::EndCombo();
    }

    if (ImGui::IsItemHovered())
    {
      ImGui::SetTooltip("%s (Index: %d / %zu)",
                        current_text.c_str(),
                        current_item,
                        options.size() - 1);
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
    {
      ImGui::OpenPopup("enum_context_menu");
    }

    if (ImGui::BeginPopup("enum_context_menu"))
    {
      ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Enum Navigation");
      ImGui::Separator();

      if (ImGui::MenuItem("Previous", current_item > 0 ? "↑" : nullptr, false, current_item > 0))
      {
        value   = static_cast<Enum>(current_item - 1);
        changed = true;
      }

      if (ImGui::MenuItem("Next",
                          current_item < static_cast<int>(options.size()) - 1 ? "↓" : nullptr,
                          false,
                          current_item < static_cast<int>(options.size()) - 1))
      {
        value   = static_cast<Enum>(current_item + 1);
        changed = true;
      }

      ImGui::Separator();

      if (ImGui::MenuItem("First"))
      {
        value   = static_cast<Enum>(0);
        changed = true;
      }

      if (ImGui::MenuItem("Last"))
      {
        value   = static_cast<Enum>(options.size() - 1);
        changed = true;
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Copy Name"))
      {
        ImGui::SetClipboardText(current_text.c_str());
      }

      ImGui::EndPopup();
    }

    end_property();

    if (changed)
      Scene::get_active_scene()->set_save_required();

    return changed;
  }

private:
  static Panels        s_show_panels;
  static PropertyStyle s_property_style;
};

}  // namespace ag
