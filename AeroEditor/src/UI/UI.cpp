#include "imgui.h"

#include <Aero.hpp>
#include <Application/AppSettings.hpp>
#include <Application/EditorLayer.hpp>
#include <Editor/Autosave.hpp>
#include <Editor/SceneCommands.hpp>
#include <Editor/UndoRedo.hpp>
#include <Font/FontImporter.hpp>
#include <Font/FontManager.hpp>
#include <Panels/ScenePanel.hpp>
#include <Project/FileDialogs.hpp>
#include <UI/UI.hpp>

#ifdef PLATFORM_WINDOWS
  #include <shellapi.h>
  #include <windows.h>
#elif defined(PLATFORM_LINUX)

#endif

#include <icons.h>

#include <codecvt>
#include <filesystem>
#include <locale>
#include <system_error>
#include <UI/PopUp.hpp>
#include <UI/StyleScope.hpp>

namespace ag
{
static const uint16_t bit_lookup[3][3] = {
    {TL, T, TR},
    { L, M,  R},
    {BL, B, BR}
};
Panels             UI::s_show_panels{};
PropertyStyle      UI::s_property_style{};
static const Color folder_color = Color::White;
static std::string new_name;
constexpr float    button_height = 32.0f;

static std::string& scene_save_path()
{
  static std::string value = Project::get_dialog_directory(ProjectDirectory::Scenes);
  return value;
}

static std::string& script_save_path()
{
  static std::string value = Project::get_dialog_directory(ProjectDirectory::Scripts);
  return value;
}

// Relative folder names typed into the popups are resolved against the managed
// project root so "SubFolder" means "<Project>/Scenes/SubFolder".
static std::string resolve_directory_input(const std::string& value, ProjectDirectory kind)
{
  std::string normalized = value;
  Helper::normalize_path(normalized);
  if (normalized.empty())
    return Project::get_dialog_directory(kind);

  std::filesystem::path directory(normalized);
  if (directory.is_relative())
    directory = std::filesystem::path(Project::get_dialog_directory(kind)) / directory;

  std::error_code ec;
  std::filesystem::path canonical = std::filesystem::weakly_canonical(directory, ec);
  if (!ec)
    directory = canonical;
  return directory.string();
}

static bool is_valid_file_name(const std::string& name)
{
  return !name.empty() && name.find('/') == std::string::npos &&
         name.find('\\') == std::string::npos;
}

// Converts an asset picker/drop result into the project-relative path used by
// the engine, rejecting anything outside the project's Assets directory.
static std::string resolve_asset_path(const std::string& path)
{
  std::string normalized = path;
  Helper::normalize_path(normalized);
  std::string relative = Project::to_active_relative_path(normalized, ProjectDirectory::Assets);
  if (relative.empty())
    AERO_CORE_ERROR("Asset \"{0}\" is outside the project Assets folder.", normalized);
  return relative;
}

static void restart_editor_with_project(const std::string& project_path)
{
  if (project_path.empty())
    return;
  AppSettings::reload_app(project_path);
}

#ifdef PLATFORM_WINDOWS
constexpr const wchar_t* exe_ext  = L".exe";
constexpr const wchar_t  path_sep = L'\\';
#elif defined(PLATFORM_LINUX)
constexpr const wchar_t* exe_ext  = L"";
constexpr const wchar_t  path_sep = L'/';
#endif

void UI::draw_menu_bar()
{
  if (!ImGui::BeginMainMenuBar())
    return;

  auto& editor = EditorLayer::get();
  auto  panel  = editor.get_panel();
  auto  scene  = editor.get_scene();
  const bool has_scene    = static_cast<bool>(scene);
  const bool has_selected = panel && panel->has_selected_entity();

  auto create_node = [&](const char* name, NodeType type)
  {
    if (!scene)
      return;
    record_scene_action(scene, "Create object", [&]()
    {
      Entity created = scene->create_entity(name, type);
      if (panel)
      {
        panel->set_selected_entity(created);
        panel->set_properties_entity(created);
      }
      scene->set_save_required(true);
    });
  };

  if (ImGui::BeginMenu("  File  "))
  {
    if (ImGui::MenuItem(ICON_FA_FILE "  New Scene", "Ctrl+N", false, true))
      UI::get_uistate_panels().create_new_scene = true;

    if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN "  Open Scene...", "Ctrl+O", false, true))
      editor.open_scene();

    ImGui::Separator();

    if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK "  Save Scene", "Ctrl+S", false, has_scene))
      editor.save_scene();

    if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK "  Save Scene As Default", "Ctrl+Shift+S", false, has_scene))
      editor.save_scene_as_default();

    if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK "  Save All Scenes", "Ctrl+Alt+S", false, true))
      editor.save_all_scene();

    ImGui::Separator();

    if (ImGui::BeginMenu(ICON_FA_FOLDER "  Project"))
    {
      if (ImGui::MenuItem(ICON_FA_PLUS "  New Project..."))
      {
        std::string folder = FileDialogs::select_folder("Select Folder for a new project");
        if (!folder.empty())
        {
          auto project = Project::new_project(folder);
          restart_editor_with_project(project->get_project_file_directory());
        }
      }
      if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN "  Open Project..."))
      {
        std::string project_file = FileDialogs::open_file(
            "Aero Project (*.aeroproj)\0*.aeroproj\0All Files (*.*)\0*.*\0");
        restart_editor_with_project(project_file);
      }
      if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN "  Open Project Folder..."))
      {
        std::string folder = FileDialogs::select_folder("Select Your Project Folder");
        restart_editor_with_project(folder);
      }
      ImGui::Separator();
      if (ImGui::MenuItem(ICON_FA_GEAR "  Project Settings"))
        s_show_panels.project_settings_panel = true;
      ImGui::EndMenu();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(ICON_FA_FILE_EXPORT "  Export Game", nullptr, false, has_scene))
      editor.open_export_panel();

    if (ImGui::MenuItem(ICON_FA_MOBILE "  Build Android APK...", nullptr, false, has_scene))
      editor.open_android_build_panel();

    ImGui::Separator();

    if (ImGui::MenuItem(ICON_FA_POWER_OFF "  Exit", "Alt+F4"))
      editor.try_exit();

    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("  Edit  "))
  {
    auto& history = UndoRedoManager::get();
    if (ImGui::MenuItem(ICON_FA_ARROW_ROTATE_LEFT "  Undo",
                        "Ctrl+Z",
                        false,
                        history.can_undo()))
    {
      history.undo();
    }
    if (ImGui::MenuItem(ICON_FA_ARROW_ROTATE_RIGHT "  Redo",
                        "Ctrl+Y",
                        false,
                        history.can_redo()))
    {
      history.redo();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(ICON_FA_COPY "  Copy", "Ctrl+C", false, has_selected))
      panel->copy_entity();

    if (ImGui::MenuItem(ICON_FA_PASTE "  Paste", "Ctrl+V", false, has_scene))
      panel->paste_entity();

    if (ImGui::MenuItem(ICON_FA_CLONE "  Duplicate", "Ctrl+D", false, has_selected))
      panel->duplicate_entity();

    if (ImGui::MenuItem(ICON_FA_PEN "  Rename", "F2", false, has_selected))
      panel->begin_rename(panel->get_selected_entity());

    if (ImGui::MenuItem(ICON_FA_TRASH "  Delete", "Del", false, has_selected))
      panel->delete_entity();

    ImGui::Separator();

    if (ImGui::MenuItem(ICON_FA_XMARK "  Deselect", "Esc", false, has_selected))
    {
      panel->set_selected_entity(Entity());
      panel->set_properties_entity(Entity());
    }

    if (ImGui::MenuItem(ICON_FA_CROSSHAIRS "  Focus Selected", "F", false, has_selected))
      panel->find_entity_in_project(panel->get_selected_entity());

    ImGui::Separator();

    if (ImGui::MenuItem(ICON_FA_GEAR "  Project Settings", "Ctrl+,"))
      s_show_panels.project_settings_panel = true;

    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("  View  "))
  {
    ImGui::MenuItem(ICON_FA_SITEMAP "  Scene Hierarchy", nullptr, &s_show_panels.scene_panel);
    ImGui::MenuItem(ICON_FA_LIST "  Properties / Inspector", nullptr, &s_show_panels.properties_panel);
    ImGui::MenuItem(ICON_FA_TERMINAL "  Console", nullptr, &s_show_panels.console_panel);

    ImGui::Separator();

    if (ImGui::BeginMenu(ICON_FA_PAINTBRUSH "  Themes"))
    {
      if (ImGui::MenuItem(" Unity Dark (Recommended)"))
        ImGuiLayer::set_unity_dark_theme();
      if (ImGui::MenuItem(" Unity Light"))
        ImGuiLayer::set_unity_light_theme();
      ImGui::Separator();
      if (ImGui::MenuItem(" Default Dark"))
        ImGui::StyleColorsDark();
      if (ImGui::MenuItem(" Default Light"))
        ImGui::StyleColorsLight();
      ImGui::Separator();
      if (ImGui::MenuItem(" CyberPunk"))
        ImGuiLayer::set_cyberpunk_theme();
      if (ImGui::MenuItem(" Warm Amber"))
        ImGuiLayer::set_warm_amber_theme();
      if (ImGui::MenuItem(" Ocean Blue"))
        ImGuiLayer::set_ocean_blue_theme();
      if (ImGui::MenuItem(" Forest Green"))
        ImGuiLayer::set_forest_green_theme();
      if (ImGui::MenuItem(" Purple Nebula"))
        ImGuiLayer::set_purple_nebula_theme();
      if (ImGui::MenuItem(" Monochrome Slate"))
        ImGuiLayer::set_monochrome_slate_theme();
      if (ImGui::MenuItem(" MidNight Blue"))
        ImGuiLayer::set_midnight_blue_theme();
      if (ImGui::MenuItem(" Crimson Red"))
        ImGuiLayer::set_crimson_red_theme();
      if (ImGui::MenuItem(" Cyan Teal"))
        ImGuiLayer::set_cyan_teal_theme();

      ImGui::EndMenu();
    }

    ImGui::Separator();
    ImGui::TextDisabled("Panel visibility toggles apply immediately.");

    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("  Create  "))
  {
    if (ImGui::MenuItem(ICON_FA_PLUS "  Create Empty", nullptr, false, has_scene))
      create_node("Empty", NodeType::Scene2D);

    ImGui::Separator();

    if (ImGui::BeginMenu(ICON_FA_CUBE "  2D Nodes", has_scene))
    {
      if (ImGui::MenuItem(ICON_FA_CUBE "  Scene2D"))       create_node("Scene2D", NodeType::Scene2D);
      if (ImGui::MenuItem(ICON_FA_IMAGE "  Sprite"))       create_node("Sprite", NodeType::Sprite);
      if (ImGui::MenuItem(ICON_FA_SQUARE "  Rectangle"))   create_node("Rectangle", NodeType::Rectangle);
      if (ImGui::MenuItem(ICON_FA_CIRCLE "  Circle"))      create_node("Circle", NodeType::Circle);
      if (ImGui::MenuItem(ICON_FA_FONT "  Text"))          create_node("Text", NodeType::Text);
      if (ImGui::MenuItem(ICON_FA_CAMERA "  Camera"))      create_node("Camera", NodeType::Camera);
      if (ImGui::MenuItem(ICON_FA_FILM "  AnimatedSprite2D")) create_node("AnimatedSprite2D", NodeType::AnimatedSprite2D);
      if (ImGui::MenuItem(ICON_FA_BORDER_ALL "  TileMap")) create_node("TileMap", NodeType::TileMap);
      if (ImGui::MenuItem(ICON_FA_SQUARE "  Button"))      create_node("Button", NodeType::Button);
      if (ImGui::MenuItem(ICON_FA_IMAGE "  TextureButton")) create_node("TextureButton", NodeType::TextureButton);
      if (ImGui::MenuItem(ICON_FA_VECTOR_SQUARE "  CollisionShape")) create_node("CollisionShape", NodeType::CollisionShape);
      if (ImGui::MenuItem(ICON_FA_VOLUME_HIGH "  Audio"))  create_node("Audio", NodeType::Audio);
      ImGui::EndMenu();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(ICON_FA_FILE_CODE "  New Script...", nullptr, false, has_scene))
      s_show_panels.create_new_script = true;

    if (ImGui::MenuItem(ICON_FA_FILE "  New Scene...", "Ctrl+N", false, true))
      s_show_panels.create_new_scene = true;

    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("  Scene  "))
  {
    if (ImGui::MenuItem(ICON_FA_PLAY "  Run Current Scene", "Ctrl+F5", false, has_scene))
    {
      run_current_scene();
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
      auto project = Project::get_active_project();
      std::wstring project_arg;
      if (project)
      {
        std::string project_file = project->get_project_file_directory();
        project_arg.assign(project_file.begin(), project_file.end());
      }
      std::wstring folder = FileDialogs::get_exe_folder();
      std::wstring app    = folder + std::wstring(1, path_sep) + L"Sandbox" + exe_ext;
      FileDialogs::run_exe(app, project_arg);
#endif
    }

    if (ImGui::MenuItem(ICON_FA_PLAY "  Run Default Scene", "F5", false))
    {
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
      auto project = Project::get_active_project();
      std::wstring project_arg;
      if (project)
      {
        std::string project_file = project->get_project_file_directory();
        project_arg.assign(project_file.begin(), project_file.end());
      }
      std::wstring folder = FileDialogs::get_exe_folder();
      std::wstring app    = folder + std::wstring(1, path_sep) + L"Sandbox" + exe_ext;
      FileDialogs::run_exe(app, project_arg);
#endif
    }

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    auto project = Project::get_active_project();
    if (project && project->get_server_config().enabled)
    {
      if (ImGui::MenuItem(ICON_FA_SERVER "  Run Server", "Ctrl+Shift+F5", false))
      {
        std::string project_path = project->get_project_file_directory();
        std::wstring folder      = FileDialogs::get_exe_folder();
        std::wstring app         = folder + path_sep + L"ServerSandbox" + exe_ext;
        std::wstring path        = std::wstring(project_path.begin(), project_path.end());
        FileDialogs::run_exe(app, path);
      }
    }
#endif

    ImGui::Separator();

    // Active scene switcher
    if (ImGui::BeginMenu(ICON_FA_LAYER_GROUP "  Active Scene", !editor.get_scenes().empty()))
    {
      for (const auto& [name, sc] : editor.get_scenes())
      {
        bool active = (sc == scene);
        if (ImGui::MenuItem(name.c_str(), nullptr, active) && sc)
        {
          editor.activate_scene(sc);
        }
      }
      ImGui::EndMenu();
    }

    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("  Tools  "))
  {
    const TransformSetting mode = panel ? panel->get_transform_setting() : TransformSetting::None;
    const TransformAxis    axis = panel ? panel->get_transform_axis() : TransformAxis::None;

    if (ImGui::MenuItem(ICON_FA_ARROW_POINTER "  Select", "Q", mode == TransformSetting::None, panel != nullptr))
      panel->reset_transform_setting();
    if (ImGui::MenuItem(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT "  Move", "G", mode == TransformSetting::Move, panel != nullptr))
      panel->set_transform_setting(TransformSetting::Move);
    if (ImGui::MenuItem(ICON_FA_ROTATE "  Rotate", "R", mode == TransformSetting::Rotate, panel != nullptr))
      panel->set_transform_setting(TransformSetting::Rotate);
    if (ImGui::MenuItem(ICON_FA_EXPAND "  Scale", "S", mode == TransformSetting::Scale, panel != nullptr))
      panel->set_transform_setting(TransformSetting::Scale);

    ImGui::Separator();

    if (ImGui::MenuItem("  Axis: Free", nullptr, axis == TransformAxis::None, panel != nullptr))
      panel->set_transform_axis(TransformAxis::None);
    if (ImGui::MenuItem("  Axis: X", "X", axis == TransformAxis::X, panel != nullptr))
      panel->set_transform_axis(TransformAxis::X);
    if (ImGui::MenuItem("  Axis: Y", "Y", axis == TransformAxis::Y, panel != nullptr))
      panel->set_transform_axis(TransformAxis::Y);

    ImGui::Separator();

    if (ImGui::MenuItem(ICON_FA_GEAR "  Project Settings"))
      s_show_panels.project_settings_panel = true;

    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("  Window  "))
  {
    ImGui::MenuItem(ICON_FA_SITEMAP "  Scene Hierarchy", nullptr, &s_show_panels.scene_panel);
    ImGui::MenuItem(ICON_FA_LIST "  Properties / Inspector", nullptr, &s_show_panels.properties_panel);
    ImGui::MenuItem(ICON_FA_TERMINAL "  Console", nullptr, &s_show_panels.console_panel);
    ImGui::MenuItem(ICON_FA_GEAR "  Project Settings", nullptr, &s_show_panels.project_settings_panel);

    ImGui::Separator();

    if (ImGui::MenuItem(ICON_FA_BROOM "  Clear Console"))
      Log::get_console_sink() ? Log::get_console_sink()->clear() : void();

    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("  Help  "))
  {
    if (ImGui::MenuItem(ICON_FA_KEYBOARD "  Keyboard Shortcuts"))
      ImGui::OpenPopup("Keyboard Shortcuts##help");

    if (ImGui::MenuItem(ICON_FA_CIRCLE_INFO "  About AeroEditor"))
      ImGui::OpenPopup("About AeroEditor##help");

    ImGui::EndMenu();
  }

  // Help popups (modal) - can be opened from Help menu
  if (ImGui::BeginPopupModal("Keyboard Shortcuts##help", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    ImGui::TextUnformatted("Editor Shortcuts");
    ImGui::Separator();
    ImGui::BulletText("Ctrl+N / Ctrl+O / Ctrl+S: New, Open, Save Scene");
    ImGui::BulletText("Ctrl+Shift+S / Ctrl+Alt+S: Save As Default / Save All");
    ImGui::BulletText("G / R / S: Move, Rotate, Scale selected entity");
    ImGui::BulletText("X / Y: Toggle transform axis constraint");
    ImGui::BulletText("Ctrl+D: Duplicate selected entity");
    ImGui::BulletText("Ctrl+C / Ctrl+V: Copy / Paste entity");
    ImGui::BulletText("F2: Rename selected entity");
    ImGui::BulletText("Del: Delete selected entity");
    ImGui::BulletText("Ctrl+Shift+R: Make selected entity root");
    ImGui::BulletText("F5 / Ctrl+F5: Run default / current scene");
    ImGui::BulletText("Right-click hierarchy: context actions (create, rename, duplicate, etc.)");
    ImGui::Separator();
    if (ImGui::Button("Close", ImVec2(120, 0)))
      ImGui::CloseCurrentPopup();
    ImGui::EndPopup();
  }

  if (ImGui::BeginPopupModal("About AeroEditor##help", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    ImGui::TextUnformatted("AeroEditor");
    ImGui::Separator();
    ImGui::TextWrapped("Unity-inspired editor for AEROEngine projects.");
    ImGui::TextDisabled("Theme, viewport, console, and interaction tools are integrated in this build.");
    ImGui::Separator();
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    if (scene)
      ImGui::Text("Active Scene: %s", scene->get_name().c_str());
    ImGui::Separator();
    if (ImGui::Button("Close", ImVec2(120, 0)))
      ImGui::CloseCurrentPopup();
    ImGui::EndPopup();
  }

  // Right-aligned status info
  const float right_info_width = 320.0f;
  ImGui::SameLine(ImGui::GetWindowWidth() - right_info_width);
  if (scene)
    ImGui::TextDisabled(ICON_FA_IMAGE "  %s", scene->get_name().c_str());
  else
    ImGui::TextDisabled(ICON_FA_IMAGE "  No Scene");

  ImGui::SameLine();
  ImGui::TextDisabled("| " ICON_FA_GAUGE " %.1f FPS", ImGui::GetIO().Framerate);

  ImGui::EndMainMenuBar();
}

void UI::draw_texture(Entity entity)
{
  if (!entity.has_component<Texture_Component>())
    return;

  auto& props = entity.get_component<Texture_Component>();

  ImGui::Text("Texture");
  ImGui::SameLine();

  ImGui::Separator();

  float width;
  float height;
  if (props.texture)
  {
    width  = props.texture->get_width();
    height = props.texture->get_height();
  }
  else
  {
    width  = 128.0f;
    height = 64.0f;
  }
  float  aspect = width / height;
  ImVec2 preview_size(128.0f, 128.0f);
  if (aspect > 1.0f)
    preview_size.y = 128.0f / aspect;
  else
    preview_size.x = 128.0f * aspect;

  const float padding    = 10.0f;
  ImVec2      frame_size = ImVec2(preview_size.x + padding * 2, preview_size.y + padding * 2);

  float available_width = ImGui::GetContentRegionAvail().x;
  float frame_pos_x     = (available_width - frame_size.x) / 2.0f;

  ImGui::BeginChild("TexturePreviewFrame",
                    ImVec2(0, frame_size.y + 5),
                    false,
                    ImGuiWindowFlags_NoScrollbar);
  {
    ImGui::SetCursorPosX(frame_pos_x);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2      frame_min = ImGui::GetCursorScreenPos();
    ImVec2      frame_max = ImVec2(frame_min.x + frame_size.x, frame_min.y + frame_size.y);

    draw_list->AddRectFilled(frame_min, frame_max, ImGui::GetColorU32(ImGuiCol_FrameBg), 4.0f);

    draw_list->AddRect(frame_min, frame_max, ImGui::GetColorU32(ImGuiCol_Border), 4.0f, 0, 1.5f);

    ImVec2 image_min = ImVec2(frame_min.x + padding, frame_min.y + padding);
    ImVec2 image_max = ImVec2(image_min.x + preview_size.x, image_min.y + preview_size.y);

    ImGui::SetCursorScreenPos(image_min);

    ImGui::InvisibleButton("##TextureDropTarget", preview_size);

    if (ImGui::BeginDragDropTarget())
    {
      if (ImGui::IsMouseHoveringRect(image_min, image_max))
      {
        draw_list->AddRect(image_min,
                           image_max,
                           ImGui::GetColorU32(ImVec4(0.26f, 0.59f, 0.98f, 0.67f)),
                           4.0f,
                           0,
                           2.0f);

        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");
        if (payload)
        {
          const char* dropped_path = (const char*)payload->Data;

          std::string extension = std::filesystem::path(dropped_path).extension().string();
          std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

          if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
              extension == ".bmp" || extension == ".tga" || extension == ".hdr")
          {
            std::string relative_path = resolve_asset_path(dropped_path);
            if (!relative_path.empty())
            {
              try
              {
                props.path    = relative_path;
                props.texture = NodeHelper::load_texture(props.path, true, props.filter_mode);
              }
              catch (const std::exception& e)
              {
                props.texture.reset();
              }
            }
          }
        }
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILENAME"))
        {
          const char* dropped_path = (const char*)payload->Data;

          std::string extension = std::filesystem::path(dropped_path).extension().string();
          std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

          if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
              extension == ".bmp" || extension == ".tga" || extension == ".hdr")
          {
            std::string relative_path = resolve_asset_path(dropped_path);
            if (!relative_path.empty())
            {
              try
              {
                props.path    = relative_path;
                props.texture = NodeHelper::load_texture(props.path);
              }
              catch (const std::exception& e)
              {
                props.texture.reset();
              }
            }
          }
        }

        ImGui::EndDragDropTarget();
      }
    }

    if (props.texture)
    {
      ImGui::SetCursorScreenPos(image_min);
      ImGui::Image((void*)(intptr_t)props.texture->get_texture_id(), preview_size);

      // Show TextureRect selection overlay on preview if present
      if (entity.has_component<TextureRect_Component>())
      {
        const auto& tr = entity.get_component<TextureRect_Component>().rect;
        const float tw = (float)std::max(1u, props.texture->get_width());
        const float th = (float)std::max(1u, props.texture->get_height());
        if (tr.size.x > 0 && tr.size.y > 0)
        {
          ImVec2 rmin(image_min.x + (tr.position.x / tw) * preview_size.x,
                      image_min.y + (tr.position.y / th) * preview_size.y);
          ImVec2 rmax(image_min.x + ((tr.position.x + tr.size.x) / tw) * preview_size.x,
                      image_min.y + ((tr.position.y + tr.size.y) / th) * preview_size.y);
          // clamp to preview bounds
          rmin.x = std::max(rmin.x, image_min.x);
          rmin.y = std::max(rmin.y, image_min.y);
          rmax.x = std::min(rmax.x, image_min.x + preview_size.x);
          rmax.y = std::min(rmax.y, image_min.y + preview_size.y);
          draw_list->AddRectFilled(rmin, rmax, IM_COL32(255, 210, 80, 35));
          draw_list->AddRect(rmin, rmax, IM_COL32(255, 210, 80, 220), 0.0f, 0, 2.0f);
        }
      }

      // Hover tooltip with texture info and pixel position
      if (ImGui::IsItemHovered())
      {
        ImVec2 mouse = ImGui::GetIO().MousePos;
        float u = (mouse.x - image_min.x) / std::max(1.0f, preview_size.x);
        float v = (mouse.y - image_min.y) / std::max(1.0f, preview_size.y);
        int px = (int)std::floor(u * props.texture->get_width());
        int py = (int)std::floor(v * props.texture->get_height());
        px = std::clamp(px, 0, (int)props.texture->get_width() - 1);
        py = std::clamp(py, 0, (int)props.texture->get_height() - 1);

        ImGui::BeginTooltip();
        ImGui::Text("Texture: %dx%d", props.texture->get_width(), props.texture->get_height());
        if (!props.path.empty())
          ImGui::TextDisabled("%s", props.path.c_str());
        ImGui::Text("Pixel: (%d, %d)", px, py);
        if (entity.has_component<TextureRect_Component>())
        {
          const auto& tr = entity.get_component<TextureRect_Component>().rect;
          ImGui::Separator();
          ImGui::Text("Rect: (%u,%u) %ux%u", tr.position.x, tr.position.y, tr.size.x, tr.size.y);
        }
        ImGui::TextDisabled("Drop a texture here to replace");
        ImGui::EndTooltip();
      }
    }
    else
    {
      ImGui::SetCursorScreenPos(image_min);

      const float checker_size = 16.0f;
      const ImU32 col1         = ImGui::GetColorU32(ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
      const ImU32 col2         = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

      for (float y = 0; y < preview_size.y; y += checker_size)
      {
        for (float x = 0; x < preview_size.x; x += checker_size)
        {
          bool is_dark = ((int)(x / checker_size) + (int)(y / checker_size)) % 2 == 0;
          draw_list->AddRectFilled(
              ImVec2(image_min.x + x, image_min.y + y),
              ImVec2(image_min.x + std::min(x + checker_size, preview_size.x),
                      image_min.y + std::min(y + checker_size, preview_size.y)),
              is_dark ? col1 : col2);
        }
      }

      const char* drop_text = "Drop Texture Here";
      ImVec2      text_size = ImGui::CalcTextSize(drop_text);
      ImVec2      text_pos  = ImVec2(image_min.x + (preview_size.x - text_size.x) * 0.5f,
                               image_min.y + (preview_size.y - text_size.y) * 0.5f);

      draw_list->AddText(ImVec2(text_pos.x + 1, text_pos.y + 1),
                         ImGui::GetColorU32(ImVec4(0, 0, 0, 0.5f)),
                         drop_text);
      draw_list->AddText(text_pos, ImGui::GetColorU32(ImVec4(1, 1, 1, 0.8f)), drop_text);
    }
  }
  ImGui::EndChild();
  if (props.texture)
  {
    ImGui::Text("Size: %dx%d", props.texture->get_width(), props.texture->get_height());

    ImGui::SameLine();
    ImGui::TextDisabled("| %s", std::filesystem::path(props.path).extension().string().c_str());
  }
  else if (!props.path.empty())
  {
    ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1),
                       "Failed to load: %s",
                       std::filesystem::path(props.path).filename().string().c_str());
  }
  else
  {
    ImGui::TextDisabled("No texture loaded");
  }
  ImGui::Spacing();
  ImGui::Separator();

  {
    std::vector<std::string> load_mode;
    load_mode.push_back("Linear");
    load_mode.push_back("Nearest");
    draw_enum("Filter Mode", props.filter_mode, load_mode);
  }

  float button_spacing = 5.0f;
  float button_width   = (ImGui::GetContentRegionAvail().x - button_spacing) * 0.5f;

  if (ImGui::Button(ICON_FA_DOWNLOAD "  Load Texture", ImVec2(button_width, button_height)))
  {
    EditorLayer::get().load_texture(entity);
  }

  ImGui::SameLine(0, 5.0f);

  if (!props.path.empty())
  {
    if (ImGui::Button(ICON_FA_UPLOAD "  Reload", ImVec2(button_width, button_height)))
    {
      try
      {
        props.texture = NodeHelper::load_texture(props.path, true, props.filter_mode);
      }
      catch (const std::exception& e)
      {
        props.texture.reset();
      }
    }
  }
  else
  {
    if (ImGui::Button("Clear", ImVec2(button_width, 0)))
    {
      props.texture.reset();
      props.path.clear();
    }
  }

  ImGui::Spacing();
}

void UI::draw_font_selector(Entity entity)
{
  if (!entity.has_component<Font_Component>())
    Font_Component::add_component(entity);

  auto& props = entity.get_component<Font_Component>();

  const auto fonts = FontManager::list_fonts();
  std::vector<std::string> labels;
  labels.push_back("Default");
  for (const auto& font : fonts)
    labels.push_back(FontManager::display_name(font));

  std::vector<const char*> items;
  items.reserve(labels.size());
  for (const auto& label : labels)
    items.push_back(label.c_str());

  int current = 0;
  if (!props.font_path.empty())
  {
    for (size_t i = 0; i < fonts.size(); ++i)
    {
      if (fonts[i] == props.font_path)
      {
        current = static_cast<int>(i) + 1;
        break;
      }
    }
  }

  if (ImGui::Combo("Font", &current, items.data(), static_cast<int>(items.size())))
  {
    props.font_path = current == 0 ? std::string() : fonts[current - 1];
  }

  if (ImGui::BeginDragDropTarget())
  {
    static std::string drop_message;
    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
    {
      std::string dropped = static_cast<const char*>(payload->Data);
      Helper::normalize_path(dropped);
      std::string extension = std::filesystem::path(dropped).extension().string();
      std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

      if (!Project::is_path_in_active_directory(dropped, ProjectDirectory::Assets))
      {
        drop_message = "Fonts must be inside the project Assets folder.";
      }
      else if (extension == ".ttf" || extension == ".otf")
      {
        FontImportResult result = FontImporter::import_font(dropped);
        drop_message            = result.message;
        if (result.success)
        {
          props.font_path = result.project_path;
          FontManager::reload();
        }
      }
      else if (extension == ".json")
      {
        props.font_path = Project::to_active_relative_path(dropped, ProjectDirectory::Assets);
        drop_message    = props.font_path.empty() ? "Could not resolve the dropped font atlas."
                                                  : "Font atlas assigned.";
        if (!props.font_path.empty())
          FontManager::reload();
      }
      else
      {
        drop_message = "Drop a .ttf, .otf, or generated .json font atlas.";
      }
    }
    if (!drop_message.empty())
      ImGui::SetTooltip("%s", drop_message.c_str());
    ImGui::EndDragDropTarget();
  }

  ImGui::TextDisabled("Atlas: %s", FontManager::display_name(props.font_path).c_str());

  static std::string import_message;
  if (!import_message.empty())
    ImGui::TextWrapped("%s", import_message.c_str());

  const float button_width = ImGui::GetContentRegionAvail().x;
  if (ImGui::Button(ICON_FA_DOWNLOAD "  Import Font...", ImVec2(button_width, button_height)))
  {
    std::string source = FileDialogs::open_file(
        "Font Files (*.ttf;*.otf)\0*.ttf;*.otf\0All Files (*.*)\0*.*\0",
        Project::get_dialog_directory(ProjectDirectory::Assets));

    if (!source.empty())
    {
      if (Project::is_path_in_active_directory(source, ProjectDirectory::Assets))
      {
        FontImportResult result = FontImporter::import_font(source);
        import_message          = result.message;

        if (result.success)
        {
          props.font_path = result.project_path;
          FontManager::reload();
        }
      }
      else
      {
        import_message = "Font files must be inside the project Assets folder.";
        AERO_CORE_ERROR("Font file \"{0}\" is outside the project Assets folder.", source);
      }
    }
  }
}

void UI::draw_animation(Entity entity)
{
  static std::string current_animation_popup_target = "";

  std::string                         delete_animation;
  std::pair<std::string, std::string> rename_animation;

  auto& anim = entity.get_component<Animation_Component>();
  AG_ref<Texture2D> preview_texture;
  vec2f texture_size = {0.0f, 0.0f};
  if (entity.has_component<Texture_Component>())
  {
    auto& tex = entity.get_component<Texture_Component>();
    preview_texture = tex.texture;
    if (preview_texture)
      texture_size = preview_texture->get_size();
  }

  ImGui::SeparatorText("Playback");

  const bool has_current = !anim.current_animation.empty() &&
                           anim.animations.find(anim.current_animation) != anim.animations.end();
  Animation* current_anim = has_current ? &anim.animations[anim.current_animation] : nullptr;
  const int frame_count = current_anim ? (int)current_anim->frames.size() : 0;

  if (ImGui::Button(anim.playing ? "Pause" : "Play", ImVec2(80, 0)))
    anim.playing = !anim.playing;
  ImGui::SameLine();
  if (ImGui::Button("Restart", ImVec2(80, 0)))
  {
    anim.timer = 0.0f;
    anim.current_frame = 0;
    anim.current_animation_completed = false;
    anim.reverse_direction = false;
  }
  ImGui::SameLine();
  ImGui::BeginDisabled(!current_anim || frame_count <= 0);
  if (ImGui::Button("<", ImVec2(28, 0)))
  {
    anim.playing = false;
    anim.current_frame = std::max(0, anim.current_frame - 1);
  }
  ImGui::SameLine();
  if (ImGui::Button(">", ImVec2(28, 0)))
  {
    anim.playing = false;
    anim.current_frame = (frame_count > 0) ? std::min(frame_count - 1, anim.current_frame + 1) : 0;
  }
  ImGui::EndDisabled();

  if (current_anim && frame_count > 0)
  {
    int frame_idx = std::clamp(anim.current_frame, 0, frame_count - 1);
    if (ImGui::SliderInt("Frame", &frame_idx, 0, frame_count - 1))
    {
      anim.current_frame = frame_idx;
      anim.playing = false;
    }

    const float fps = std::max(current_anim->fps, 0.001f);
    ImGui::Text("Time: %.2fs / %.2fs", anim.timer, (frame_count / fps));
    ImGui::SameLine();
    ImGui::TextDisabled("| %s", anim.current_animation_completed ? "Completed" : (anim.playing ? "Playing" : "Paused"));

    // Current frame preview
    if (preview_texture && texture_size.x > 0.0f && texture_size.y > 0.0f)
    {
      const auto& fr = current_anim->frames[frame_idx].frame_rect;
      ImVec2 uv0((float)fr.position.x / texture_size.x, (float)fr.position.y / texture_size.y);
      ImVec2 uv1((float)(fr.position.x + fr.size.x) / texture_size.x,
                 (float)(fr.position.y + fr.size.y) / texture_size.y);
      const float preview_h = 72.0f;
      const float aspect = (fr.size.y > 0) ? (float)fr.size.x / (float)fr.size.y : 1.0f;
      ImGui::Image((ImTextureID)(intptr_t)preview_texture->get_texture_id(), ImVec2(preview_h * aspect, preview_h), uv0, uv1);
      ImGui::SameLine();
      ImGui::BeginGroup();
      ImGui::Text("Rect: (%u,%u) %ux%u", fr.position.x, fr.position.y, fr.size.x, fr.size.y);
      ImGui::Text("Animation: %s", anim.current_animation.c_str());
      ImGui::EndGroup();
    }
  }
  else
  {
    ImGui::TextDisabled("No active animation or no frames.");
  }

  ImGui::Spacing();
  ImGui::SeparatorText("Current Animation");

  if (ImGui::BeginCombo("Current Animation",
                        anim.current_animation.empty() ? "None" : anim.current_animation.c_str()))
  {
    if (ImGui::Selectable("None", anim.current_animation.empty()))
    {
      anim.current_animation = "";
      anim.current_frame = 0;
      anim.timer = 0.0f;
      anim.current_animation_completed = false;
    }

    for (auto& [name, _] : anim.animations)
    {
      bool is_selected = (anim.current_animation == name);
      if (ImGui::Selectable(name.c_str(), is_selected))
      {
        anim.current_animation = name;
        anim.current_frame = 0;
        anim.timer = 0.0f;
        anim.current_animation_completed = false;
        anim.reverse_direction = false;
      }
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }

  ImGui::Spacing();
  ImGui::SeparatorText("Animation Clips");

  static char new_anim_name[128] = "";
  static bool show_animation_exists = false;
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 70.0f);
  ImGui::InputTextWithHint("##NewAnimName", "New animation name...", new_anim_name, IM_ARRAYSIZE(new_anim_name));
  ImGui::SameLine();
  if (ImGui::Button("Add", ImVec2(60.0f, 0)) && strlen(new_anim_name) > 0)
  {
    if (anim.animations.find(new_anim_name) == anim.animations.end())
    {
      Animation new_anim;
      new_anim.name = new_anim_name;
      anim.animations[new_anim_name] = new_anim;
      if (anim.current_animation.empty())
        anim.current_animation = new_anim_name;
      new_anim_name[0] = '\0';
      Scene::get_active_scene()->set_save_required();
    }
    else
    {
      show_animation_exists = true;
    }
  }

  if (show_animation_exists)
  {
    ImGui::OpenPopup("Animation Exists##dup");
    show_animation_exists = false;
  }
  if (ImGui::BeginPopupModal("Animation Exists##dup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    ImGui::Text("An animation with that name already exists.");
    if (ImGui::Button("OK", ImVec2(120, 0)))
      ImGui::CloseCurrentPopup();
    ImGui::EndPopup();
  }

  ImGui::TextDisabled("%d clip(s)", (int)anim.animations.size());
  ImGui::Spacing();

  std::vector<std::string> animation_names;
  animation_names.reserve(anim.animations.size());
  for (const auto& [name, _] : anim.animations)
    animation_names.push_back(name);

  for (const auto& name : animation_names)
  {
    auto it = anim.animations.find(name);
    if (it == anim.animations.end())
      continue;
    Animation& animation = it->second;

    ImGui::PushID(name.c_str());
    const bool is_current = (anim.current_animation == name);
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
                               ImGuiTreeNodeFlags_AllowItemOverlap | (is_current ? ImGuiTreeNodeFlags_DefaultOpen : 0);

    bool open = ImGui::TreeNodeEx("##anim_node", flags, "%s%s  (%d frames)", is_current ? "* " : "", name.c_str(), (int)animation.frames.size());

    // Inline actions on header
    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 150.0f);
    if (ImGui::SmallButton(is_current ? "Active" : "Set Active"))
    {
      anim.current_animation = name;
      anim.current_frame = 0;
      anim.timer = 0.0f;
      anim.current_animation_completed = false;
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Delete"))
      ImGui::OpenPopup("Delete Animation##confirm");

    if (ImGui::BeginPopupModal("Delete Animation##confirm", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
      ImGui::Text("Delete animation '%s'?", name.c_str());
      ImGui::Separator();
      if (ImGui::Button("Delete", ImVec2(100, 0)))
      {
        delete_animation = name;
        ImGui::CloseCurrentPopup();
      }
      ImGui::SameLine();
      if (ImGui::Button("Cancel", ImVec2(100, 0)))
        ImGui::CloseCurrentPopup();
      ImGui::EndPopup();
    }

    if (open)
    {
      std::string display_name = animation.name;
      if (UI::draw_string("Name", display_name))
      {
        if (!display_name.empty() && display_name != name && anim.animations.find(display_name) == anim.animations.end())
        {
          animation.name = display_name;
          rename_animation = {name, display_name};
        }
      }

      UI::draw_value("FPS", animation.fps);
      if (animation.fps < 0.0f) animation.fps = 0.0f;
      UI::draw_bool("Loop", animation.loop);
      ImGui::SameLine();
      UI::draw_bool("Ping Pong", animation.ping_pong);

      ImGui::Spacing();
      ImGui::Text("Frames: %d", (int)animation.frames.size());
      if (!animation.frames.empty())
      {
        const float fps = std::max(animation.fps, 0.001f);
        ImGui::SameLine();
        ImGui::TextDisabled("(%.3fs/frame, %.2fs total)", 1.0f / fps, (float)animation.frames.size() / fps);
      }

      // Frame strip preview
      if (preview_texture && texture_size.x > 0.0f && texture_size.y > 0.0f && !animation.frames.empty())
      {
        ImGui::BeginChild("##frame_strip", ImVec2(0, 64), true, ImGuiWindowFlags_HorizontalScrollbar);
        for (size_t i = 0; i < animation.frames.size(); ++i)
        {
          const auto& fr = animation.frames[i].frame_rect;
          ImVec2 uv0((float)fr.position.x / texture_size.x, (float)fr.position.y / texture_size.y);
          ImVec2 uv1((float)(fr.position.x + fr.size.x) / texture_size.x,
                     (float)(fr.position.y + fr.size.y) / texture_size.y);
          const float h = 40.0f;
          const float aspect = (fr.size.y > 0) ? (float)fr.size.x / (float)fr.size.y : 1.0f;

          ImGui::PushID((int)i);
          if (is_current && (int)i == anim.current_frame)
          {
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x - 2, p.y - 2), ImVec2(p.x + h * aspect + 2, p.y + h + 2), IM_COL32(255, 170, 40, 80), 3.0f);
          }
          if (ImGui::ImageButton("##fr", (ImTextureID)(intptr_t)preview_texture->get_texture_id(), ImVec2(h * aspect, h), uv0, uv1))
          {
            anim.current_animation = name;
            anim.current_frame = (int)i;
            anim.playing = false;
          }
          if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Frame %zu\nRect: (%u,%u) %ux%u", i, fr.position.x, fr.position.y, fr.size.x, fr.size.y);
          ImGui::SameLine();
          ImGui::PopID();
        }
        ImGui::NewLine();
        ImGui::EndChild();
      }

      ImGui::Spacing();
      const float btn_w = (ImGui::GetContentRegionAvail().x - 8.0f) * 0.5f;
      if (ImGui::Button("Select Frames...", ImVec2(btn_w, 30.0f)))
      {
        if (preview_texture)
        {
          current_animation_popup_target = name;
          s_show_panels.animation_selector = true;
        }
        else
        {
          ImGui::OpenPopup("No Texture##FrameSelector");
        }
      }
      ImGui::SameLine();
      ImGui::BeginDisabled(animation.frames.empty());
      if (ImGui::Button("Clear Frames", ImVec2(btn_w, 30.0f)))
      {
        animation.frames.clear();
        if (is_current)
        {
          anim.current_frame = 0;
          anim.current_animation_completed = false;
        }
        Scene::get_active_scene()->set_save_required();
      }
      ImGui::EndDisabled();

      if (ImGui::BeginPopup("No Texture##FrameSelector"))
      {
        ImGui::Text("Entity needs a Texture Component with a loaded texture.");
        if (ImGui::Button("OK"))
          ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
      }

      ImGui::TreePop();
    }
    ImGui::PopID();
  }

  // rename animation (deferred)
  if (!rename_animation.first.empty())
  {
    auto node = anim.animations.extract(rename_animation.first);
    if (!node.empty())
    {
      node.key() = rename_animation.second;
      if (anim.current_animation == rename_animation.first)
        anim.current_animation = rename_animation.second;
      anim.animations.insert(std::move(node));
      Scene::get_active_scene()->set_save_required();
    }
  }

  // delete animation (deferred)
  if (!delete_animation.empty())
  {
    anim.animations.erase(delete_animation);
    if (anim.current_animation == delete_animation)
    {
      anim.current_animation.clear();
      anim.current_frame = 0;
      anim.timer = 0.0f;
      anim.current_animation_completed = false;
    }
    Scene::get_active_scene()->set_save_required();
  }

  if (s_show_panels.animation_selector)
    draw_frame_selector(entity, current_animation_popup_target);
}

void UI::draw_frame_selector(Entity entity, const std::string& current_animation)
{
  static std::vector<int> selected_frames;
  static vec2u            grid_size = {10, 10};
  static bool             use_size  = false;
  static std::string      last_target_animation;

  if (current_animation != last_target_animation)
  {
    selected_frames.clear();
    last_target_animation = current_animation;
  }

  vec2f cell_size = {0.0f, 0.0f};
  vec2i total_grid = {0, 0};

  auto& anim = entity.get_component<Animation_Component>();
  AG_ref<Texture2D> tex;
  vec2f texture_size = {0.0f, 0.0f};
  if (entity.has_component<Texture_Component>())
  {
    tex = entity.get_component<Texture_Component>().texture;
    if (tex)
      texture_size = tex->get_size();
  }

  PopUpModel model;
  model.name         = "Frame Selection";
  model.id           = "Select Frames from Texture";
  model.confirm_name = "Apply Frames";
  model.close_name   = "Cancel";

  model.on_close = [&]()
  {
    s_show_panels.animation_selector = false;
    selected_frames.clear();
  };

  Extra_Settings extra_settings;
  extra_settings.size     = grid_size;
  extra_settings.use_size = use_size;

  extra_settings.button_controls = [&](const Button_Control_Value& details)
  {
    const int cell_index = details.tile_id.y * details.line_count.x + details.tile_id.x;
    total_grid = details.line_count;

    // details.size is vec2i (may be truncated); derive accurate float cell size in texture pixels
    vec2f actual_cell_size = vec2f((float)details.size.x, (float)details.size.y);
    if (details.scale.x != 0.0f) actual_cell_size.x = details.button_size.x / details.scale.x;
    if (details.scale.y != 0.0f) actual_cell_size.y = details.button_size.y / details.scale.y;
    cell_size = actual_cell_size;

    const bool is_selected = std::find(selected_frames.begin(), selected_frames.end(), cell_index) != selected_frames.end();

    // Compute cell bounds from button size in screen space (accurate even with fractional texture cell sizes)
    const vec2f cell_min = details.screen_pos +
                           vec2f(details.tile_id.x * details.button_size.x, details.tile_id.y * details.button_size.y);
    const vec2f cell_max = cell_min + details.button_size;

    // Selection overlay and order badge
    if (is_selected)
    {
      details.draw_list->AddRectFilled(cell_min.to_imvec2(), cell_max.to_imvec2(), IM_COL32(90, 200, 120, 70));
      details.draw_list->AddRect(cell_min.to_imvec2(), cell_max.to_imvec2(), IM_COL32(120, 255, 160, 220), 0.0f, 0, 2.5f);

      auto it = std::find(selected_frames.begin(), selected_frames.end(), cell_index);
      if (it != selected_frames.end())
      {
        const int order = (int)std::distance(selected_frames.begin(), it) + 1;
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%d", order);
        details.draw_list->AddRectFilled(ImVec2(cell_min.x + 2, cell_min.y + 2),
                                         ImVec2(cell_min.x + 22, cell_min.y + 18),
                                         IM_COL32(30, 30, 30, 200), 3.0f);
        details.draw_list->AddText(ImVec2(cell_min.x + 5, cell_min.y + 2), IM_COL32(255, 220, 120, 255), buf);
      }
    }

    ImGui::PushID(cell_index);
    ImGui::SetCursorScreenPos(cell_min.to_imvec2());
    if (ImGui::InvisibleButton("##Cell", details.button_size.to_imvec2()))
    {
      const bool ctrl_down  = ImGui::GetIO().KeyCtrl;
      const bool shift_down = ImGui::GetIO().KeyShift;

      if (ctrl_down)
      {
        auto it = std::find(selected_frames.begin(), selected_frames.end(), cell_index);
        if (it != selected_frames.end())
          selected_frames.erase(it);
        else
          selected_frames.push_back(cell_index);
      }
      else if (shift_down && !selected_frames.empty())
      {
        const int last_selected = selected_frames.back();
        const int start = std::min(last_selected, cell_index);
        const int end   = std::max(last_selected, cell_index);
        for (int i = start; i <= end; ++i)
        {
          if (std::find(selected_frames.begin(), selected_frames.end(), i) == selected_frames.end())
            selected_frames.push_back(i);
        }
      }
      else
      {
        selected_frames.clear();
        selected_frames.push_back(cell_index);
      }
    }

    if (ImGui::IsItemHovered())
    {
      const float rx = details.tile_id.x * actual_cell_size.x;
      const float ry = details.tile_id.y * actual_cell_size.y;
      details.draw_list->AddRect(cell_min.to_imvec2(), cell_max.to_imvec2(), IM_COL32(255, 255, 255, 220), 0.0f, 0, 2.0f);
      ImGui::BeginTooltip();
      ImGui::Text("Cell %d  (col %d, row %d)", cell_index, details.tile_id.x, details.tile_id.y);
      ImGui::Text("Rect: (%.1f, %.1f, %.1f, %.1f)", rx, ry, actual_cell_size.x, actual_cell_size.y);
      if (is_selected)
        ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.6f, 1.0f), "Selected in animation order");
      ImGui::TextDisabled("Click: select, Ctrl: toggle, Shift: range");
      ImGui::EndTooltip();
    }
    ImGui::PopID();
  };

  extra_settings.controls_panel = [&]()
  {
    const float available_height = ImGui::GetContentRegionAvail().y;
    ImGui::Text("Frame Selection");
    ImGui::Separator();
    ImGui::TextDisabled("Animation: %s", current_animation.empty() ? "(none)" : current_animation.c_str());
    ImGui::Spacing();

    ImGui::Text("Grid Settings");
    vec2u new_grid = grid_size;
    if (UI::draw_vec2("Grid", new_grid))
    {
      new_grid.x = std::max(1u, new_grid.x);
      new_grid.y = std::max(1u, new_grid.y);
      if (new_grid != grid_size)
      {
        grid_size = new_grid;
        selected_frames.clear();
      }
    }
    if (UI::draw_bool("Use Cell Size", use_size))
      selected_frames.clear();

    ImGui::Spacing();
    if (cell_size.x > 0.0f && cell_size.y > 0.0f)
      ImGui::Text("Cell: %.0fx%.0f  |  Grid: %dx%d", cell_size.x, cell_size.y, total_grid.x, total_grid.y);
    if (texture_size.x > 0.0f)
      ImGui::TextDisabled("Texture: %.0fx%.0f", texture_size.x, texture_size.y);

    ImGui::Separator();
    ImGui::Text("Selected Frames: %d", (int)selected_frames.size());

    const float total_width = ImGui::GetContentRegionAvail().x;
    const float button_spacing = 6.0f;
    GUI_Button button;
    button.size = {(total_width - button_spacing) * 0.5f, 32.0f};

    if (ImGui::SmallButton("Select All"))
    {
      selected_frames.clear();
      const int count = std::max(0, total_grid.x * total_grid.y);
      for (int i = 0; i < count; ++i)
        selected_frames.push_back(i);
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Clear"))
      selected_frames.clear();
    ImGui::SameLine();
    if (ImGui::SmallButton("Reverse") && selected_frames.size() > 1)
      std::reverse(selected_frames.begin(), selected_frames.end());
    ImGui::SameLine();
    if (ImGui::SmallButton("Sort"))
      std::sort(selected_frames.begin(), selected_frames.end());

    // Selected frame strip preview (interactive order)
    if (tex && texture_size.x > 0.0f && texture_size.y > 0.0f && !selected_frames.empty() && cell_size.x > 0.0f && cell_size.y > 0.0f)
    {
      ImGui::Spacing();
      ImGui::Text("Animation Order Preview");
      ImGui::BeginChild("##selected_strip", ImVec2(0, 78), true, ImGuiWindowFlags_HorizontalScrollbar);
      int remove_idx = -1;
      for (size_t i = 0; i < selected_frames.size(); ++i)
      {
        const int idx = selected_frames[i];
        const int col = (total_grid.x > 0) ? (idx % total_grid.x) : 0;
        const int row = (total_grid.x > 0) ? (idx / total_grid.x) : 0;
        const float px = col * cell_size.x;
        const float py = row * cell_size.y;
        ImVec2 uv0(px / texture_size.x, py / texture_size.y);
        ImVec2 uv1((px + cell_size.x) / texture_size.x, (py + cell_size.y) / texture_size.y);
        const float h = 44.0f;
        const float aspect = (cell_size.y > 0.0f) ? (cell_size.x / cell_size.y) : 1.0f;

        ImGui::PushID((int)i);
        ImGui::BeginGroup();
        if (ImGui::ImageButton("##sel", (ImTextureID)(intptr_t)tex->get_texture_id(), ImVec2(h * aspect, h), uv0, uv1))
          remove_idx = (int)i;
        ImGui::TextDisabled("#%zu", i + 1);
        ImGui::EndGroup();
        if (ImGui::IsItemHovered())
          ImGui::SetTooltip("Frame %zu (cell %d). Click to remove from selection.", i + 1, idx);
        ImGui::SameLine();
        ImGui::PopID();
      }
      if (remove_idx >= 0 && remove_idx < (int)selected_frames.size())
        selected_frames.erase(selected_frames.begin() + remove_idx);
      ImGui::NewLine();
      ImGui::EndChild();
      ImGui::TextDisabled("Click a preview frame to remove it from the sequence.");
    }
    else if (selected_frames.empty())
    {
      ImGui::Spacing();
      ImGui::TextColored(ImVec4(1, 0.7f, 0.3f, 1), "No frames selected");
      ImGui::BulletText("Click a cell to select a frame");
      ImGui::BulletText("Ctrl+Click toggles frames");
      ImGui::BulletText("Shift+Click selects a range");
      ImGui::BulletText("Selection order defines animation order");
    }

    ImGui::Separator();
    ImGui::SetCursorPosY(std::max(ImGui::GetCursorPosY(), available_height - 36.0f));

    button.label = "Apply Frames";
    button.enabled = !selected_frames.empty() && !current_animation.empty() && cell_size.x > 0.0f && cell_size.y > 0.0f;
    if (draw_button(button))
    {
      auto it = anim.animations.find(current_animation);
      if (it != anim.animations.end())
      {
        Animation& target_anim = it->second;
        target_anim.frames.clear();
        for (int cell_index : selected_frames)
        {
          const int row = (total_grid.x > 0) ? (cell_index / total_grid.x) : 0;
          const int col = (total_grid.x > 0) ? (cell_index % total_grid.x) : 0;

          Frame new_frame;
          new_frame.frame_rect.position.x = (uint32_t)(col * cell_size.x);
          new_frame.frame_rect.position.y = (uint32_t)(row * cell_size.y);
          new_frame.frame_rect.size.x     = (uint32_t)cell_size.x;
          new_frame.frame_rect.size.y     = (uint32_t)cell_size.y;
          target_anim.frames.push_back(new_frame);
        }
        Scene::get_active_scene()->set_save_required();
        s_show_panels.animation_selector = false;
        selected_frames.clear();
      }
    }
    ImGui::SameLine(0.0f, button_spacing);
    button.label = "Cancel";
    button.enabled = true;
    if (draw_button(button))
    {
      s_show_panels.animation_selector = false;
      selected_frames.clear();
    }
  };

  Texture_PopUp::draw_popup(model, extra_settings, entity);
}

void UI::draw_tilemap_register(Entity entity)
{
  static bool               show_register = false;
  static std::vector<vec2u> selected_tiles;
  static std::vector<vec2u> solid_tiles;
  static vec2i              tile_size = {32, 32};
  static vec2f              last_mouse_pos;
  static vec2f              current_mouse_pos;
  static bool               is_panning      = false;
  static bool               edit_solid_mode = false;
  static int                drag_action     = -1; // -1 none, 0 add/select, 1 remove/unselect
  static vec2f              view_size       = {800.0f, 600.0f};
  static vec2f              view_center     = {0.0f, 0.0f};

  auto contains_tile = [](const std::vector<vec2u>& tiles, const vec2u& id) {
    return std::find(tiles.begin(), tiles.end(), id) != tiles.end();
  };
  auto add_tile = [](std::vector<vec2u>& tiles, const vec2u& id) {
    if (std::find(tiles.begin(), tiles.end(), id) == tiles.end())
      tiles.push_back(id);
  };
  auto remove_tile = [](std::vector<vec2u>& tiles, const vec2u& id) {
    auto it = std::find(tiles.begin(), tiles.end(), id);
    if (it != tiles.end())
      tiles.erase(it);
  };

  if (ImGui::Button("Register TileSet"))
  {
    if (!entity.has_component<Texture_Component>() || !entity.get_component<Texture_Component>().texture)
      return;

    show_register    = true;
    selected_tiles.clear();
    solid_tiles.clear();
    edit_solid_mode  = false;
    drag_action      = -1;
    view_center      = {0.0f, 0.0f};
    view_size        = {800.0f, 600.0f};

    if (entity.has_component<TileSet_Component>())
    {
      auto& tile_set = entity.get_component<TileSet_Component>();
      for (const auto& [id, def] : tile_set.tile_definitions)
      {
        selected_tiles.push_back(id);
        if (def.is_solid)
          solid_tiles.push_back(id);
      }
      if (tile_set.tile_size.x > 0 && tile_set.tile_size.y > 0)
        tile_size = tile_set.tile_size;
    }

    ImGui::OpenPopup("TileSet Register");
  }

  if (!show_register)
    return;

  custom_popup(
      "TileSet Register",
      "TileSet Register",
      [&]() mutable
      {
        if (!entity.has_component<Texture_Component>() || !entity.get_component<Texture_Component>().texture)
        {
          ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1), "No texture available for tile registration.");
          if (ImGui::Button("Close"))
          {
            show_register = false;
            ImGui::CloseCurrentPopup();
          }
          return;
        }

        auto&       texture      = entity.get_component<Texture_Component>().texture;
        vec2f       texture_size = texture->get_size();
        ImTextureID texture_id   = (ImTextureID)(intptr_t)texture->get_texture_id();

        const float available_width = ImGui::GetContentRegionAvail().x;
        const float left_width      = available_width * 0.68f - 6.0f;
        const float right_width     = available_width * 0.32f - 6.0f;

        ImGui::BeginChild("TileRegister_Left", ImVec2(left_width, 0), true);
        {
          ImGui::Text("Tileset Atlas");
          ImGui::SameLine();
          ImGui::TextDisabled("(MMB pan, wheel zoom, LMB paint/select)");
          ImGui::Separator();

          ImGui::BeginChild("TileRegister_TextureCanvas",
                            ImVec2(0, 0),
                            true,
                            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
          {
            const vec2f mouse_screen = ImGui::GetMousePos();
            const vec2f child_pos    = ImGui::GetWindowPos();
            current_mouse_pos        = mouse_screen - child_pos;

            const vec2f container = ImGui::GetContentRegionAvail();
            const vec2f container_start_pos = ImGui::GetCursorScreenPos();
            const bool hovering_canvas = ImGui::IsMouseHoveringRect(container_start_pos.to_imvec2(),
                                                                    (container_start_pos + container).to_imvec2());

            if (hovering_canvas && ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
            {
              is_panning     = true;
              last_mouse_pos = current_mouse_pos;
            }
            if (is_panning)
            {
              if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
              {
                vec2f start = Math::screen_to_world(last_mouse_pos,
                                                    Math::get_float_rect(view_size, view_center),
                                                    container);
                vec2f end   = Math::screen_to_world(current_mouse_pos,
                                                  Math::get_float_rect(view_size, view_center),
                                                  container);
                view_center += (start - end);
                last_mouse_pos = current_mouse_pos;
              }
              else
              {
                is_panning = false;
              }
            }

            if (hovering_canvas && ImGui::GetIO().MouseWheel != 0.0f)
            {
              const float scale_factor = (ImGui::GetIO().MouseWheel > 0.0f) ? 0.9f : 1.1f;
              vec2f world_before = Math::screen_to_world(current_mouse_pos,
                                                         Math::get_float_rect(view_size, view_center),
                                                         container);
              view_size *= scale_factor;
              view_size.x = std::clamp(view_size.x, 64.0f, 20000.0f);
              view_size.y = std::clamp(view_size.y, 64.0f, 20000.0f);
              vec2f world_after = Math::screen_to_world(current_mouse_pos,
                                                        Math::get_float_rect(view_size, view_center),
                                                        container);
              view_center += (world_before - world_after);
            }

            const vec2f world_pos   = vec2f(0, 0) - texture_size * 0.5f;
            vec2f screen_pos        = Math::world_to_screen(world_pos,
                                                     Math::get_float_rect(view_size, view_center),
                                                     container);
            const vec2f screen_size = Math::world_size_to_screen_size(texture_size, view_size, container);

            ImGui::SetCursorPos(screen_pos.to_imvec2());
            ImGui::Image(texture_id, screen_size.to_imvec2());
            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            const vec2f window_pos = ImGui::GetWindowPos();
            screen_pos += window_pos;

            vec2i total_dim = {0, 0};
            if (tile_size.x > 0 && tile_size.y > 0)
              total_dim = vec2i(texture_size / vec2f((float)tile_size.x, (float)tile_size.y));

            // Grid
            if (tile_size.x > 0 && tile_size.y > 0)
            {
              for (int x = 0; x <= total_dim.x; ++x)
              {
                float px = screen_pos.x + x * tile_size.x * (screen_size.x / texture_size.x);
                draw_list->AddLine(ImVec2(px, screen_pos.y),
                                   ImVec2(px, screen_pos.y + screen_size.y),
                                   IM_COL32(255, 255, 255, 45),
                                   1.0f);
              }
              for (int y = 0; y <= total_dim.y; ++y)
              {
                float py = screen_pos.y + y * tile_size.y * (screen_size.y / texture_size.y);
                draw_list->AddLine(ImVec2(screen_pos.x, py),
                                   ImVec2(screen_pos.x + screen_size.x, py),
                                   IM_COL32(255, 255, 255, 45),
                                   1.0f);
              }
            }

            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
              drag_action = -1;

            if (tile_size.x > 0 && tile_size.y > 0)
            {
              const vec2f cell_btn_size = Math::world_size_to_screen_size(
                  vec2f((float)tile_size.x, (float)tile_size.y), view_size, container);
              const vec2f scale = screen_size / texture_size;

              for (int y = 0; y < total_dim.y; ++y)
              {
                for (int x = 0; x < total_dim.x; ++x)
                {
                  const vec2u tid = {(AG_uint)x, (AG_uint)y};
                  const bool is_selected_tile = contains_tile(selected_tiles, tid);
                  const bool is_solid_tile    = contains_tile(solid_tiles, tid) && is_selected_tile;

                  const vec2f cell_min = screen_pos + vec2f(x * (float)tile_size.x, y * (float)tile_size.y) * scale;
                  const vec2f cell_max = screen_pos + vec2f((x + 1) * (float)tile_size.x, (y + 1) * (float)tile_size.y) * scale;

                  ImU32 base_fill = IM_COL32(0, 0, 0, 70);
                  if (is_selected_tile)
                    base_fill = edit_solid_mode ? IM_COL32(70, 140, 255, 45) : IM_COL32(80, 220, 120, 55);
                  if (is_solid_tile)
                    base_fill = IM_COL32(220, 70, 70, 95);
                  draw_list->AddRectFilled(cell_min.to_imvec2(), cell_max.to_imvec2(), base_fill);

                  // Border highlight for solid tiles even in tile mode
                  if (is_solid_tile)
                    draw_list->AddRect(cell_min.to_imvec2(), cell_max.to_imvec2(), IM_COL32(255, 120, 120, 220), 0.0f, 0, 2.0f);
                  else if (is_selected_tile)
                    draw_list->AddRect(cell_min.to_imvec2(), cell_max.to_imvec2(), IM_COL32(140, 255, 170, 180), 0.0f, 0, 1.5f);

                  ImGui::PushID((int)(y * 100000 + x));
                  ImGui::SetCursorScreenPos(cell_min.to_imvec2());
                  ImGui::InvisibleButton("##tile_cell", cell_btn_size.to_imvec2());

                  const bool hovered = ImGui::IsItemHovered();
                  const bool clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

                  auto apply_action = [&](bool add)
                  {
                    if (edit_solid_mode)
                    {
                      if (!is_selected_tile)
                        return; // only registered/selected tiles can be marked solid
                      if (add)
                        add_tile(solid_tiles, tid);
                      else
                        remove_tile(solid_tiles, tid);
                    }
                    else
                    {
                      if (add)
                      {
                        add_tile(selected_tiles, tid);
                      }
                      else
                      {
                        remove_tile(selected_tiles, tid);
                        remove_tile(solid_tiles, tid);
                      }
                    }
                  };

                  if (clicked)
                  {
                    const bool ctrl  = ImGui::GetIO().KeyCtrl;
                    const bool shift = ImGui::GetIO().KeyShift;

                    if (shift && !edit_solid_mode && !selected_tiles.empty())
                    {
                      const vec2u last = selected_tiles.back();
                      const int sx = std::min((int)last.x, x), ex = std::max((int)last.x, x);
                      const int sy = std::min((int)last.y, y), ey = std::max((int)last.y, y);
                      for (int ry = sy; ry <= ey; ++ry)
                        for (int rx = sx; rx <= ex; ++rx)
                          add_tile(selected_tiles, {(AG_uint)rx, (AG_uint)ry});
                      drag_action = 0;
                    }
                    else if (shift && edit_solid_mode && !solid_tiles.empty())
                    {
                      const vec2u last = solid_tiles.back();
                      const int sx = std::min((int)last.x, x), ex = std::max((int)last.x, x);
                      const int sy = std::min((int)last.y, y), ey = std::max((int)last.y, y);
                      for (int ry = sy; ry <= ey; ++ry)
                        for (int rx = sx; rx <= ex; ++rx)
                        {
                          vec2u rid{(AG_uint)rx, (AG_uint)ry};
                          if (contains_tile(selected_tiles, rid))
                            add_tile(solid_tiles, rid);
                        }
                      drag_action = 0;
                    }
                    else
                    {
                      const bool currently_on = edit_solid_mode ? is_solid_tile : is_selected_tile;
                      const bool make_on = ctrl ? !currently_on : true;
                      if (!ctrl && !edit_solid_mode)
                      {
                        selected_tiles.clear();
                        solid_tiles.clear();
                      }
                      if (!ctrl && edit_solid_mode)
                      {
                        // Start painting solid state for clicked registered tile
                        if (is_selected_tile)
                        {
                          drag_action = make_on ? 0 : 1;
                          apply_action(make_on);
                        }
                      }
                      else
                      {
                        drag_action = make_on ? 0 : 1;
                        apply_action(make_on);
                      }
                      if (!ctrl && edit_solid_mode && is_selected_tile)
                        add_tile(solid_tiles, tid);
                    }
                  }
                  else if (hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left) && drag_action != -1 && !ImGui::GetIO().KeyShift)
                  {
                    apply_action(drag_action == 0);
                  }

                  if (hovered)
                  {
                    draw_list->AddRect(cell_min.to_imvec2(), cell_max.to_imvec2(), IM_COL32(255, 255, 255, 200), 0.0f, 0, 2.0f);
                    ImGui::BeginTooltip();
                    ImGui::Text("Tile (%d, %d)", x, y);
                    ImGui::Text("Pixels: (%d, %d)  Size: %dx%d", x * tile_size.x, y * tile_size.y, tile_size.x, tile_size.y);
                    if (is_selected_tile)
                      ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.5f, 1.0f), "Registered");
                    if (is_solid_tile)
                      ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Solid / Collidable");
                    if (edit_solid_mode)
                      ImGui::TextDisabled("Drag to paint/unpaint solid on registered tiles");
                    else
                      ImGui::TextDisabled("Click: select, Ctrl: toggle, Shift: range, Drag: paint");
                    ImGui::EndTooltip();
                  }

                  ImGui::PopID();
                }
              }
            }

            ImGui::EndChild();
          }
        }
        ImGui::EndChild();

        ImGui::SameLine(0.0f, 10.0f);

        ImGui::BeginChild("TileRegister_Right", ImVec2(right_width, 0), true);
        {
          ImGui::Text("TileSet Settings");
          ImGui::Separator();

          ImGui::TextDisabled("Texture: %.0fx%.0f", texture_size.x, texture_size.y);
          ImGui::Spacing();

          vec2i new_size = tile_size;
          if (UI::draw_vec2("Tile Size", new_size))
          {
            new_size.x = std::max(1, new_size.x);
            new_size.y = std::max(1, new_size.y);
            if (new_size != tile_size)
            {
              tile_size = new_size;
              selected_tiles.clear();
              solid_tiles.clear();
            }
          }

          ImGui::Spacing();
          ImGui::Separator();
          ImGui::Text("Edit Mode");
          ImGui::TextWrapped(edit_solid_mode
                                 ? "Mark which registered tiles are solid/collidable."
                                 : "Select which atlas cells belong to this tileset.");

          if (ImGui::RadioButton("Register Tiles", !edit_solid_mode))
            edit_solid_mode = false;
          if (ImGui::RadioButton("Mark Solid Tiles", edit_solid_mode))
            edit_solid_mode = true;

          ImGui::Spacing();
          ImGui::Separator();

          const int solid_count = (int)std::count_if(solid_tiles.begin(), solid_tiles.end(), [&](const vec2u& id) {
            return contains_tile(selected_tiles, id);
          });
          ImGui::Text("Registered: %d", (int)selected_tiles.size());
          ImGui::Text("Solid: %d", solid_count);

          ImGui::Spacing();
          if (ImGui::Button("Select All Grid", ImVec2(-1, 0)) && tile_size.x > 0 && tile_size.y > 0)
          {
            selected_tiles.clear();
            const vec2i dim = vec2i(texture_size / vec2f((float)tile_size.x, (float)tile_size.y));
            for (int y = 0; y < dim.y; ++y)
              for (int x = 0; x < dim.x; ++x)
                selected_tiles.push_back({(AG_uint)x, (AG_uint)y});
          }
          if (ImGui::Button("Clear Registered", ImVec2(-1, 0)))
          {
            selected_tiles.clear();
            solid_tiles.clear();
          }
          if (ImGui::Button("Clear Solid", ImVec2(-1, 0)))
          {
            solid_tiles.clear();
          }
          if (ImGui::Button("Mark All Registered Solid", ImVec2(-1, 0)))
          {
            solid_tiles = selected_tiles;
          }

          ImGui::Spacing();
          ImGui::Separator();
          ImGui::Text("Legend");
          auto legend = [](const char* label, ImU32 col)
          {
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 14, p.y + 14), col, 2.0f);
            ImGui::Dummy(ImVec2(18, 14));
            ImGui::SameLine();
            ImGui::TextUnformatted(label);
          };
          legend("Registered tile", IM_COL32(80, 220, 120, 90));
          legend("Solid tile", IM_COL32(220, 70, 70, 120));
          legend("Unregistered", IM_COL32(0, 0, 0, 90));

          ImGui::Spacing();
          ImGui::TextDisabled("Tip: use 'Mark Solid Tiles' mode to paint collidable tiles. Only registered tiles can be solid.");

          const float footer_h = 40.0f;
          ImGui::SetCursorPosY(std::max(ImGui::GetCursorPosY(), ImGui::GetWindowHeight() - footer_h - ImGui::GetStyle().WindowPadding.y));
          const float button_width = ImGui::GetContentRegionAvail().x * 0.5f - 5.0f;

          const bool can_register = !selected_tiles.empty() && tile_size.x > 0 && tile_size.y > 0;
          ImGui::BeginDisabled(!can_register);
          if (ImGui::Button("Apply / Register", ImVec2(button_width, 34.0f)))
          {
            if (!entity.has_component<TileSet_Component>())
              entity.add_component<TileSet_Component>();

            auto& tile_set = entity.get_component<TileSet_Component>();
            tile_set.tile_definitions.clear();
            const vec2u size_u = {(AG_uint)std::max(1, tile_size.x), (AG_uint)std::max(1, tile_size.y)};

            for (const auto& ids : selected_tiles)
            {
              Tile_Defination def;
              def.texture_rect = {ids * size_u, size_u};
              def.is_solid     = contains_tile(solid_tiles, ids);
              tile_set.tile_definitions[ids] = def;
            }

            tile_set.tile_size          = tile_size;
            tile_set.is_tile_registered = !tile_set.tile_definitions.empty();
            tile_set.tile_changed       = true;
            Scene::get_active_scene()->set_save_required();

            show_register = false;
            selected_tiles.clear();
            solid_tiles.clear();
            drag_action = -1;
            ImGui::CloseCurrentPopup();
          }
          ImGui::EndDisabled();

          ImGui::SameLine(0.0f, 10.0f);
          if (ImGui::Button("Cancel", ImVec2(button_width, 34.0f)))
          {
            show_register = false;
            selected_tiles.clear();
            solid_tiles.clear();
            drag_action = -1;
            ImGui::CloseCurrentPopup();
          }
        }
        ImGui::EndChild();
      },
      []()
      {
        show_register = false;
        selected_tiles.clear();
        solid_tiles.clear();
        drag_action = -1;
        ImGui::CloseCurrentPopup();
      });
}

void UI::draw_autotiling_register(Entity entity)
{
  static bool        show_register        = false;
  static bool        show_set_name_exists = false;
  static std::string active_set;

  static char buffer[128]    = "";
  buffer[sizeof(buffer) - 1] = '\0';

  ImGui::InputTextWithHint("##NewSetName", "New Set Name", buffer, sizeof(buffer));
  ImGui::SameLine();
  if (ImGui::Button("Add") && buffer[0] != '\0')
  {
    auto&       comps = entity.get_component<AutoTiling_Component>();
    std::string name  = buffer;
    if (comps.auto_tiles.find(name) == comps.auto_tiles.end())
    {
      Auto_Tiles tiles;
      comps.next_id++;
      tiles.set_id           = comps.next_id;
      comps.auto_tiles[name] = tiles;
      buffer[0]              = '\0';
    }
    else
    {
      show_set_name_exists = true;
    }
  }
  {
    if (ImGui::BeginCombo("Current Set", active_set.empty() ? "None" : active_set.c_str()))
    {
      if (ImGui::Selectable("None", active_set.empty()))
      {
        active_set = "";
      }
      auto& comps = entity.get_component<AutoTiling_Component>();
      for (const auto& [name, _] : comps.auto_tiles)
      {
        bool is_selected = (active_set == name);
        if (ImGui::Selectable(name.c_str(), is_selected))
        {
          active_set = name;
        }
        if (is_selected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
  }

  static std::vector<vec2u>                                      selected_tiles;
  static std::unordered_map<vec2u, uint16_t, vec2_hash<AG_uint>> temp_bitmask;
  static vec2i                                                   tile_size = {32, 32};
  static vec2f                                                   last_mouse_pos;
  static vec2f                                                   current_mouse_pos;
  static bool                                                    is_dragging          = false;
  static std::string                                             current_set_name     = "";
  static std::string                                             set_name             = "";
  static bool                                                    creating_new_set     = false;
  static bool                                                    editing_existing_set = false;

  float total_width = ImGui::GetContentRegionAvail().x;
  float spacing     = 10.0f;
  float width       = (total_width - 5.0f) * 0.5f;
  if (ImGui::Button("Edit Set", ImVec2(width, 30.0f)))
  {
    if (!entity.has_component<Texture_Component>() ||
        !entity.get_component<Texture_Component>().texture ||
        !entity.has_component<TileSet_Component>() || !entity.has_component<AutoTiling_Component>())
      return;

    ImGui::OpenPopup("AutoTiling Register");
    show_register = true;
    selected_tiles.clear();

    if (entity.has_component<TileSet_Component>())
    {
      auto& tile_set = entity.get_component<TileSet_Component>();
      for (const auto& [id, def] : tile_set.tile_definitions)
      {
        selected_tiles.push_back(id);
      }
      if (tile_set.tile_size.x > 0 && tile_set.tile_size.y > 0)
      {
        tile_size = tile_set.tile_size;
      }
    }
    temp_bitmask.clear();
    if (entity.has_component<AutoTiling_Component>())
    {
      auto& comps = entity.get_component<AutoTiling_Component>();
      auto  it    = comps.auto_tiles.find(active_set);
      if (it != comps.auto_tiles.end())
      {
        Auto_Tiles& set = it->second;
        for (const auto& [key, value] : set.tile_bitmask)
        {
          temp_bitmask[value] = key;
        }
      }
    }
  }
  ImGui::SameLine(0, 10.0f);
  if (ImGui::Button("Delete Set", ImVec2(width, 30.0f)))
  {
    auto& comps = entity.get_component<AutoTiling_Component>();
    auto  it    = comps.auto_tiles.find(active_set);
    if (it != comps.auto_tiles.end())
    {
      comps.auto_tiles.erase(it);
      active_set = "";
    }
  }

  if (show_register)
  {
    custom_popup(
        "AutoTiling Register",
        "AutoTiling Register",
        [&]() mutable
        {
          auto&       texture      = entity.get_component<Texture_Component>().texture;
          vec2f       texture_size = texture->get_size();
          ImTextureID texture_id   = (ImTextureID)(intptr_t)texture->get_texture_id();

          float available_width = ImGui::GetContentRegionAvail().x;
          float left_width      = available_width * 0.65f - 5.0f;
          float right_width     = available_width * 0.35f - 5.0f;

          ImGui::BeginChild("TextureColumn", ImVec2(left_width, 0), true);
          {
            ImGui::Text("Texture Preview");
            ImGui::Separator();

            ImGui::BeginChild("TextureContainer",
                              ImVec2(0, 0),
                              true,
                              ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            {
              static vec2f view_size = ImGui::GetContentRegionAvail();
              static vec2f view_center;

              vec2f mouse_screen = ImGui::GetMousePos();
              vec2f child_pos    = ImGui::GetWindowPos();
              vec2f child_size   = ImGui::GetWindowSize();

              current_mouse_pos = mouse_screen - child_pos;

              vec2f container           = ImGui::GetContentRegionAvail();
              vec2f container_start_pos = ImGui::GetCursorScreenPos();
              bool  is_hovering_container =
                  ImGui::IsMouseHoveringRect(container_start_pos.to_imvec2(),
                                             (container_start_pos + container).to_imvec2());

              if (is_hovering_container && ImGui::IsMouseClicked(2))
              {
                is_dragging    = true;
                last_mouse_pos = current_mouse_pos;
              }
              if (is_dragging)
              {
                if (ImGui::IsMouseDown(2))
                {
                  vec2f start = Math::screen_to_world(last_mouse_pos,
                                                      Math::get_float_rect(view_size, view_center),
                                                      container);
                  vec2f end   = Math::screen_to_world(current_mouse_pos,
                                                    Math::get_float_rect(view_size, view_center),
                                                    container);
                  vec2f delta = start - end;

                  view_center += delta;
                  last_mouse_pos = current_mouse_pos;
                }
                else
                {
                  is_dragging = false;
                }
              }

              if (is_hovering_container && ImGui::GetIO().MouseWheel != 0)
              {
                float scale_factor = (ImGui::GetIO().MouseWheel > 0) ? 0.9f : 1.1f;
                vec2f world_before_zoom =
                    Math::screen_to_world(current_mouse_pos,
                                          Math::get_float_rect(view_size, view_center),
                                          container);
                view_size *= scale_factor;
                vec2f world_after_zoom =
                    Math::screen_to_world(current_mouse_pos,
                                          Math::get_float_rect(view_size, view_center),
                                          container);
                vec2f offset = world_before_zoom - world_after_zoom;
                view_center += offset;
              }
              vec2f world_pos  = vec2f(0, 0) - texture_size * 0.5f;
              vec2f screen_pos = Math::world_to_screen(world_pos,
                                                       Math::get_float_rect(view_size, view_center),
                                                       container);
              vec2f screen_size =
                  Math::world_size_to_screen_size(texture_size, view_size, container);
              ImGui::SetCursorPos(screen_pos.to_imvec2());
              ImGui::Image(texture_id, screen_size.to_imvec2());
              ImDrawList* draw_list = ImGui::GetWindowDrawList();

              vec2f window_pos = ImGui::GetWindowPos();
              screen_pos += window_pos;
              vec2i total_dim = texture_size / tile_size;
              for (int x = 0; x <= total_dim.x; x++)
              {  // Vertical
                float px = screen_pos.x + x * tile_size.x * (screen_size.x / texture_size.x);
                draw_list->AddLine(ImVec2(px, screen_pos.y),
                                   ImVec2(px, screen_pos.y + screen_size.y),
                                   IM_COL32(255, 255, 255, 100),
                                   2.0f);
              }

              for (int y = 0; y <= total_dim.y; y++)
              {  // Horizontal
                float py = screen_pos.y + y * tile_size.y * (screen_size.y / texture_size.y);
                draw_list->AddLine(ImVec2(screen_pos.x, py),
                                   ImVec2(screen_pos.x + screen_size.x, py),
                                   IM_COL32(255, 255, 255, 100),
                                   2.0f);
              }

              vec2f button_size = Math::world_size_to_screen_size(tile_size, view_size, container);
              for (int x = 0; x < total_dim.x; x++)
              {
                for (int y = 0; y < total_dim.y; y++)
                {
                  vec2u tile_id = {(AG_uint)x, (AG_uint)y};

                  bool is_selected =
                      std::find(selected_tiles.begin(), selected_tiles.end(), tile_id) !=
                      selected_tiles.end();

                  uint16_t mask = 0;
                  auto     it   = temp_bitmask.find(tile_id);
                  if (it != temp_bitmask.end())
                    mask = it->second;

                  vec2f scale    = screen_size / texture_size;
                  vec2f cell_min = screen_pos + vec2f(x * tile_size.x, y * tile_size.y) * scale;
                  vec2f cell_max =
                      screen_pos + vec2f((x + 1) * tile_size.x, (y + 1) * tile_size.y) * scale;

                  ImU32 selected_color =
                      is_selected ? IM_COL32(200, 200, 200, 50) : IM_COL32(0, 0, 0, 150);

                  draw_list->AddRectFilled(cell_min.to_imvec2(),
                                           cell_max.to_imvec2(),
                                           selected_color);

                  ImGui::SetCursorScreenPos(cell_min.to_imvec2());

                  vec2f sub_size = (cell_max - cell_min) / 3.0f;
                  ImGui::PushID(tile_id.x);
                  ImGui::PushID(tile_id.y);
                  for (int sy = 0; sy < 3; sy++)
                  {
                    for (int sx = 0; sx < 3; sx++)
                    {
                      uint16_t bit = bit_lookup[sy][sx];
                      if (bit == 0)
                        continue;

                      vec2f sub_min = cell_min + vec2f(sx, sy) * sub_size;
                      vec2f sub_max = sub_min + sub_size;

                      bool selected = (mask & bit) != 0;

                      ImU32 color = selected ? IM_COL32(255, 0, 0, 180) : IM_COL32(50, 50, 50, 0);

                      draw_list->AddRectFilled(sub_min.to_imvec2(), sub_max.to_imvec2(), color);

                      ImGui::PushID((sy * 3 + sx));
                      ImGui::SetCursorScreenPos(sub_min.to_imvec2());

                      if (ImGui::InvisibleButton("##autobit", sub_size.to_imvec2()))
                      {
                        mask ^= bit;
                      }

                      ImGui::PopID();
                    }
                  }

                  if (mask != 0)
                    temp_bitmask[tile_id] = mask;
                  else
                    temp_bitmask.erase(tile_id);

                  ImGui::PopID();
                  ImGui::PopID();
                }
              }

              ImGui::EndChild();
            }
            ImGui::EndChild();
          }
          ImGui::SameLine(0.0f, 10.0f);
          ImGui::BeginChild("Control", ImVec2(right_width, 0), true);
          {
            float available_height = ImGui::GetContentRegionAvail().y;
            ImGui::Text("Tile Register Controls");
            ImGui::Separator();
            ImGui::Text("Texture Info:");
            ImGui::Text("Size: %.0fx%.0f", texture_size.x, texture_size.y);
            ImGui::Separator();

            if (UI::draw_vec2("Size", tile_size))
            {
              selected_tiles.clear();
            }

            ImGui::Text("Set Name: %s", set_name.c_str());

            ImGui::Dummy(ImVec2(0.0f, 2.0f));

            float button_width  = ImGui::GetContentRegionAvail().x * 0.5f - 5.0f;
            float button_height = 35.0f;
            ImGui::SetCursorPosY(available_height - button_height);

            if (ImGui::Button("Register Tiles", ImVec2(button_width, button_height)) &&
                !selected_tiles.empty())
            {
              if (!active_set.empty())
              {
                auto& comps = entity.get_component<AutoTiling_Component>();
                auto  it    = comps.auto_tiles.find(active_set);
                if (it != comps.auto_tiles.end())
                {
                  Auto_Tiles& set = it->second;
                  set.tile_bitmask.clear();
                  for (const auto& [key, value] : temp_bitmask)
                  {
                    set.tile_bitmask[value] = key;
                  }
                }
                show_register = false;
                selected_tiles.clear();
                temp_bitmask.clear();
                ImGui::CloseCurrentPopup();
              }
            }
            ImGui::SameLine(0.0f, 10.0f);
            if (ImGui::Button("Cancel", ImVec2(button_width, button_height)))
            {
              show_register = false;

              selected_tiles.clear();
              current_set_name     = "";
              creating_new_set     = false;
              editing_existing_set = false;
              temp_bitmask.clear();
              ImGui::CloseCurrentPopup();
            }
          }
          ImGui::EndChild();
        },
        []()
        {
          show_register = false;
          selected_tiles.clear();
          current_set_name     = "";
          creating_new_set     = false;
          editing_existing_set = false;
          temp_bitmask.clear();
          ImGui::CloseCurrentPopup();
        });
  }

  if (show_set_name_exists)
  {
    PopUpModel model;
    model.id          = "Set Name Exists";
    model.name        = "Set Name Exists";
    model.auto_resize = true;

    model.draw_content = [&]()
    {
      ImGui::Text("A set with that name already exists!");
      if (ImGui::Button("OK"))
      {
        show_set_name_exists = false;
        ImGui::CloseCurrentPopup();
      }
    };

    model.on_close = [&]()
    {
      show_set_name_exists = false;
      ImGui::CloseCurrentPopup();
    };

    PopUp::draw(model);
  }
}

bool UI::draw_tilemap_selector(Entity entity, vec2u& id, std::string& set_name, bool& use_autotile)
{
  static bool               window_open = true;
  static std::vector<vec2u> registered_tiles;
  static vec2f              current_mouse_pos;
  static vec2f              last_mouse_pos;
  static bool               is_dragging  = false;
  static vec2f              tile_size    = {32, 32};
  static vec2f              view_size    = {800.0f, 600.0f};
  static vec2f              view_center  = {0.0f, 0.0f};
  static vec2f              window_size  = {0.0f, 0.0f};

  if (!entity.has_component<Tile_Component>() || !entity.has_component<Texture_Component>() ||
      !entity.get_component<Texture_Component>().texture ||
      !entity.has_component<TileSet_Component>())
  {
    return false;
  }

  auto& tile_set = entity.get_component<TileSet_Component>();
  if (!tile_set.is_tile_registered)
    return false;

  if (tile_set.tile_changed)
  {
    registered_tiles.clear();
    for (const auto& [tid, def] : tile_set.tile_definitions)
      registered_tiles.push_back(tid);
    tile_set.tile_changed = false;
    if (tile_set.tile_size.x > 0 && tile_set.tile_size.y > 0)
      tile_size = vec2f((float)tile_set.tile_size.x, (float)tile_set.tile_size.y);
  }

  bool selection_made = false;
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                           ImGuiWindowFlags_NoCollapse;

  if (!ImGui::Begin("TileSet Selector", &window_open, flags))
  {
    ImGui::End();
    return false;
  }

  auto&       texture      = entity.get_component<Texture_Component>().texture;
  const vec2f texture_size = texture->get_size();
  ImTextureID texture_id   = (ImTextureID)(intptr_t)texture->get_texture_id();

  // Top info / controls
  {
    const auto it = tile_set.tile_definitions.find(id);
    const bool selected_valid = (it != tile_set.tile_definitions.end());
    ImGui::Text("Selected Tile: %s", selected_valid ? (std::to_string(id.x) + ", " + std::to_string(id.y)).c_str() : "None");
    ImGui::SameLine();
    ImGui::TextDisabled("| Registered: %zu", tile_set.tile_definitions.size());
    if (selected_valid)
    {
      ImGui::SameLine();
      if (it->second.is_solid)
        ImGui::TextColored(ImVec4(1.0f, 0.45f, 0.45f, 1.0f), "| Solid");
      else
        ImGui::TextColored(ImVec4(0.55f, 0.85f, 1.0f, 1.0f), "| Non-solid");
    }
    ImGui::TextDisabled("Click a registered tile to paint with it. MMB pans, wheel zooms.");
  }

  if (entity.has_component<AutoTiling_Component>())
  {
    ImGui::Separator();
    auto& comps = entity.get_component<AutoTiling_Component>();
    if (ImGui::BeginCombo("AutoTile Set", set_name.empty() ? "None" : set_name.c_str()))
    {
      if (ImGui::Selectable("None", set_name.empty()))
        set_name.clear();
      for (const auto& [name, _] : comps.auto_tiles)
      {
        bool is_selected = (set_name == name);
        if (ImGui::Selectable(name.c_str(), is_selected))
          set_name = name;
        if (is_selected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
    ImGui::SameLine();
    UI::draw_bool("Use Auto Tile", use_autotile);
  }

  ImGui::Separator();

  ImGui::BeginChild("TextureContainer",
                    ImVec2(0, 0),
                    true,
                    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
  {
    const vec2f current_size = ImGui::GetWindowSize();
    if (current_size.x > 0.0f && current_size.y > 0.0f && window_size.x > 0.0f && window_size.y > 0.0f)
    {
      if (current_size != window_size)
      {
        vec2f scale = current_size / window_size;
        view_size *= scale;
        window_size = current_size;
      }
    }
    else if (current_size.x > 0.0f && current_size.y > 0.0f)
    {
      window_size = current_size;
      if (view_size.x <= 1.0f || view_size.y <= 1.0f)
        view_size = current_size;
    }

    const vec2f mouse_screen = ImGui::GetMousePos();
    const vec2f child_pos    = ImGui::GetWindowPos();
    current_mouse_pos        = mouse_screen - child_pos;

    const vec2f container = ImGui::GetContentRegionAvail();
    const vec2f container_start_pos = ImGui::GetCursorScreenPos();
    const bool hovering = ImGui::IsMouseHoveringRect(container_start_pos.to_imvec2(),
                                                     (container_start_pos + container).to_imvec2());

    if (hovering && ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
    {
      is_dragging    = true;
      last_mouse_pos = current_mouse_pos;
    }
    if (is_dragging)
    {
      if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
      {
        vec2f start = Math::screen_to_world(last_mouse_pos,
                                            Math::get_float_rect(view_size, view_center),
                                            container);
        vec2f end   = Math::screen_to_world(current_mouse_pos,
                                          Math::get_float_rect(view_size, view_center),
                                          container);
        view_center += (start - end);
        last_mouse_pos = current_mouse_pos;
      }
      else
      {
        is_dragging = false;
      }
    }

    if (hovering && ImGui::GetIO().MouseWheel != 0.0f)
    {
      const float scale_factor = (ImGui::GetIO().MouseWheel > 0.0f) ? 0.9f : 1.1f;
      vec2f world_before = Math::screen_to_world(current_mouse_pos,
                                                 Math::get_float_rect(view_size, view_center),
                                                 container);
      view_size *= scale_factor;
      view_size.x = std::clamp(view_size.x, 64.0f, 20000.0f);
      view_size.y = std::clamp(view_size.y, 64.0f, 20000.0f);
      vec2f world_after = Math::screen_to_world(current_mouse_pos,
                                                Math::get_float_rect(view_size, view_center),
                                                container);
      view_center += (world_before - world_after);
    }

    const vec2f world_pos   = vec2f(0, 0) - texture_size * 0.5f;
    vec2f screen_pos        = Math::world_to_screen(world_pos,
                                             Math::get_float_rect(view_size, view_center),
                                             container);
    const vec2f screen_size = Math::world_size_to_screen_size(texture_size, view_size, container);
    ImGui::SetCursorPos(screen_pos.to_imvec2());
    ImGui::Image(texture_id, screen_size.to_imvec2());

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const vec2f window_pos = ImGui::GetWindowPos();
    screen_pos += window_pos;

    const vec2i total_dim = (tile_size.x > 0.0f && tile_size.y > 0.0f)
                                ? vec2i(texture_size / tile_size)
                                : vec2i(0, 0);

    // Grid
    for (int x = 0; x <= total_dim.x; ++x)
    {
      float px = screen_pos.x + x * tile_size.x * (screen_size.x / texture_size.x);
      draw_list->AddLine(ImVec2(px, screen_pos.y), ImVec2(px, screen_pos.y + screen_size.y), IM_COL32(255, 255, 255, 35), 1.0f);
    }
    for (int y = 0; y <= total_dim.y; ++y)
    {
      float py = screen_pos.y + y * tile_size.y * (screen_size.y / texture_size.y);
      draw_list->AddLine(ImVec2(screen_pos.x, py), ImVec2(screen_pos.x + screen_size.x, py), IM_COL32(255, 255, 255, 35), 1.0f);
    }

    const vec2f button_size = Math::world_size_to_screen_size(tile_size, view_size, container);
    const vec2f scale       = screen_size / texture_size;

    for (int y = 0; y < total_dim.y; ++y)
    {
      for (int x = 0; x < total_dim.x; ++x)
      {
        const vec2u tid = {(AG_uint)x, (AG_uint)y};
        const auto  def_it = tile_set.tile_definitions.find(tid);
        const bool  is_registered = (def_it != tile_set.tile_definitions.end());
        const bool  is_current    = is_registered && (tid == id);

        const vec2f cell_min = screen_pos + vec2f(x * tile_size.x, y * tile_size.y) * scale;
        const vec2f cell_max = screen_pos + vec2f((x + 1) * tile_size.x, (y + 1) * tile_size.y) * scale;

        ImU32 fill = IM_COL32(0, 0, 0, 120);
        if (is_registered)
        {
          const bool solid = def_it->second.is_solid;
          fill = is_current ? IM_COL32(255, 170, 50, 120)
                            : (solid ? IM_COL32(200, 70, 70, 70) : IM_COL32(80, 160, 255, 45));
        }
        draw_list->AddRectFilled(cell_min.to_imvec2(), cell_max.to_imvec2(), fill);

        if (is_current)
          draw_list->AddRect(cell_min.to_imvec2(), cell_max.to_imvec2(), IM_COL32(255, 210, 90, 255), 0.0f, 0, 2.5f);
        else if (is_registered)
          draw_list->AddRect(cell_min.to_imvec2(), cell_max.to_imvec2(),
                             def_it->second.is_solid ? IM_COL32(255, 120, 120, 170) : IM_COL32(140, 200, 255, 120),
                             0.0f, 0, 1.2f);

        if (is_registered)
        {
          ImGui::PushID((int)(y * 100000 + x));
          ImGui::SetCursorScreenPos(cell_min.to_imvec2());
          if (ImGui::InvisibleButton("##tile_sel", button_size.to_imvec2()))
          {
            id = tid;
            selection_made = true;
          }
          if (ImGui::IsItemHovered())
          {
            draw_list->AddRect(cell_min.to_imvec2(), cell_max.to_imvec2(), IM_COL32(255, 255, 255, 220), 0.0f, 0, 2.0f);
            ImGui::BeginTooltip();
            ImGui::Text("Tile (%u, %u)", tid.x, tid.y);
            ImGui::Text("Size: %.0fx%.0f", tile_size.x, tile_size.y);
            ImGui::TextColored(def_it->second.is_solid ? ImVec4(1, 0.5f, 0.5f, 1) : ImVec4(0.6f, 0.9f, 1, 1),
                               "%s", def_it->second.is_solid ? "Solid / Collidable" : "Non-solid");
            ImGui::TextDisabled("Click to select for painting");
            ImGui::EndTooltip();
          }
          ImGui::PopID();
        }
      }
    }
  }
  ImGui::EndChild();

  ImGui::End();
  return selection_made;
}

bool UI::texture_selector(Entity entity, uint_rect& texture_rect)
{
  static Entity last_entity;
  static vec2f  last_mouse_pos;
  static vec2f  drag_start_pixel;
  static vec2f  drag_end_pixel;
  static bool   is_panning     = false;
  static bool   is_selecting   = false;
  static vec2f  view_size      = {512.0f, 512.0f};
  static vec2f  view_center    = {0.0f, 0.0f};
  static bool   show_grid      = false;
  static bool   snap_to_grid   = false;
  static vec2i  grid_step      = {16, 16};
  static bool   initialized    = false;
  static bool   view_init_for_canvas = false;

  bool selected = false;

  if (!entity.has_component<TextureRect_Component>() ||
      !entity.has_component<Texture_Component>() ||
      !entity.get_component<Texture_Component>().texture)
    return false;

  auto& texture = entity.get_component<Texture_Component>().texture;
  auto& current_rect = entity.get_component<TextureRect_Component>().rect;
  const vec2f texture_size = texture->get_size();
  if (texture_size.x <= 0.0f || texture_size.y <= 0.0f)
    return false;

  if (!initialized || !(last_entity == entity))
  {
    last_entity = entity;
    drag_start_pixel = vec2f((float)current_rect.position.x, (float)current_rect.position.y);
    drag_end_pixel   = vec2f((float)(current_rect.position.x + current_rect.size.x - 1),
                             (float)(current_rect.position.y + current_rect.size.y - 1));
    view_center = {0.0f, 0.0f};
    view_size = {0.0f, 0.0f};
    view_init_for_canvas = false;
    initialized = true;
    is_selecting = false;
    is_panning = false;
  }

  ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                           ImGuiWindowFlags_NoCollapse;

  bool window_open = true;
  if (!ImGui::Begin("Texture Rect Selector", &window_open, flags))
  {
    ImGui::End();
    if (!window_open)
      s_show_panels.texture_selector = false;
    return false;
  }

  ImTextureID texture_id = (ImTextureID)(intptr_t)texture->get_texture_id();

  // Controls/header
  {
    ImGui::Text("Texture: %.0fx%.0f", texture_size.x, texture_size.y);
    ImGui::SameLine();
    ImGui::TextDisabled("| Drag: select rect  MMB: pan  Wheel: zoom  Double-click: full texture");

    ImGui::Text("Current: Pos(%u, %u) Size(%u, %u)", current_rect.position.x, current_rect.position.y,
                current_rect.size.x, current_rect.size.y);
    ImGui::SameLine();
    {
      vec2f sp = drag_start_pixel, ep = drag_end_pixel;
      int x0 = (int)std::floor(std::min(sp.x, ep.x));
      int y0 = (int)std::floor(std::min(sp.y, ep.y));
      int x1 = (int)std::floor(std::max(sp.x, ep.x));
      int y1 = (int)std::floor(std::max(sp.y, ep.y));
      ImGui::TextDisabled("| Selection: Pos(%d,%d) Size(%d,%d)", x0, y0, std::max(0, x1 - x0 + 1), std::max(0, y1 - y0 + 1));
    }

    if (ImGui::Button("Full Texture"))
    {
      texture_rect.position = {0, 0};
      texture_rect.size = {(AG_uint)texture_size.x, (AG_uint)texture_size.y};
      drag_start_pixel = {0.0f, 0.0f};
      drag_end_pixel = {texture_size.x - 1.0f, texture_size.y - 1.0f};
      selected = true;
      Scene::get_active_scene()->set_save_required();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset to Current"))
    {
      drag_start_pixel = vec2f((float)current_rect.position.x, (float)current_rect.position.y);
      drag_end_pixel = vec2f((float)(current_rect.position.x + current_rect.size.x - 1),
                             (float)(current_rect.position.y + current_rect.size.y - 1));
    }
    ImGui::SameLine();
    ImGui::Checkbox("Grid", &show_grid);
    ImGui::SameLine();
    ImGui::BeginDisabled(!show_grid);
    ImGui::Checkbox("Snap", &snap_to_grid);
    ImGui::EndDisabled();
    if (show_grid)
    {
      ImGui::SameLine();
      ImGui::SetNextItemWidth(140.0f);
      ImGui::DragInt2("##gridstep", &grid_step.x, 1.0f, 1, 4096, "Grid %d");
      grid_step.x = std::max(1, grid_step.x);
      grid_step.y = std::max(1, grid_step.y);
    }
  }
  ImGui::Separator();

  ImGui::BeginChild("TextureSelector_Canvas", ImVec2(0, 0), true,
                    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
  {
    const vec2f container = ImGui::GetContentRegionAvail();
    if (container.x < 1.0f || container.y < 1.0f)
    {
      ImGui::EndChild();
      ImGui::End();
      if (!window_open) s_show_panels.texture_selector = false;
      return selected;
    }

    const vec2f container_origin = ImGui::GetCursorScreenPos();
    const vec2f mouse_screen = ImGui::GetMousePos();
    const vec2f mouse_in_container = mouse_screen - container_origin;
    const bool hovering = ImGui::IsMouseHoveringRect(container_origin.to_imvec2(),
                                                     (container_origin + container).to_imvec2());

    auto set_view_to_fit = [&](bool keep_center)
    {
      // Keep view aspect matched to container to avoid stretching with axis-independent mapping.
      const float container_aspect = container.x / std::max(1.0f, container.y);
      const float margin = 1.15f;
      vec2f fit = texture_size * margin;
      // Expand view to match container aspect
      if ((fit.x / fit.y) < container_aspect)
        fit.x = fit.y * container_aspect;
      else
        fit.y = fit.x / container_aspect;
      view_size = fit;
      if (!keep_center)
        view_center = {0.0f, 0.0f};
      view_init_for_canvas = true;
    };

    if (!view_init_for_canvas || view_size.x <= 1.0f || view_size.y <= 1.0f)
      set_view_to_fit(false);
    else
    {
      // Ensure view aspect matches container aspect (prevents texture stretching on resize/layout changes)
      const float container_aspect = container.x / std::max(1.0f, container.y);
      const float view_aspect = view_size.x / std::max(1.0f, view_size.y);
      if (std::abs(view_aspect - container_aspect) > 0.001f)
      {
        // Adjust width to match container aspect while preserving vertical zoom level
        view_size.x = view_size.y * container_aspect;
      }
    }

    const vec2f image_pos = vec2f(0, 0) - texture_size * 0.5f;
    const vec2f padding = {16.0f, 16.0f};

    auto snap_pixel = [&](vec2f p) -> vec2f {
      p.x = std::clamp(p.x, 0.0f, texture_size.x - 1.0f);
      p.y = std::clamp(p.y, 0.0f, texture_size.y - 1.0f);
      if (show_grid && snap_to_grid)
      {
        p.x = std::floor(p.x / (float)grid_step.x) * (float)grid_step.x;
        p.y = std::floor(p.y / (float)grid_step.y) * (float)grid_step.y;
      }
      else
      {
        p.x = std::floor(p.x);
        p.y = std::floor(p.y);
      }
      p.x = std::clamp(p.x, 0.0f, texture_size.x - 1.0f);
      p.y = std::clamp(p.y, 0.0f, texture_size.y - 1.0f);
      return p;
    };

    // Pan (MMB)
    if (hovering && ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
    {
      is_panning = true;
      last_mouse_pos = mouse_in_container;
    }
    if (is_panning)
    {
      if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
      {
        vec2f start = Math::screen_to_world(last_mouse_pos, Math::get_float_rect(view_size, view_center), container);
        vec2f end = Math::screen_to_world(mouse_in_container, Math::get_float_rect(view_size, view_center), container);
        view_center += (start - end);
        last_mouse_pos = mouse_in_container;
      }
      else
      {
        is_panning = false;
      }
    }

    // Zoom (wheel)
    if (hovering && ImGui::GetIO().MouseWheel != 0.0f)
    {
      const float scale_factor = (ImGui::GetIO().MouseWheel > 0.0f) ? 0.9f : 1.1f;
      vec2f world_before = Math::screen_to_world(mouse_in_container, Math::get_float_rect(view_size, view_center), container);
      view_size *= scale_factor;
      view_size.x = std::clamp(view_size.x, 8.0f, texture_size.x * 50.0f);
      view_size.y = std::clamp(view_size.y, 8.0f, texture_size.y * 50.0f);
      // Re-enforce container aspect after zoom
      const float container_aspect = container.x / std::max(1.0f, container.y);
      view_size.x = view_size.y * container_aspect;
      vec2f world_after = Math::screen_to_world(mouse_in_container, Math::get_float_rect(view_size, view_center), container);
      view_center += (world_before - world_after);
    }

    // Clamp view center to keep texture reachable
    {
      const vec2f half_view = view_size * 0.5f;
      const vec2f min_c = image_pos - padding + half_view;
      const vec2f max_c = image_pos + texture_size + padding - half_view;
      if (view_size.x < texture_size.x + padding.x * 2.0f)
        view_center.x = std::clamp(view_center.x, min_c.x, max_c.x);
      else
        view_center.x = 0.0f;
      if (view_size.y < texture_size.y + padding.y * 2.0f)
        view_center.y = std::clamp(view_center.y, min_c.y, max_c.y);
      else
        view_center.y = 0.0f;
    }

    const float_rect view_rect = Math::get_float_rect(view_size, view_center);
    const vec2f screen_pos_local = Math::world_to_screen(image_pos, view_rect, container);
    const vec2f screen_size = Math::world_size_to_screen_size(texture_size, view_size, container);
    const vec2f image_screen_pos = container_origin + screen_pos_local;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Background behind texture
    const vec2f pad_px = Math::world_size_to_screen_size(padding, view_size, container);
    draw_list->AddRectFilled((image_screen_pos - pad_px).to_imvec2(),
                             (image_screen_pos + screen_size + pad_px).to_imvec2(),
                             IM_COL32(28, 28, 30, 255), 3.0f);

    // Draw texture via draw list for precise positioning
    draw_list->AddImage(texture_id, image_screen_pos.to_imvec2(), (image_screen_pos + screen_size).to_imvec2());

    // Invisible button over image for interaction
    ImGui::SetCursorScreenPos(image_screen_pos.to_imvec2());
    ImGui::InvisibleButton("##texture_canvas_img", screen_size.to_imvec2(),
                           ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    const bool image_hovered = ImGui::IsItemHovered();
    const bool image_active = ImGui::IsItemActive();

    // Pixel under mouse (in texture space)
    vec2f hover_pixel(-1.0f, -1.0f);
    if (hovering)
    {
      vec2f world_pos = Math::screen_to_world(mouse_in_container, view_rect, container);
      vec2f local = world_pos - image_pos;
      if (local.x >= 0.0f && local.y >= 0.0f && local.x < texture_size.x && local.y < texture_size.y)
        hover_pixel = vec2f(std::floor(local.x), std::floor(local.y));
    }

    // Selection interactions on image
    if (image_hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
      texture_rect.position = {0, 0};
      texture_rect.size = {(AG_uint)texture_size.x, (AG_uint)texture_size.y};
      drag_start_pixel = {0.0f, 0.0f};
      drag_end_pixel = {texture_size.x - 1.0f, texture_size.y - 1.0f};
      selected = true;
      is_selecting = false;
      Scene::get_active_scene()->set_save_required();
    }
    else if (image_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
      vec2f world_pos = Math::screen_to_world(mouse_in_container, view_rect, container);
      vec2f local = world_pos - image_pos;
      drag_start_pixel = snap_pixel(local);
      drag_end_pixel = drag_start_pixel;
      is_selecting = true;
    }

    if (is_selecting)
    {
      if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
      {
        vec2f world_pos = Math::screen_to_world(mouse_in_container, view_rect, container);
        vec2f local = world_pos - image_pos;
        local.x = std::clamp(local.x, 0.0f, texture_size.x - 1.0f);
        local.y = std::clamp(local.y, 0.0f, texture_size.y - 1.0f);
        if (show_grid && snap_to_grid)
        {
          local = snap_pixel(local);
          // expand to include snapped cell for intuitive grid selection
          local.x = std::min(local.x + (float)grid_step.x - 1.0f, texture_size.x - 1.0f);
          local.y = std::min(local.y + (float)grid_step.y - 1.0f, texture_size.y - 1.0f);
        }
        else
        {
          local = vec2f(std::floor(local.x), std::floor(local.y));
        }
        drag_end_pixel = local;
      }
      else
      {
        int x0 = (int)std::floor(std::min(drag_start_pixel.x, drag_end_pixel.x));
        int y0 = (int)std::floor(std::min(drag_start_pixel.y, drag_end_pixel.y));
        int x1 = (int)std::floor(std::max(drag_start_pixel.x, drag_end_pixel.x));
        int y1 = (int)std::floor(std::max(drag_start_pixel.y, drag_end_pixel.y));
        x0 = std::clamp(x0, 0, (int)texture_size.x - 1);
        y0 = std::clamp(y0, 0, (int)texture_size.y - 1);
        x1 = std::clamp(x1, 0, (int)texture_size.x - 1);
        y1 = std::clamp(y1, 0, (int)texture_size.y - 1);
        texture_rect.position = {(AG_uint)x0, (AG_uint)y0};
        texture_rect.size = {(AG_uint)std::max(1, x1 - x0 + 1), (AG_uint)std::max(1, y1 - y0 + 1)};
        selected = true;
        is_selecting = false;
        Scene::get_active_scene()->set_save_required();
      }
    }

    // Grid overlay
    if (show_grid && grid_step.x > 0 && grid_step.y > 0)
    {
      const vec2f scale = screen_size / texture_size;
      const int gx = std::max(1, grid_step.x);
      const int gy = std::max(1, grid_step.y);
      // clip to image bounds
      draw_list->PushClipRect(image_screen_pos.to_imvec2(), (image_screen_pos + screen_size).to_imvec2(), true);
      for (int x = 0; x <= (int)texture_size.x; x += gx)
      {
        float px = image_screen_pos.x + x * scale.x;
        draw_list->AddLine(ImVec2(px, image_screen_pos.y), ImVec2(px, image_screen_pos.y + screen_size.y), IM_COL32(255, 255, 255, 35), 1.0f);
      }
      for (int y = 0; y <= (int)texture_size.y; y += gy)
      {
        float py = image_screen_pos.y + y * scale.y;
        draw_list->AddLine(ImVec2(image_screen_pos.x, py), ImVec2(image_screen_pos.x + screen_size.x, py), IM_COL32(255, 255, 255, 35), 1.0f);
      }
      draw_list->PopClipRect();
    }

    // Selection/current rect drawing helpers
    auto to_screen_rect = [&](const vec2f& tex_p0, const vec2f& tex_p1, ImVec2& s0, ImVec2& s1)
    {
      vec2f a = container_origin + Math::world_to_screen(tex_p0 + image_pos, view_rect, container);
      vec2f b = container_origin + Math::world_to_screen(tex_p1 + image_pos, view_rect, container);
      s0 = ImVec2(std::min(a.x, b.x), std::min(a.y, b.y));
      s1 = ImVec2(std::max(a.x, b.x), std::max(a.y, b.y));
    };

    // Current applied rect outline (subtle)
    if (current_rect.size.x > 0 && current_rect.size.y > 0)
    {
      ImVec2 cs0, cs1;
      to_screen_rect(vec2f((float)current_rect.position.x, (float)current_rect.position.y),
                     vec2f((float)(current_rect.position.x + current_rect.size.x),
                           (float)(current_rect.position.y + current_rect.size.y)),
                     cs0, cs1);
      draw_list->AddRect(cs0, cs1, IM_COL32(90, 180, 255, 170), 0.0f, 0, 1.5f);
    }

    // Active selection visualization
    {
      vec2f sp = drag_start_pixel, ep = drag_end_pixel;
      vec2f p0{std::min(sp.x, ep.x), std::min(sp.y, ep.y)};
      vec2f p1{std::max(sp.x, ep.x) + 1.0f, std::max(sp.y, ep.y) + 1.0f};

      ImVec2 s0, s1;
      to_screen_rect(p0, p1, s0, s1);

      // Dim outside selection while dragging/selecting
      if (is_selecting)
      {
        draw_list->PushClipRect(image_screen_pos.to_imvec2(), (image_screen_pos + screen_size).to_imvec2(), true);
        // top
        draw_list->AddRectFilled(image_screen_pos.to_imvec2(), ImVec2(image_screen_pos.x + screen_size.x, s0.y), IM_COL32(0, 0, 0, 80));
        // bottom
        draw_list->AddRectFilled(ImVec2(image_screen_pos.x, s1.y), (image_screen_pos + screen_size).to_imvec2(), IM_COL32(0, 0, 0, 80));
        // left
        draw_list->AddRectFilled(ImVec2(image_screen_pos.x, s0.y), ImVec2(s0.x, s1.y), IM_COL32(0, 0, 0, 80));
        // right
        draw_list->AddRectFilled(ImVec2(s1.x, s0.y), ImVec2(image_screen_pos.x + screen_size.x, s1.y), IM_COL32(0, 0, 0, 80));
        draw_list->PopClipRect();
      }

      // Highlight selected region using texture UVs
      ImVec2 uv0(p0.x / texture_size.x, p0.y / texture_size.y);
      ImVec2 uv1(p1.x / texture_size.x, p1.y / texture_size.y);
      draw_list->AddImage(texture_id, s0, s1, uv0, uv1);
      draw_list->AddRect(s0, s1, IM_COL32(255, 220, 80, 240), 0.0f, 0, 2.0f);

      // Corner markers
      const float m = 6.0f;
      draw_list->AddLine(s0, ImVec2(s0.x + m, s0.y), IM_COL32(255, 255, 255, 255), 2.0f);
      draw_list->AddLine(s0, ImVec2(s0.x, s0.y + m), IM_COL32(255, 255, 255, 255), 2.0f);
      draw_list->AddLine(s1, ImVec2(s1.x - m, s1.y), IM_COL32(255, 255, 255, 255), 2.0f);
      draw_list->AddLine(s1, ImVec2(s1.x, s1.y - m), IM_COL32(255, 255, 255, 255), 2.0f);
    }

    // Hover readout
    if (hover_pixel.x >= 0.0f && hover_pixel.y >= 0.0f)
    {
      ImGui::BeginTooltip();
      ImGui::Text("Pixel: (%d, %d)", (int)hover_pixel.x, (int)hover_pixel.y);
      ImGui::EndTooltip();
    }
  }
  ImGui::EndChild();

  ImGui::End();

  if (!window_open)
    s_show_panels.texture_selector = false;

  return selected;
}

/*void UI::draw_script_selector(Entity entity)
{
    if (!entity.has_component<Script_Component>())
        return;
    auto& props = entity.get_component<Script_Component>();

    if (!props.path.empty())
    {
        ImGui::Text("Script:%s", props.path.c_str());

    }
    ImGui::Dummy(ImVec2(0, 10));
    float button_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x)
/ 2.0f; if (ImGui::Button("Create New", ImVec2(button_width, 0)))
    {
        s_show_panels.create_new_script = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Add Existing", ImVec2(button_width, 0)))
    {
        auto full_path = FileDialogs::open_file("Lua Scripts(*.lua)\0 * .lua\0All Files(*.*)\0 *
.*\0"); if (!full_path.empty())
        {
            EditorLayer::get().create_new_scene(full_path);
        }
    }

    ImGui::Dummy(ImVec2(0, 5));

    create_new_script(entity);
}*/

void UI::draw_script_selector(Entity entity)
{
  if (!entity.has_component<Script_Component>())
    return;

  auto& props = entity.get_component<Script_Component>();

  ImGui::Dummy(ImVec2(0, 5));

  bool has_script = !props.path.empty();

  if (has_script)
  {
    draw_loaded_script_panel(entity, props);
  }
  else
  {
    draw_script_drop_zone(entity, props);
  }
  draw_script_actions(entity, props, has_script);

  create_new_script(entity);
}

void UI::draw_loaded_script_panel(Entity entity, Script_Component& props)
{
  const float  corner_radius = 8.0f;
  const ImVec4 accent_color  = ImVec4(0.1f, 0.5f, 0.9f, 1.0f);
  const ImVec4 muted_color   = ImVec4(0.5f, 0.5f, 0.55f, 1.0f);

  ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, corner_radius);
  ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.1f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.2f, 0.25f, 1.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);

  ImGui::BeginChild("##script_info", ImVec2(0, 100), true);

  // Header with icon and filename
  ImGui::PushStyleColor(ImGuiCol_Text, accent_color);
  ImGui::Text(ICON_FA_FILE_CODE " LUA");
  ImGui::PopStyleColor();

  ImGui::SameLine(0, 12);
  ImGui::BeginGroup();

  std::filesystem::path file_path(props.path);
  std::string           filename = file_path.filename().string();

  ImVec2 available_size = ImGui::GetContentRegionAvail();

  ImGui::PushStyleColor(ImGuiCol_Text, muted_color);

  // Tooltip for full path
  if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
  {
    ImGui::BeginTooltip();
    ImGui::Text("%s", props.path.c_str());
    ImGui::EndTooltip();
  }
  ImGui::PopStyleColor();

  ImGui::EndGroup();
  ImGui::EndChild();

  ImGui::PopStyleColor(2);
  ImGui::PopStyleVar(2);

  if (ImGui::BeginPopupContextItem("##script_context_menu", ImGuiPopupFlags_MouseButtonRight))
  {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

    if (ImGui::MenuItem(ICON_FA_CODE "  Open in VS Code", nullptr, false, !props.path.empty()))
    {
      auto        project = Project::get_active_project();
      std::string full_path =
          project->get_directory() + project->get_scripts_directory() + props.path;
      open_in_vscode(full_path);
    }

    if (ImGui::MenuItem(ICON_FA_LINK "  Open Containing Folder",
                        nullptr,
                        false,
                        !props.path.empty()))
    {
      auto        project = Project::get_active_project();
      std::string full_path =
          project->get_directory() + project->get_scripts_directory() + props.path;
      show_in_explorer(full_path);
    }

    if (ImGui::MenuItem(ICON_FA_COPY "  Copy Path", nullptr, false, !props.path.empty()))
    {
      auto        project = Project::get_active_project();
      std::string full_path =
          project->get_directory() + project->get_scripts_directory() + props.path;
      ImGui::SetClipboardText(full_path.c_str());
    }

    ImGui::Separator();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
    if (ImGui::MenuItem(ICON_FA_TRASH "  Remove Script", nullptr, false, !props.path.empty()))
    {
      props.path.clear();
      props.env.get().clear();
      props.on_create          = LuaFunc();
      props.on_update          = LuaFunc();
      props.on_destroy         = LuaFunc();
      props.on_event           = LuaFunc();
      props.on_connected       = LuaFunc();
      props.on_disconnected    = LuaFunc();
      props.on_packet_received = LuaFunc();
      Scene::save_required();
    }
    ImGui::PopStyleColor();

    ImGui::PopStyleVar();
    ImGui::EndPopup();
  }

  if (ImGui::BeginDragDropTarget())
  {
    const ImGuiPayload* payload =
        ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM",
                                     ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

    if (payload)
    {
      const char* data = static_cast<const char*>(payload->Data);
      std::string file_path(data, payload->DataSize);
      script_drag_drop(file_path, entity);
    }
    ImGui::EndDragDropTarget();
  }
}

void UI::script_drag_drop(std::string& file_path, Entity entity)
{
  file_path.erase(std::find(file_path.begin(), file_path.end(), '\0'), file_path.end());
  file_path.erase(std::remove(file_path.begin(), file_path.end(), '\"'), file_path.end());

  Helper::normalize_path(file_path);

  std::filesystem::path path(file_path);
  std::string           ext = path.extension().string();
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  if (ext == ".lua")
  {
    EditorLayer::get().create_new_script(file_path, entity);
  }
}

void UI::draw_script_drop_zone(Entity entity, Script_Component& props)
{
  const float corner_radius = 10.0f;

  ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, corner_radius);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

  ImVec2 drop_zone_size = ImVec2(ImGui::GetContentRegionAvail().x, 120);
  ImGui::BeginChild("##drop_zone", drop_zone_size, true, ImGuiWindowFlags_NoScrollbar);

  ImVec2 window_size  = ImGui::GetWindowSize();
  ImVec2 content_size = ImVec2(window_size.x - ImGui::GetStyle().WindowPadding.x * 2,
                               window_size.y - ImGui::GetStyle().WindowPadding.y * 2);

  ImGui::SetCursorPos(
      ImVec2((window_size.x - content_size.x) * 0.5f, (window_size.y - content_size.y) * 0.5f));

  ImGui::BeginGroup();

  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 0.45f, 1.0f));
  ImGui::SetCursorPosX((content_size.x - ImGui::CalcTextSize(ICON_FA_FILE_CODE).x) * 0.5f);
  ImGui::Text(ICON_FA_FILE_CODE);
  ImGui::PopStyleColor();

  ImGui::Dummy(ImVec2(0, 8));

  // Title
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.75f, 1.0f));
  ImGui::SetCursorPosX((content_size.x - ImGui::CalcTextSize("Drop Lua Script Here").x) * 0.5f);
  ImGui::Text("Drop Lua Script Here");
  ImGui::PopStyleColor();

  ImGui::Dummy(ImVec2(0, 4));

  // Subtitle
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.55f, 1.0f));
  ImGui::SetCursorPosX((content_size.x - ImGui::CalcTextSize(".lua files supported").x) * 0.5f);
  ImGui::Text(".lua files supported");
  ImGui::PopStyleColor();

  ImGui::EndGroup();
  ImGui::EndChild();

  bool is_dragging_over = false;
  if (ImGui::BeginDragDropTarget())
  {
    if (ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM",
                                     ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
    {
      const ImGuiPayload* payload = ImGui::GetDragDropPayload();
      if (payload)
      {
        const char* data = static_cast<const char*>(payload->Data);
        std::string file_path(data, payload->DataSize);
        script_drag_drop(file_path, entity);
      }
    }
    ImGui::EndDragDropTarget();
  }

  // Optional: Add subtle hover effect
  if (ImGui::IsItemHovered() && !is_dragging_over)
  {
    ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(),
                                        ImGui::GetItemRectMax(),
                                        ImGui::GetColorU32(ImVec4(0.3f, 0.5f, 0.8f, 0.3f)),
                                        corner_radius,
                                        0,
                                        1.0f);
  }

  ImGui::PopStyleVar(2);
}

void UI::draw_script_actions(Entity entity, Script_Component& props, bool has_script)
{
  (void)props;
  (void)has_script;

  ImGui::Dummy(ImVec2(0, 10));

  const float spacing      = ImGui::GetStyle().ItemSpacing.x;
  const float button_width = (ImGui::GetContentRegionAvail().x - spacing) * 0.5f;

  GUI_Button create_button;
  create_button.label   = ICON_FA_FILE "  New Script";
  create_button.size    = {button_width, 32.0f};
  create_button.enabled = true;

  if (draw_button(create_button))
  {
    s_show_panels.create_new_script = true;
  }

  ImGui::SameLine(0, spacing);
  GUI_Button existing_button;
  existing_button.label   = ICON_FA_PLUS "  Add Existing";
  existing_button.size    = {button_width, 32.0f};
  existing_button.enabled = true;

  if (draw_button(existing_button))
  {
    auto full_path = FileDialogs::open_file("Lua Scripts (*.lua)\0*.lua\0All Files (*.*)\0*.*\0",
                                            Project::get_dialog_directory(
                                                ProjectDirectory::Scripts));
    if (!full_path.empty())
    {
      EditorLayer::get().create_new_script(full_path, entity);
    }
  }
}

void UI::open_in_vscode(const std::string& path)
{
  if (path.empty())
    return;

  std::filesystem::path abs_path = std::filesystem::absolute(path);

#ifdef PLATFORM_WINDOWS
  std::string command = "code \"" + abs_path.string() + "\"";
  system(command.c_str());

#elif defined(PLATFORM_LINUX)
  std::string command = "code \"" + abs_path.string() + "\" &";
  system(command.c_str());
#endif
}

void UI::show_in_explorer(const std::string& file_path)
{
  if (file_path.empty())
    return;

  std::filesystem::path abs_path = std::filesystem::absolute(file_path);
  std::filesystem::path folder   = abs_path.parent_path();

#ifdef PLATFORM_WINDOWS
  std::string command = "explorer /select,\"" + abs_path.string() + "\"";
  system(command.c_str());

#elif defined(PLATFORM_LINUX)
  std::string command = "xdg-open \"" + folder.string() + "\" &";
  system(command.c_str());
#endif
}

void UI::test_popup(Entity entity)
{
  static bool show_popup = false;

  if (ImGui::Button("PopUp"))
  {
    show_popup = true;
  }
  else
  {
    // return;
  }

  PopUpModel model;
  model.name         = "Texture Selector";
  model.id           = "1234567890";
  model.confirm_name = "Accept";
  model.close_name   = "Cancel";

  Extra_Settings x_settings;
  x_settings.size           = {32, 32};
  x_settings.draw_lines     = true;
  x_settings.use_size       = true;
  x_settings.create_buttons = false;
  if (show_popup)
    Texture_PopUp::draw_popup(model, x_settings, entity);
}

bool UI::draw_button(const GUI_Button& btn)
{
  bool clicked = false;

  if (btn.active)
  {
    ImVec4 old_active = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
    ImGui::PushStyleColor(ImGuiCol_Button, old_active);
  }

  if (btn.enabled)
  {
    clicked = ImGui::Button(btn.label.c_str(), btn.size.to_imvec2());
  }
  else
  {
    ImGui::BeginDisabled();
    ImGui::Button(btn.label.c_str(), btn.size.to_imvec2());
    ImGui::EndDisabled();
  }
  if (btn.active)
  {
    ImGui::PopStyleColor();
  }
  return clicked;
}

void UI::popup_functions()
{
  dirty_action();
  crash_recovery();

  create_new_scene();
  save_changes();
  EditorLayer::get().render_export_panel();
  project_settings();
}

void UI::create_new_scene()
{
  if (!s_show_panels.create_new_scene)
    return;

  PopUpModel model;
  model.id          = "##CreateNewScene";
  model.name        = "New Scene";
  model.window_size = {500, 275};

  model.on_open = []()
  {
    scene_save_path() = Project::get_dialog_directory(ProjectDirectory::Scenes);
  };

  model.draw_content = [&]()
  {
    ImGui::Text("Scene Name");
    ImGui::SetNextItemWidth(-1);

    char buffer[256];
    strncpy(buffer, new_name.c_str(), sizeof(buffer));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));
    if (ImGui::InputText("##SceneName", buffer, sizeof(buffer)))
    {
      new_name = buffer;
    }

    ImGui::Dummy(ImVec2(0, 15));

    ImGui::Text("Save Path");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 50);

    char path_buffer[512];
    strncpy(path_buffer, scene_save_path().c_str(), sizeof(path_buffer));
    if (ImGui::InputText("##ScenePath", path_buffer, sizeof(path_buffer)))
    {
      scene_save_path() = path_buffer;
    }
    ImGui::PopStyleVar();
    ImGui::SameLine();

    GUI_Button button;

    {
      button.label   = "...";
      button.size    = {40, 32};
      button.enabled = true;
      if (draw_button(button))
      {
        const std::string root = Project::get_dialog_directory(ProjectDirectory::Scenes);
        std::string selected =
            ag::FileDialogs::select_folder("Select Folder for a new Scene", root);
        if (!selected.empty())
        {
          if (Project::is_path_in_active_directory(selected, ProjectDirectory::Scenes))
          {
            scene_save_path() = selected;
          }
          else
          {
            AERO_CORE_ERROR("Scene folder \"{0}\" is outside the project Scenes folder.", selected);
          }
        }
      }
    }

    const std::string scene_root = Project::get_dialog_directory(ProjectDirectory::Scenes);
    const std::string directory  = resolve_directory_input(scene_save_path(),
                                                            ProjectDirectory::Scenes);
    const bool valid_location =
        !scene_save_path().empty() && Project::is_path_in_active_directory(directory,
                                                                     ProjectDirectory::Scenes);
    if (!valid_location)
    {
      ImGui::TextColored(ImVec4(1.0f, 0.45f, 0.45f, 1.0f),
                         "Scenes must be inside: %s",
                         scene_root.c_str());
    }

    bool can_create = is_valid_file_name(new_name) && valid_location;

    ImGui::Dummy(ImVec2(0, 25));

    float available_width = ImGui::GetContentRegionAvail().x;
    float spacing         = 10.0f;
    vec2f button_size;
    button_size.x = (available_width - (spacing * 3)) * 0.5f;
    button_size.y = 35.0f;

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + spacing);

    {
      button.label   = "Create";
      button.size    = button_size;
      button.enabled = can_create;
      if (draw_button(button))
      {
        if (can_create)
        {
          std::error_code ec;
          std::filesystem::create_directories(std::filesystem::path(directory), ec);
          std::string full_path =
              (std::filesystem::path(directory) / (new_name + ".aeroscene")).string();
          EditorLayer::get().create_new_scene(full_path);
          new_name.clear();
          scene_save_path().clear();
          s_show_panels.create_new_scene = false;
          ImGui::CloseCurrentPopup();
        }
      }
    }
    ImGui::SameLine(0, spacing);

    Color cancel_color       = Color(94, 94, 94);
    Color cancel_hover_color = Color(112, 112, 112);
    {
      button.label   = "Cancel";
      button.size    = button_size;
      button.enabled = true;
      if (draw_button(button))
      {
        new_name.clear();
        scene_save_path().clear();
        s_show_panels.create_new_scene = false;
        ImGui::CloseCurrentPopup();
      }
    }
  };

  model.on_close = []()
  {
    new_name.clear();
    scene_save_path().clear();
    s_show_panels.create_new_scene = false;
    ImGui::CloseCurrentPopup();
  };

  PopUp::draw(model);
}

void UI::create_new_script(Entity entity)
{
  if (!s_show_panels.create_new_script)
    return;

  PopUpModel model;
  model.id          = "##CreateNewScript";
  model.name        = "New Script";
  model.window_size = {500, 280};

  model.on_open = []()
  {
    script_save_path() = Project::get_dialog_directory(ProjectDirectory::Scripts);
  };

  model.draw_content = [&]()
  {
    ImGui::Text("Script Name");
    ImGui::SetNextItemWidth(-1);

    char buffer[256];
    strncpy(buffer, new_name.c_str(), sizeof(buffer));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));
    if (ImGui::InputText("##ScriptName", buffer, sizeof(buffer)))
    {
      new_name = buffer;
    }

    ImGui::Dummy(ImVec2(0, 15));

    ImGui::Text("Save Path");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 50);

    char path_buffer[512];
    strncpy(path_buffer, script_save_path().c_str(), sizeof(path_buffer));
    if (ImGui::InputText("##ScriptPath", path_buffer, sizeof(path_buffer)))
    {
      script_save_path() = path_buffer;
    }
    ImGui::PopStyleVar();
    ImGui::SameLine();

    GUI_Button button;

    {
      button.label   = "...";
      button.size    = {40, 32};
      button.enabled = true;
      if (draw_button(button))
      {
        const std::string root = Project::get_dialog_directory(ProjectDirectory::Scripts);
        std::string selected =
            ag::FileDialogs::select_folder("Select Folder for a new Script", root);
        if (!selected.empty())
        {
          if (Project::is_path_in_active_directory(selected, ProjectDirectory::Scripts))
          {
            script_save_path() = selected;
          }
          else
          {
            AERO_CORE_ERROR("Script folder \"{0}\" is outside the project Scripts folder.",
                            selected);
          }
        }
      }
    }

    const std::string script_root = Project::get_dialog_directory(ProjectDirectory::Scripts);
    const std::string directory   = resolve_directory_input(script_save_path(),
                                                             ProjectDirectory::Scripts);
    const bool valid_location =
        !script_save_path().empty() && Project::is_path_in_active_directory(directory,
                                                                      ProjectDirectory::Scripts);
    if (!valid_location)
    {
      ImGui::TextColored(ImVec4(1.0f, 0.45f, 0.45f, 1.0f),
                         "Scripts must be inside: %s",
                         script_root.c_str());
    }

    bool can_create = is_valid_file_name(new_name) && valid_location;

    ImGui::Dummy(ImVec2(0, 25));

    float available_width = ImGui::GetContentRegionAvail().x;
    float spacing         = 10.0f;
    vec2f button_size;
    button_size.x = (available_width - (spacing * 3)) * 0.5f;
    button_size.y = 35.0f;

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + spacing);

    Color create_color = can_create ? Color(94, 94, 94) : Color(58, 58, 58);
    Color hover_color  = can_create ? Color(112, 112, 112) : Color(58, 58, 58);
    {
      button.label   = "Create";
      button.size    = button_size;
      button.enabled = can_create;
      if (draw_button(button))
      {
        if (can_create)
        {
          std::error_code ec;
          std::filesystem::create_directories(std::filesystem::path(directory), ec);
          std::string full_path =
              (std::filesystem::path(directory) / (new_name + ".lua")).string();
          Helper::normalize_path(full_path);

          std::ofstream file(full_path);
          if (file.is_open())
          {
            file << "--- " << new_name << ".lua ---\n\n";
            file << "local entity = get_entity()\n\n";
            for (const char* hook : script_hooks::component)
            {
              std::string args;
              const std::string hook_name = hook;
              if (hook_name == script_hooks::OnUpdate)
                args = "dt";
              else if (hook_name == script_hooks::OnEvent)
                args = "event";
              else if (hook_name == script_hooks::OnPacketReceived)
                args = "packet";

              file << "function " << hook << "(" << args << ")\n";
              file << "  -- Implement " << hook << " for this entity.\n";
              file << "end\n\n";
            }
            file.close();

            if (!entity.has_component<Script_Component>())
              Script_Component::add_component(entity);

            auto& comps = entity.get_component<Script_Component>();
            comps.path  = Project::to_active_relative_path(full_path, ProjectDirectory::Scripts);
            Script_Component::load_scripts(entity);
            Scene::get_active_scene()->set_save_required();
          }
          else
          {
            AERO_CORE_ERROR("Failed to create script file: {0}", full_path);
          }

          new_name.clear();
          script_save_path().clear();
          s_show_panels.create_new_script = false;
          ImGui::CloseCurrentPopup();
        }
      }
    }
    ImGui::SameLine(0, spacing);

    Color cancel_color       = Color(94, 94, 94);
    Color cancel_hover_color = Color(112, 112, 112);
    {
      button.label   = "Cancel";
      button.size    = button_size;
      button.enabled = true;
      if (draw_button(button))
      {
        new_name.clear();
        script_save_path().clear();
        s_show_panels.create_new_script = false;
        ImGui::CloseCurrentPopup();
      }
    }
  };

  model.on_close = []()
  {
    new_name.clear();
    script_save_path().clear();
    s_show_panels.create_new_script = false;
    ImGui::CloseCurrentPopup();
  };

  PopUp::draw(model);
}

void UI::save_changes()
{
  if (!s_show_panels.save_changes_panel)
    return;

  {
    auto        scene             = Scene::get_active_scene();
    std::string remove_scene_name = EditorLayer::get().get_remove_scene();
    std::string scene_name        = scene->get_name();
    if (remove_scene_name == scene_name)
    {
      if (!scene->is_save_required())
      {
        EditorLayer::get().handle_scene_deletion();
        s_show_panels.save_changes_panel = false;
        return;
      }
    }
  }

  PopUpModel model;
  model.id          = "##SaveChangesPanel";
  model.name        = "Save Changes";
  model.window_size = {600, 300};

  model.draw_content = [&]()
  {
    ImGui::Text("Save Changes to the following items?");

    ImGui::Dummy(ImVec2(0, 10));

    float available_width = ImGui::GetContentRegionAvail().x;
    float spacing         = 10.0f;

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + spacing);
    float width = (available_width - (2 * spacing));

    ImGui::BeginChild("SaveChangesScenes", ImVec2(width, 150), true);
    {
      if (Application::get().m_is_closing)
      {
        EditorLayer::get().print_scene_name(true);
      }
      else
      {
        EditorLayer::get().print_scene_name();
      }
    }
    ImGui::EndChild();

    GUI_Button button;

    button.size.x = (available_width - (spacing * 4)) * 0.33f;
    button.size.y = 35.0f;

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + spacing);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetContentRegionAvail().y - button.size.y -
                         spacing);

    {
      button.label = "Save";
      if (draw_button(button))
      {
        if (Application::get().m_is_closing)
        {
          EditorLayer::get().save_all_scene();
          Application::get().m_running = false;
        }
        else
        {
          EditorLayer::get().save_scene();
          EditorLayer::get().handle_scene_deletion();
        }
        s_show_panels.save_changes_panel = false;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::SameLine(0, spacing);
    {
      button.label = "Don't Save";
      if (draw_button(button))
      {
        if (Application::get().m_is_closing)
        {
          Application::get().m_running = false;
        }
        else
        {
          EditorLayer::get().handle_scene_deletion();
        }
        s_show_panels.save_changes_panel = false;
        ImGui::CloseCurrentPopup();
      }
    }
    ImGui::SameLine(0, spacing);
    {
      button.label = "Cancel";
      if (draw_button(button))
      {
        if (Application::get().m_is_closing)
        {
          Application::get().m_is_closing = false;
        }
        else
        {
          EditorLayer::get().set_remove_scene("");
        }
        s_show_panels.save_changes_panel = false;
        ImGui::CloseCurrentPopup();
      }
    }
  };

  model.on_close = []()
  {
    if (Application::get().m_is_closing)
    {
      Application::get().m_is_closing = false;
    }
    else
    {
      EditorLayer::get().set_remove_scene("");
    }
    s_show_panels.save_changes_panel = false;
    ImGui::CloseCurrentPopup();
  };

  PopUp::draw(model);
}

void UI::dirty_action()
{
  if (!s_show_panels.dirty_action_panel)
    return;

  PopUpModel model;
  model.id          = "##DirtyActionPanel";
  model.name        = "Unsaved Changes";
  model.window_size = {520, 240};

  model.draw_content = [&]()
  {
    ImGui::TextWrapped("%s", EditorLayer::get().get_pending_scene_description().c_str());
    ImGui::Spacing();
    ImGui::TextDisabled("There are unsaved scene changes. Save them before continuing?");
    ImGui::Spacing();

    const float width     = ImGui::GetContentRegionAvail().x;
    const float spacing   = 8.0f;
    const float btn_width = (width - spacing * 2.0f) / 3.0f;

    if (ImGui::Button("Save & Continue", ImVec2(btn_width, 32)))
    {
      EditorLayer::get().apply_pending_scene_action(true);
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine(0, spacing);
    if (ImGui::Button("Continue Without Saving", ImVec2(btn_width, 32)))
    {
      EditorLayer::get().apply_pending_scene_action(false);
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine(0, spacing);
    if (ImGui::Button("Cancel", ImVec2(btn_width, 32)))
    {
      EditorLayer::get().cancel_pending_scene_action();
      ImGui::CloseCurrentPopup();
    }
  };

  model.on_close = []()
  {
    EditorLayer::get().cancel_pending_scene_action();
    ImGui::CloseCurrentPopup();
  };

  PopUp::draw(model);
}

void UI::crash_recovery()
{
  if (!s_show_panels.recovery_panel)
    return;

  PopUpModel model;
  model.id          = "##CrashRecoveryPanel";
  model.name        = "Crash Recovery";
  model.window_size = {520, 300};

  model.draw_content = [&]()
  {
    ImGui::TextWrapped(
        "The editor found autosaved scene data from a previous session. Recover it, or discard "
        "the autosaves and continue with the saved scenes.");
    ImGui::Spacing();

    ImGui::BeginChild("##RecoveryList", ImVec2(0, 110), true);
    auto names = EditorAutosave::list_names();
    if (names.empty())
      ImGui::TextDisabled("No recovery files found.");
    for (const auto& name : names)
      ImGui::BulletText("%s", name.c_str());
    ImGui::EndChild();

    ImGui::Spacing();
    const float width     = ImGui::GetContentRegionAvail().x;
    const float btn_width = (width - 8.0f) * 0.5f;
    if (ImGui::Button(ICON_FA_ROTATE_LEFT "  Recover", ImVec2(btn_width, 32)))
    {
      EditorLayer::get().recover_autosaves();
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine(0, 8.0f);
    if (ImGui::Button(ICON_FA_TRASH "  Discard", ImVec2(btn_width, 32)))
    {
      EditorLayer::get().discard_autosaves();
      ImGui::CloseCurrentPopup();
    }
  };

  model.on_close = []()
  {
    EditorLayer::get().discard_autosaves();
    ImGui::CloseCurrentPopup();
  };

  PopUp::draw(model);
}

void UI::project_settings()
{
  if (!s_show_panels.project_settings_panel)
    return;

  static int         s_active_category = 0;
  static char        s_category_filter[64] = "";
  static const char* s_status_message = nullptr;

  PopUpModel model;
  model.id   = "##ProjectSettingsPanel";
  model.name = "Project Settings";

  model.draw_content = [&]()
  {
    const float available_width  = ImGui::GetContentRegionAvail().x;
    const float available_height = ImGui::GetContentRegionAvail().y;
    const float footer_height    = 36.0f;
    const float content_height   = std::max(120.0f, available_height - footer_height - 6.0f);
    const float sidebar_width    = 220.0f;
    const float spacing          = 8.0f;

    struct SettingsCategory
    {
      const char* icon;
      const char* name;
      const char* description;
    };

    static const SettingsCategory categories[] = {
        {   ICON_FA_CIRCLE_INFO,    "Application", "Project identity, startup, and metadata" },
        { ICON_FA_WINDOW_MAXIMIZE,  "Window & Display", "Resolution, window mode, and presentation" },
        {       ICON_FA_DISPLAY,      "Rendering", "Graphics quality and rendering defaults" },
        {   ICON_FA_VOLUME_HIGH,         "Audio", "Mixing, playback, and audio behavior" },
        {     ICON_FA_GAMEPAD,         "Input", "Input devices, maps, and interaction settings" },
        {          ICON_FA_CUBE,       "Physics", "Simulation, collisions, and physics defaults" },
        {         ICON_FA_CLOCK, "Time & Simulation", "Frame timing and simulation stepping" },
        {        ICON_FA_SCROLL,     "Scripting", "Script runtime, globals, and tooling" },
        {    ICON_FA_FOLDER_OPEN, "Assets & Import", "Asset paths and import defaults" },
        {       ICON_FA_LAYER_GROUP, "Layers & Tags", "Tags, layers, and collision filtering UI" },
        { ICON_FA_NETWORK_WIRED,    "Networking", "Server/client and multiplayer settings" },
        {  ICON_FA_FILE_EXPORT, "Build & Export", "Packaging targets and build configuration" },
        {       ICON_FA_SLIDERS,        "Editor", "Editor behavior and workflow preferences" },
    };

    // Sidebar
    ImGui::BeginChild("##ProjectSettingsSidebar", ImVec2(sidebar_width, content_height), true);
    {
      ImGui::TextUnformatted(ICON_FA_GEAR "  Settings");
      ImGui::Separator();
      ImGui::SetNextItemWidth(-1.0f);
      ImGui::InputTextWithHint("##ProjectSettingsFilter", ICON_FA_MAGNIFYING_GLASS " Filter...", s_category_filter, IM_ARRAYSIZE(s_category_filter));
      ImGui::Spacing();

      ImGui::BeginChild("##ProjectSettingsCategoryList", ImVec2(0, 0), false);
      for (int i = 0; i < IM_ARRAYSIZE(categories); ++i)
      {
        if (s_category_filter[0] != '\0')
        {
          std::string hay = std::string(categories[i].name) + " " + categories[i].description;
          std::string needle = s_category_filter;
          std::transform(hay.begin(), hay.end(), hay.begin(), [](unsigned char c) { return (char)std::tolower(c); });
          std::transform(needle.begin(), needle.end(), needle.begin(), [](unsigned char c) { return (char)std::tolower(c); });
          if (hay.find(needle) == std::string::npos)
            continue;
        }

        const bool is_selected = (s_active_category == i);
        if (is_selected)
        {
          ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive));
          ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered));
        }

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 6.0f));
        const std::string label = std::string(categories[i].icon) + "  " + categories[i].name;
        if (ImGui::Selectable(("##ps_cat_" + std::to_string(i)).c_str(), is_selected, ImGuiSelectableFlags_None, ImVec2(0, 34.0f)))
          s_active_category = i;

        const ImVec2 rect_min = ImGui::GetItemRectMin();
        ImGui::GetWindowDrawList()->AddText(ImVec2(rect_min.x + 8.0f, rect_min.y + 8.0f), ImGui::GetColorU32(ImGuiCol_Text), label.c_str());
        if (ImGui::IsItemHovered() && categories[i].description)
          ImGui::SetTooltip("%s", categories[i].description);

        ImGui::PopStyleVar();
        if (is_selected)
          ImGui::PopStyleColor(2);
      }
      ImGui::EndChild();
    }
    ImGui::EndChild();

    ImGui::SameLine(0, spacing);

    // Content
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 14.0f));
    ImGui::BeginChild("##ProjectSettingsContent", ImVec2(available_width - sidebar_width - spacing, content_height), true);
    {
      ImGui::PopStyleVar();
      const float old_min_width = s_property_style.min_width;
      s_property_style.min_width = ImGui::GetContentRegionAvail().x * 0.62f;

      // Category header/description strip
      if (s_active_category >= 0 && s_active_category < IM_ARRAYSIZE(categories))
      {
        ImGui::Text("%s  %s", categories[s_active_category].icon, categories[s_active_category].name);
        ImGui::TextDisabled("%s", categories[s_active_category].description);
        ImGui::Separator();
        ImGui::Spacing();
      }

      switch (s_active_category)
      {
        case 0:  UI::draw_project_settings_application(); break;
        case 1:  UI::draw_project_settings_window(); break;
        case 2:  UI::draw_project_settings_rendering(); break;
        case 3:  UI::draw_project_settings_audio(); break;
        case 4:  UI::draw_project_settings_input(); break;
        case 5:  UI::draw_project_settings_physics(); break;
        case 6:  UI::draw_project_settings_time(); break;
        case 7:  UI::draw_project_settings_scripting(); break;
        case 8:  UI::draw_project_settings_assets(); break;
        case 9:  UI::draw_project_settings_layers(); break;
        case 10: UI::draw_project_settings_networking(); break;
        case 11: UI::draw_project_settings_build(); break;
        case 12: UI::draw_project_settings_editor(); break;
        default: break;
      }

      s_property_style.min_width = old_min_width;
    }
    ImGui::EndChild();

    // Footer actions (UI only placeholders)
    ImGui::Separator();
    ImGui::BeginChild("##ProjectSettingsFooter", ImVec2(0, footer_height), false, ImGuiWindowFlags_NoScrollbar);
    {
      if (s_status_message)
      {
        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("%s", s_status_message);
      }
      else
      {
        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("UI preview: settings wiring may be added later.");
      }

      const float button_w = 110.0f;
      const float total_w = button_w * 3.0f + ImGui::GetStyle().ItemSpacing.x * 2.0f;
      ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - total_w);
      if (ImGui::Button("Revert", ImVec2(button_w, 0))) s_status_message = "Revert requested (UI only).";
      ImGui::SameLine();
      if (ImGui::Button("Apply", ImVec2(button_w, 0))) s_status_message = "Apply requested (UI only).";
      ImGui::SameLine();
      if (ImGui::Button("Save", ImVec2(button_w, 0))) s_status_message = "Save requested (UI only).";
    }
    ImGui::EndChild();
  };

  model.on_close = []()
  {
    s_show_panels.project_settings_panel = false;
    s_status_message = nullptr;
    ImGui::CloseCurrentPopup();
  };

  PopUp::draw(model);
}

void UI::draw_settings_section(const char* label, const char* description)
{
  ImGui::Spacing();
  ImGui::SeparatorText(label);
  if (description && description[0] != '\0')
  {
    ImGui::TextDisabled("%s", description);
  }
  ImGui::Spacing();
}

void UI::draw_project_settings_application()
{
  static std::string project_name = "My Game";
  static std::string company_name = "Aero Studio";
  static std::string version_str  = "0.1.0";
  static std::string bundle_id    = "com.example.mygame";
  static std::string startup_scene= "game.aeroscene";
  static std::string icon_path    = "Assets/icon.png";
  static char description[512]    = "Describe your project...";
  static std::string author;
  static std::string website;

  // Pre-fill name from active project when available (UI convenience only)
  if (auto project = Project::get_active_project())
  {
    static bool initialized = false;
    if (!initialized)
    {
      project_name = project->get_name();
      initialized = true;
    }
  }

  draw_settings_section("Identity", "Basic project information used by the editor and build pipeline.");
  draw_string("Project Name", project_name);
  draw_string("Company / Studio", company_name);
  draw_string("Author", author);
  draw_string("Version", version_str);
  draw_string("Bundle / App ID", bundle_id);
  draw_string("Website / Homepage", website);
  ImGui::Text("Description");
  ImGui::SetNextItemWidth(-1.0f);
  ImGui::InputTextMultiline("##ProjectDescription", description, IM_ARRAYSIZE(description), ImVec2(0, 72));

  draw_settings_section("Startup & Scenes", "Default scene and entry behavior for play/build.");
  draw_string("Startup Scene", startup_scene);
  draw_string("Project Icon", icon_path);
  static int orientation = 0;
  std::vector<std::string> orientations = {"Landscape", "Portrait", "Auto", "Landscape Flipped", "Portrait Flipped"};
  draw_enum("Default Orientation", orientation, orientations);
  static bool allow_background = false;
  static bool single_instance = true;
  draw_bool("Run in Background", allow_background);
  draw_bool("Single Instance", single_instance);

  draw_settings_section("Metadata & Capabilities", "General project-level flags and metadata placeholders.");
  static bool use_custom_user_dir = false;
  static std::string user_dir_name = "AeroGame";
  draw_bool("Use Custom User Data Folder", use_custom_user_dir);
  ImGui::BeginDisabled(!use_custom_user_dir);
  draw_string("User Data Folder Name", user_dir_name);
  ImGui::EndDisabled();
  static bool enable_analytics = false;
  static bool enable_crash_reporting = false;
  draw_bool("Enable Analytics (placeholder)", enable_analytics);
  draw_bool("Enable Crash Reporting (placeholder)", enable_crash_reporting);
}

void UI::draw_project_settings_window()
{
  static int window_mode = 0;
  static int width = 1280, height = 720;
  static bool resizable = true;
  static bool borderless = false;
  static bool always_on_top = false;
  static bool vsync = true;
  static int target_fps = 60;
  static int display_index = 0;
  static int aspect_mode = 0;
  static int scale_mode = 1;
  static bool center_on_start = true;
  static bool remember_window_state = true;
  static bool allow_high_dpi = true;
  static float min_aspect_w = 16.0f, min_aspect_h = 9.0f;

  draw_settings_section("Window", "Window size, mode, and behavior.");
  std::vector<std::string> modes = {"Windowed", "Fullscreen", "Borderless Fullscreen", "Exclusive Fullscreen"};
  draw_enum("Window Mode", window_mode, modes);
  draw_value("Width", width, 320, 7680);
  draw_value("Height", height, 240, 4320);
  std::vector<std::string> displays = {"Primary Display", "Display 1", "Display 2", "Display 3"};
  draw_enum("Target Display", display_index, displays);
  draw_bool("Resizable", resizable);
  draw_bool("Borderless", borderless);
  draw_bool("Always on Top", always_on_top);
  draw_bool("Center on Start", center_on_start);
  draw_bool("Remember Window State", remember_window_state);
  draw_bool("Allow High DPI", allow_high_dpi);

  draw_settings_section("Presentation", "Frame pacing and viewport scaling behavior.");
  draw_bool("VSync", vsync);
  ImGui::BeginDisabled(vsync);
  draw_value("Target FPS", target_fps, 0, 500);
  ImGui::EndDisabled();
  std::vector<std::string> aspect_modes = {"Ignore", "Keep", "Keep Width", "Keep Height", "Expand"};
  draw_enum("Aspect Ratio Mode", aspect_mode, aspect_modes);
  std::vector<std::string> scale_modes = {"Pixel Perfect", "Linear", "Integer Scale", "Viewport Stretch"};
  draw_enum("Scale / Stretch Mode", scale_mode, scale_modes);
  draw_value("Aspect Width", min_aspect_w, 1.0f, 100.0f);
  draw_value("Aspect Height", min_aspect_h, 1.0f, 100.0f);

  draw_settings_section("Safe Area & Margins", "UI-safe viewport settings for different displays.");
  static int margin_l = 0, margin_r = 0, margin_t = 0, margin_b = 0;
  draw_value("Margin Left", margin_l, 0, 1000);
  draw_value("Margin Right", margin_r, 0, 1000);
  draw_value("Margin Top", margin_t, 0, 1000);
  draw_value("Margin Bottom", margin_b, 0, 1000);
}

void UI::draw_project_settings_rendering()
{
  static int msaa = 1;
  static int texture_filter = 1;
  static int anisotropy = 2;
  static bool batching = true;
  static bool gpu_instancing = false;
  static bool frustum_culling = true;
  static bool occlusion_culling = false;
  static bool shadows = true;
  static bool post_processing = false;
  static bool hdr = false;
  static float render_scale = 1.0f;
  static int max_texture_size = 2;
  static Color clear_color = Color(26, 26, 31, 255);
  static int pixel_snap = 0;

  draw_settings_section("Quality", "Core rendering quality and filtering defaults.");
  std::vector<std::string> msaa_opts = {"Off", "2x", "4x", "8x"};
  draw_enum("MSAA", msaa, msaa_opts);
  std::vector<std::string> filter_opts = {"Nearest", "Linear", "Mipmapped Linear"};
  draw_enum("Default Texture Filter", texture_filter, filter_opts);
  std::vector<std::string> aniso_opts = {"Off", "2x", "4x", "8x", "16x"};
  draw_enum("Anisotropic Filtering", anisotropy, aniso_opts);
  draw_value("Render Scale", render_scale, 0.25f, 2.0f);
  std::vector<std::string> tex_sizes = {"512", "1024", "2048", "4096", "8192"};
  draw_enum("Max Texture Size", max_texture_size, tex_sizes);

  draw_settings_section("Features", "Rendering features and optimizations (UI placeholders).");
  draw_bool("Sprite/Draw Batching", batching);
  draw_bool("GPU Instancing", gpu_instancing);
  draw_bool("Frustum Culling", frustum_culling);
  draw_bool("Occlusion Culling", occlusion_culling);
  draw_bool("Shadows", shadows);
  draw_bool("Post Processing", post_processing);
  draw_bool("HDR", hdr);
  std::vector<std::string> snap_opts = {"Off", "Pixel Snap 2D", "Subpixel"};
  draw_enum("Pixel Snap Mode", pixel_snap, snap_opts);

  draw_settings_section("Defaults", "Default render appearance values.");
  draw_color("Default Clear Color", clear_color);
  ImGui::TextDisabled("Note: Backend wiring for these options may be added later.");
}

void UI::draw_project_settings_audio()
{
  static float master = 1.0f, music = 0.8f, sfx = 1.0f, ui = 0.8f, ambience = 0.8f;
  static bool mute_all = false;
  static bool spatial_audio = true;
  static int max_channels = 64;
  static int sample_rate = 1;
  static float doppler = 1.0f;
  static std::string default_device = "System Default";

  draw_settings_section("Mix", "Global volume levels and mute controls.");
  draw_bool("Mute All", mute_all);
  ImGui::BeginDisabled(mute_all);
  draw_value("Master Volume", master, 0.0f, 1.0f);
  draw_value("Music Volume", music, 0.0f, 1.0f);
  draw_value("SFX Volume", sfx, 0.0f, 1.0f);
  draw_value("UI Volume", ui, 0.0f, 1.0f);
  draw_value("Ambience Volume", ambience, 0.0f, 1.0f);
  ImGui::EndDisabled();

  draw_settings_section("Playback", "Audio device and playback behavior placeholders.");
  draw_string("Output Device", default_device);
  std::vector<std::string> rates = {"44100 Hz", "48000 Hz", "96000 Hz"};
  draw_enum("Sample Rate", sample_rate, rates);
  draw_value("Max Channels / Voices", max_channels, 8, 512);
  draw_bool("Enable Spatial Audio", spatial_audio);
  draw_value("Doppler Scale", doppler, 0.0f, 10.0f);

  draw_settings_section("Streaming & Compression", "Audio import/playback defaults (UI placeholders).");
  static bool stream_music = true;
  static int compression = 1;
  static float preload_mb = 32.0f;
  draw_bool("Stream Long Music Tracks", stream_music);
  std::vector<std::string> comp = {"Uncompressed", "Compressed", "Adaptive"};
  draw_enum("Default Compression", compression, comp);
  draw_value("Audio Preload Budget (MB)", preload_mb, 0.0f, 1024.0f);
}

void UI::draw_project_settings_input()
{
  static bool mouse_capture = false;
  static bool confine_mouse = false;
  static float mouse_sensitivity = 1.0f;
  static bool invert_y = false;
  static bool enable_gamepad = true;
  static bool enable_touch = false;
  static float deadzone_left = 0.2f;
  static float deadzone_right = 0.2f;
  static int action_map_count = 0;

  draw_settings_section("Mouse & Pointer", "Pointer behavior and sensitivity.");
  draw_bool("Capture Mouse on Play", mouse_capture);
  draw_bool("Confine Mouse to Window", confine_mouse);
  draw_value("Mouse Sensitivity", mouse_sensitivity, 0.01f, 10.0f);
  draw_bool("Invert Y Axis", invert_y);
  static bool raw_mouse_input = false;
  draw_bool("Raw Mouse Input (placeholder)", raw_mouse_input);

  draw_settings_section("Gamepad & Touch", "Controller/touch support placeholders.");
  draw_bool("Enable Gamepad", enable_gamepad);
  draw_value("Left Stick Deadzone", deadzone_left, 0.0f, 1.0f);
  draw_value("Right Stick Deadzone", deadzone_right, 0.0f, 1.0f);
  draw_bool("Enable Touch Input", enable_touch);
  static bool emulate_touch_from_mouse = false;
  draw_bool("Emulate Touch from Mouse", emulate_touch_from_mouse);

  draw_settings_section("Action Maps", "Input action/axis mapping UI (placeholder list).");
  ImGui::TextDisabled("Action maps: %d configured (UI placeholder)", action_map_count);
  if (ImGui::Button(ICON_FA_PLUS " Add Action Map")) { action_map_count++; }
  ImGui::SameLine();
  if (ImGui::Button(ICON_FA_FILE_IMPORT " Import Input Map")) {}
  ImGui::SameLine();
  if (ImGui::Button(ICON_FA_FILE_EXPORT " Export Input Map")) {}
  ImGui::BeginChild("##InputActionMapPlaceholder", ImVec2(0, 120), true);
  if (action_map_count == 0)
  {
    ImGui::TextDisabled("No action maps yet. Add mappings for Move, Jump, Interact, etc.");
  }
  else
  {
    for (int i = 0; i < action_map_count; ++i)
    {
      ImGui::PushID(i);
      ImGui::BulletText("Action Map %d (UI placeholder)", i + 1);
      ImGui::SameLine();
      if (ImGui::SmallButton("Edit")) {}
      ImGui::SameLine();
      if (ImGui::SmallButton("Remove")) { action_map_count = std::max(0, action_map_count - 1); ImGui::PopID(); break; }
      ImGui::PopID();
    }
  }
  ImGui::EndChild();
}

void UI::draw_project_settings_physics()
{
  static float gravity_x = 0.0f, gravity_y = -9.81f;
  static float fixed_dt = 1.0f / 60.0f;
  static int vel_iterations = 8;
  static int pos_iterations = 3;
  static bool allow_sleep = true;
  static bool continuous_collision = false;
  static bool debug_draw_shapes = false;
  static bool debug_draw_contacts = false;
  static bool debug_draw_aabb = false;
  static float ppm = 32.0f;
  static float default_friction = 0.3f;
  static float default_restitution = 0.0f;
  static float linear_sleep_tol = 0.01f;
  static float angular_sleep_tol = 2.0f;

  draw_settings_section("World", "Global physics world parameters.");
  draw_value("Gravity X", gravity_x, -1000.0f, 1000.0f);
  draw_value("Gravity Y", gravity_y, -1000.0f, 1000.0f);
  draw_value("Pixels Per Meter", ppm, 1.0f, 1000.0f);
  draw_bool("Allow Sleeping", allow_sleep);
  draw_bool("Continuous Collision (CCD)", continuous_collision);

  draw_settings_section("Simulation", "Stepper and solver configuration.");
  draw_value("Fixed Timestep (s)", fixed_dt, 1.0f / 240.0f, 1.0f / 10.0f);
  draw_value("Velocity Iterations", vel_iterations, 1, 50);
  draw_value("Position Iterations", pos_iterations, 1, 50);
  draw_value("Linear Sleep Tolerance", linear_sleep_tol, 0.0f, 10.0f);
  draw_value("Angular Sleep Tolerance", angular_sleep_tol, 0.0f, 100.0f);

  draw_settings_section("Defaults & Debug", "Default material values and debug visualization toggles.");
  draw_value("Default Friction", default_friction, 0.0f, 1.0f);
  draw_value("Default Restitution", default_restitution, 0.0f, 1.0f);
  draw_bool("Debug Draw Shapes", debug_draw_shapes);
  draw_bool("Debug Draw Contacts", debug_draw_contacts);
  draw_bool("Debug Draw AABBs", debug_draw_aabb);
}

void UI::draw_project_settings_time()
{
  static float time_scale = 1.0f;
  static float max_delta = 0.1f;
  static float fixed_delta = 1.0f / 60.0f;
  static int max_fixed_steps = 8;
  static bool pause_on_focus_loss = false;
  static bool deterministic_mode = false;

  draw_settings_section("Timing", "Global time scaling and delta-time limits.");
  draw_value("Time Scale", time_scale, 0.0f, 10.0f);
  draw_value("Max Frame Delta (s)", max_delta, 1.0f / 1000.0f, 1.0f);
  draw_bool("Pause When Unfocused", pause_on_focus_loss);

  draw_settings_section("Fixed Update", "Fixed-step simulation settings.");
  draw_value("Fixed Delta Time (s)", fixed_delta, 1.0f / 240.0f, 1.0f / 10.0f);
  draw_value("Max Fixed Steps / Frame", max_fixed_steps, 1, 64);
  draw_bool("Deterministic Step Mode (placeholder)", deterministic_mode);

  draw_settings_section("Profiling", "Timing diagnostics UI placeholders.");
  static bool show_fps_overlay = false;
  static bool show_frame_graph = false;
  draw_bool("Show FPS Overlay in Play Mode", show_fps_overlay);
  draw_bool("Show Frame Time Graph", show_frame_graph);
}

void UI::draw_project_settings_scripting()
{
  auto& global_scripts = Project::get_active_project()->get_global_scripts();
  static bool hot_reload = true;
  static bool strict_mode = false;
  static bool sandbox_scripts = true;
  static bool log_script_errors = true;
  static int stack_size_kb = 256;

  draw_settings_section("Runtime", "Script execution behavior (UI placeholders).");
  draw_bool("Hot Reload Scripts", hot_reload);
  draw_bool("Strict Mode / Warnings as Errors", strict_mode);
  draw_bool("Sandbox Script Environment", sandbox_scripts);
  draw_bool("Log Script Errors to Console", log_script_errors);
  draw_value("Script VM Stack Size (KB)", stack_size_kb, 64, 8192);

  draw_settings_section("Global Scripts", "Scripts loaded globally for the project.");
  if (ImGui::Button(ICON_FA_PLUS " Add Global Script"))
  {
    std::string path = FileDialogs::open_file("Lua Scripts (*.lua)\0*.lua\0All Files (*.*)\0*.*\0",
                                              Project::get_dialog_directory(ProjectDirectory::Scripts));
    if (!path.empty())
    {
      std::string script_path = EditorLayer::get().get_script_path(path);
      if (!script_path.empty())
        global_scripts.global_scripts.push_back(script_path);
    }
  }
  ImGui::SameLine();
  ImGui::TextDisabled("%zu script(s)", global_scripts.global_scripts.size());

  ImGui::BeginChild("##GlobalScriptsList", ImVec2(0, 180), true);
  for (size_t i = 0; i < global_scripts.global_scripts.size(); i++)
  {
    ImGui::PushID(static_cast<int>(i));
    ImGui::BulletText("%s", global_scripts.global_scripts[i].c_str());
    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 70.0f);
    if (ImGui::SmallButton("Remove"))
    {
      global_scripts.global_scripts.erase(global_scripts.global_scripts.begin() + i);
      ImGui::PopID();
      break;
    }
    ImGui::PopID();
  }
  if (global_scripts.global_scripts.empty())
    ImGui::TextDisabled("No global scripts added.");
  ImGui::EndChild();

  draw_settings_section("Debugging", "Script debugging and diagnostics placeholders.");
  static bool enable_remote_debugger = false;
  static int debugger_port = 8172;
  static bool break_on_error = false;
  draw_bool("Enable Remote Debugger", enable_remote_debugger);
  draw_value("Debugger Port", debugger_port, 1024, 65535);
  draw_bool("Break on Script Error", break_on_error);
}

void UI::draw_project_settings_assets()
{
  static std::string assets_dir   = "Assets";
  static std::string scenes_dir   = "Scenes";
  static std::string scripts_dir  = "Scripts";
  static std::string textures_dir = "Textures";
  static std::string audio_dir    = "Audio";
  static bool auto_import = true;
  static bool watch_filesystem = true;
  static int texture_import_filter = 1;
  static bool generate_mipmaps = false;
  static int atlas_max_size = 2;

  draw_settings_section("Project Directories", "Common asset folder conventions (UI placeholders).");
  draw_string("Assets Directory", assets_dir);
  draw_string("Scenes Directory", scenes_dir);
  draw_string("Scripts Directory", scripts_dir);
  draw_string("Textures Directory", textures_dir);
  draw_string("Audio Directory", audio_dir);

  draw_settings_section("Import Behavior", "Default import and refresh behavior.");
  draw_bool("Auto Import on Asset Change", auto_import);
  draw_bool("Watch Filesystem for Changes", watch_filesystem);
  static bool reimport_on_startup = false;
  draw_bool("Reimport Modified Assets on Startup", reimport_on_startup);

  draw_settings_section("Texture & Atlas Defaults", "Default texture import settings (UI placeholders).");
  std::vector<std::string> filters = {"Nearest", "Linear", "Inherit"};
  draw_enum("Default Texture Filter", texture_import_filter, filters);
  draw_bool("Generate Mipmaps", generate_mipmaps);
  std::vector<std::string> atlas_sizes = {"1024", "2048", "4096", "8192"};
  draw_enum("Atlas Max Size", atlas_max_size, atlas_sizes);
}

void UI::draw_project_settings_layers()
{
  static char new_tag[64] = "";
  static char new_layer[64] = "";
  static std::vector<std::string> tags = {"Untagged", "Player", "Enemy", "Collectible", "UI"};
  static std::vector<std::string> layers = {"Default", "UI", "World", "Player", "Enemy", "Projectiles"};
  static bool collision_matrix[8][8] = {};

  draw_settings_section("Tags", "Named tags for gameplay and editor queries (UI placeholder list).");
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 90.0f);
  ImGui::InputTextWithHint("##NewTag", "New tag name...", new_tag, IM_ARRAYSIZE(new_tag));
  ImGui::SameLine();
  if (ImGui::Button("Add##Tag", ImVec2(80, 0)) && new_tag[0] != '\0')
  {
    tags.push_back(new_tag);
    new_tag[0] = '\0';
  }
  ImGui::BeginChild("##TagsList", ImVec2(0, 120), true);
  for (size_t i = 0; i < tags.size(); ++i)
  {
    ImGui::PushID((int)i);
    ImGui::BulletText("%s", tags[i].c_str());
    if (i > 0)
    {
      ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 70.0f);
      if (ImGui::SmallButton("Remove")) { tags.erase(tags.begin() + i); ImGui::PopID(); break; }
    }
    ImGui::PopID();
  }
  ImGui::EndChild();

  draw_settings_section("Layers", "Rendering/physics layers (UI placeholder list).");
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 90.0f);
  ImGui::InputTextWithHint("##NewLayer", "New layer name...", new_layer, IM_ARRAYSIZE(new_layer));
  ImGui::SameLine();
  if (ImGui::Button("Add##Layer", ImVec2(80, 0)) && new_layer[0] != '\0')
  {
    layers.push_back(new_layer);
    new_layer[0] = '\0';
  }
  ImGui::BeginChild("##LayersList", ImVec2(0, 120), true);
  for (size_t i = 0; i < layers.size(); ++i)
  {
    ImGui::PushID(1000 + (int)i);
    ImGui::BulletText("[%zu] %s", i, layers[i].c_str());
    ImGui::PopID();
  }
  ImGui::EndChild();

  draw_settings_section("Collision Matrix", "Layer collision filtering matrix (UI placeholder).");
  ImGui::TextDisabled("Toggle which layers collide. (Preview UI; persistence/wiring TBD)");
  const int count = (int)std::min<size_t>(layers.size(), 8);
  if (count > 0 && ImGui::BeginTable("##CollisionMatrix", count + 1, ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollX | ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn("Layer");
    for (int c = 0; c < count; ++c) ImGui::TableSetupColumn(layers[c].c_str());
    ImGui::TableHeadersRow();
    for (int r = 0; r < count; ++r)
    {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::TextUnformatted(layers[r].c_str());
      for (int c = 0; c < count; ++c)
      {
        ImGui::TableSetColumnIndex(c + 1);
        ImGui::PushID(r * 16 + c);
        ImGui::Checkbox("##cm", &collision_matrix[r][c]);
        ImGui::PopID();
      }
    }
    ImGui::EndTable();
  }
  else if (count == 0)
  {
    ImGui::TextDisabled("Add layers to configure collision filtering.");
  }
}

void UI::draw_project_settings_networking()
{
  auto& server_config = Project::get_active_project()->get_server_config();

  draw_settings_section("Mode", "Enable and configure networking/multiplayer behavior.");
  draw_bool("Enable Networking", server_config.enabled);
  static int net_mode = 0;
  std::vector<std::string> net_modes = {"Client/Server", "Peer-to-Peer (placeholder)", "Single Player + Net Ready"};
  draw_enum("Networking Mode", net_mode, net_modes);
  static int transport = 0;
  std::vector<std::string> transports = {"UDP", "TCP", "WebSocket (placeholder)"};
  draw_enum("Transport", transport, transports);

  if (!server_config.enabled)
  {
    ImGui::Spacing();
    ImGui::TextDisabled("Networking is disabled. Enable it to configure server/client settings.");
    return;
  }

  draw_settings_section("Server", "Server hosting and session settings.");
  draw_value("Port", server_config.port);
  draw_value("Max Clients", server_config.max_clients);
  draw_value("Tick Rate", server_config.tick_rate);
  static std::string server_name = "Aero Server";
  draw_string("Server Name", server_name);

  draw_settings_section("Client", "Connection target and client behavior.");
  draw_string("Server IP Address", server_config.server_IP);
  static bool auto_connect = false;
  draw_bool("Auto Connect on Start", auto_connect);

  draw_settings_section("Connection & Reliability", "Timeouts, reconnect, and reliability options.");
  draw_value("Connection Timeout (s)", server_config.connection_timeout, 1, 60);
  draw_bool("Auto Reconnect", server_config.auto_reconnect);
  draw_value("Max Reconnect Tries", server_config.max_reconnection_tries, 1, 10);
  static bool snapshot_interpolation = true;
  static bool lag_compensation = false;
  draw_bool("Snapshot Interpolation (placeholder)", snapshot_interpolation);
  draw_bool("Lag Compensation (placeholder)", lag_compensation);

  draw_settings_section("Network Scripts", "Scripts used for networking hooks/logic.");
  if (ImGui::Button(ICON_FA_PLUS " Add Network Script"))
  {
    std::string path = FileDialogs::open_file("Lua Scripts (*.lua)\0*.lua\0All Files (*.*)\0*.*\0",
                                              Project::get_dialog_directory(ProjectDirectory::Scripts));
    if (!path.empty())
    {
      std::string script_path = EditorLayer::get().get_script_path(path);
      if (!script_path.empty())
        server_config.scripts.push_back(script_path);
    }
  }
  ImGui::SameLine();
  ImGui::TextDisabled("%zu script(s)", server_config.scripts.size());

  ImGui::BeginChild("##NetworkScriptsList", ImVec2(0, 140), true);
  for (size_t i = 0; i < server_config.scripts.size(); i++)
  {
    ImGui::PushID(static_cast<int>(i));
    ImGui::BulletText("%s", server_config.scripts[i].c_str());
    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 70.0f);
    if (ImGui::SmallButton("Remove"))
    {
      server_config.scripts.erase(server_config.scripts.begin() + i);
      ImGui::PopID();
      break;
    }
    ImGui::PopID();
  }
  if (server_config.scripts.empty())
    ImGui::TextDisabled("No network scripts added.");
  ImGui::EndChild();
}

void UI::draw_project_settings_build()
{
  static int target_platform = 0;
  static int build_config = 0;
  static std::string output_dir = "Builds";
  static std::string executable_name = "Game";
  static bool include_debug_symbols = false;
  static bool strip_unused_assets = true;
  static bool compress_assets = true;
  static int compression_level = 1;
  static bool code_signing = false;
  static char scenes_in_build[512] = "game.aeroscene";

  draw_settings_section("Target", "Build target and configuration (UI placeholders).");
  std::vector<std::string> platforms = {"Windows", "Linux", "macOS", "Android", "Web (placeholder)"};
  draw_enum("Target Platform", target_platform, platforms);
  std::vector<std::string> configs = {"Debug", "Development", "Release", "Shipping"};
  draw_enum("Build Configuration", build_config, configs);
  draw_string("Output Directory", output_dir);
  draw_string("Executable / App Name", executable_name);

  draw_settings_section("Content & Packaging", "Scenes, assets, and packaging options.");
  ImGui::Text("Scenes Included in Build");
  ImGui::SetNextItemWidth(-1.0f);
  ImGui::InputTextMultiline("##ScenesInBuild", scenes_in_build, IM_ARRAYSIZE(scenes_in_build), ImVec2(0, 70));
  ImGui::TextDisabled("One scene per line or comma-separated (UI placeholder).");
  draw_bool("Strip Unused Assets", strip_unused_assets);
  draw_bool("Compress Assets", compress_assets);
  ImGui::BeginDisabled(!compress_assets);
  std::vector<std::string> comp_levels = {"Fast", "Balanced", "Maximum"};
  draw_enum("Compression Level", compression_level, comp_levels);
  ImGui::EndDisabled();

  draw_settings_section("Debugging & Distribution", "Symbols, signing, and release options.");
  draw_bool("Include Debug Symbols", include_debug_symbols);
  draw_bool("Enable Code Signing (placeholder)", code_signing);
  static bool generate_build_report = true;
  draw_bool("Generate Build Report", generate_build_report);
  if (ImGui::Button(ICON_FA_FILE_EXPORT " Open Export / Build..."))
  {
    // UI convenience hook if available; backend/export flow remains unchanged here
    // (safe to call into existing UI/editor entry point)
  }
}

void UI::draw_project_settings_editor()
{
  static bool autosave = true;
  static int autosave_minutes = 5;
  static bool load_last_project = true;
  static bool show_grid = true;
  static bool show_gizmos = true;
  static bool snap_to_grid = false;
  static float grid_size = 32.0f;
  static float ui_scale = 1.0f;
  static int theme = 0;
  static bool confirm_delete = true;
  static bool highlight_unsaved = true;

  draw_settings_section("Workflow", "General editor behavior preferences (UI placeholders).");
  draw_bool("Autosave Scenes/Project", autosave);
  ImGui::BeginDisabled(!autosave);
  draw_value("Autosave Interval (minutes)", autosave_minutes, 1, 120);
  ImGui::EndDisabled();
  draw_bool("Load Last Project on Startup", load_last_project);
  draw_bool("Confirm Before Delete", confirm_delete);
  draw_bool("Highlight Unsaved Changes", highlight_unsaved);

  draw_settings_section("Viewport & Gizmos", "Default editor viewport preferences.");
  draw_bool("Show Grid", show_grid);
  draw_value("Default Grid Size", grid_size, 1.0f, 1024.0f);
  draw_bool("Show Gizmos", show_gizmos);
  draw_bool("Snap to Grid by Default", snap_to_grid);

  draw_settings_section("Appearance", "Editor UI appearance options.");
  std::vector<std::string> themes = {"Unity Dark", "Unity Light", "Default Dark", "Default Light", "Custom"};
  draw_enum("Editor Theme", theme, themes);
  draw_value("UI Scale", ui_scale, 0.75f, 2.0f);
  static bool compact_mode = true;
  draw_bool("Compact UI Mode", compact_mode);
  ImGui::TextDisabled("Note: These are UI placeholders unless wired to editor settings.");
}

void UI::draw_settings_title(const char* title)
{
  float title_width = ImGui::CalcTextSize(title).x;
  ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - title_width) * 0.5f);
  ImGui::Text("%s", title);
  ImGui::Separator();
  ImGui::Dummy({0, 0});
}

void UI::custom_popup(const std::string&    popup_id,
                      const std::string&    popup_name,
                      std::function<void()> draw_content,
                      std::function<void()> close)
{
  PopUpModel model;
  model.id           = popup_id;
  model.name         = popup_name;
  model.draw_content = std::move(draw_content);
  model.on_close     = std::move(close);

  PopUp::draw(model);
}

void UI::run_current_scene()
{
  auto scene   = Scene::get_active_scene();
  auto project = Project::get_active_project();

  if (!scene || !project)
    return;

  std::string project_file = project->get_project_file_directory();

  // Make file writable
  Helper::makefile_read_only(project_file, false);

  // Read project file
  std::fstream file(project_file);
  if (!file.is_open())
  {
    AERO_CORE_ERROR("Cannot open project file: {}", project_file);
    return;
  }

  json j;
  try
  {
    file >> j;
    file.close();

    // Update scene info
    Helper::save_json(j["Scene"], "Default", scene->get_name(), std::string(""));
    Helper::save_json(j["Scene"], "Default Path", scene->get_directory(), std::string(""));

    // Save back
    std::ofstream out_file(project_file);
    if (!out_file.is_open())
    {
      AERO_CORE_ERROR("Cannot write to project file: {}", project_file);
      return;
    }

    out_file << j.dump(4);
    out_file.close();

    AERO_CORE_INFO("Updated scene for runtime: {}", scene->get_name());
    AERO_CORE_INFO("Scene directory: {}", scene->get_directory());
  }
  catch (const json::exception& e)
  {
    AERO_CORE_ERROR("JSON error in project file: {}", e.what());
  }
}

void UI::draw_console()
{
  // The console sink owns its own dockable window to avoid nested ImGui windows.
  Log::draw_console("Console", &s_show_panels.console_panel);
}
}  // namespace ag
