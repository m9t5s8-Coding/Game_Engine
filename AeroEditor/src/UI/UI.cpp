#include <UI/UI.hpp>
#include <Aero.hpp>
#include <Application/EditorLayer.hpp>
#include <Panels/ScenePanel.hpp>

namespace ag::UI
{
  
  void draw_menu_bar() 
  {
    if (!ImGui::BeginMainMenuBar()) return;

    // Static state for menu items
    static MenuState state;

    // File Menu
    if (ImGui::BeginMenu("File")) {
      // New Scene
      if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
        /*if (EditorLayer::get().has_unsaved_changes()) {
          state.show_new_scene_dialog = true;
        }
        else {
          EditorLayer::get().create_new_scene();
        }*/
      }

      // Open Scene
      if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {
        EditorLayer::get().open_scene();
      }

      // Recent Scenes submenu
      if (ImGui::BeginMenu("Recent Scenes")) {
        /*auto recent_scenes = EditorLayer::get().get_recent_scenes();
        if (recent_scenes.empty()) {
          ImGui::MenuItem("No recent scenes", nullptr, false, false);
        }
        else {
          for (const auto& scene : recent_scenes) {
            if (ImGui::MenuItem(scene.filename().string().c_str())) {
              EditorLayer::get().load_scene(scene.string());
            }
          }
          ImGui::Separator();
          if (ImGui::MenuItem("Clear Recent List")) {
            EditorLayer::get().clear_recent_scenes();
          }
        }*/
        ImGui::EndMenu();
      }

      ImGui::Separator();

      // Save Scene
      if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
        EditorLayer::get().save_scene();
      }

      // Save Scene As
      if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) {
        state.show_save_as_dialog = true;
      }

      // Save All
      if (ImGui::MenuItem("Save All", "Ctrl+Alt+S")) {
        //EditorLayer::get().save_all();
      }

      ImGui::Separator();

      // Project submenu
      if (ImGui::BeginMenu("Project")) {
        if (ImGui::MenuItem("New Project...")) {
          // Project creation logic
        }
        if (ImGui::MenuItem("Open Project...")) {
          // Project opening logic
        }
        if (ImGui::MenuItem("Project Settings")) {
          state.show_project_settings = true;
        }
        ImGui::EndMenu();
      }

      ImGui::Separator();

      // Import/Export
      if (ImGui::BeginMenu("Import")) {
        if (ImGui::MenuItem("FBX Model...")) {
          // Import FBX
        }
        if (ImGui::MenuItem("GLTF/GLB Model...")) {
          // Import GLTF
        }
        if (ImGui::MenuItem("Texture...")) {
          // Import texture
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Export")) {
        if (ImGui::MenuItem("Scene as FBX...")) {
          // Export scene
        }
        if (ImGui::MenuItem("Selected Objects...")) {
          // Export selection
        }
        ImGui::EndMenu();
      }

      ImGui::Separator();

      // Exit
      if (ImGui::MenuItem("Exit", "Alt+F4")) {
        //EditorLayer::get().try_exit();
      }

      ImGui::EndMenu();
    }

    // Edit Menu
    if (ImGui::BeginMenu("Edit")) {
      //bool can_undo = EditorLayer::get().can_undo();
      //bool can_redo = EditorLayer::get().can_redo();
      bool can_redo = false;
      bool can_undo = false;

      if (ImGui::MenuItem("Undo", "Ctrl+Z", false, can_undo)) {
        //EditorLayer::get().undo();
      }

      if (ImGui::MenuItem("Redo", "Ctrl+Y", false, can_redo)) {
        //EditorLayer::get().redo();
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Cut", "Ctrl+X")) {
        //EditorLayer::get().cut_selection();
      }

      if (ImGui::MenuItem("Copy", "Ctrl+C")) {
        //EditorLayer::get().copy_selection();
      }

      if (ImGui::MenuItem("Paste", "Ctrl+V")) {
        //EditorLayer::get().paste();
      }

      if (ImGui::MenuItem("Duplicate", "Ctrl+D")) {
        //EditorLayer::get().duplicate_selection();
      }

      if (ImGui::MenuItem("Delete", "Del")) {
        //EditorLayer::get().delete_selection();
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Select All", "Ctrl+A")) {
        //EditorLayer::get().select_all();
      }

      if (ImGui::MenuItem("Deselect All", "Ctrl+Shift+A")) {
        //EditorLayer::get().deselect_all();
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Find", "Ctrl+F")) {
        //EditorLayer::get().show_find_dialog();
      }

      if (ImGui::MenuItem("Replace", "Ctrl+H")) {
        //EditorLayer::get().show_replace_dialog();
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Editor Settings")) {
        state.show_editor_settings = true;
      }

      if (ImGui::MenuItem("Preferences", "Ctrl+,")) {
        //EditorLayer::get().show_preferences();
      }

      ImGui::EndMenu();
    }

    // View Menu
    if (ImGui::BeginMenu("View")) {
      static bool show_scene = true;
      static bool show_inspector = true;
      static bool show_hierarchy = true;
      static bool show_console = true;
      static bool show_project = true;
      static bool show_stats = false;
      static bool show_debug = false;

      if (ImGui::MenuItem("Scene", nullptr, &show_scene)) {
        //EditorLayer::get().toggle_window("Scene", show_scene);
      }

      if (ImGui::MenuItem("Inspector", nullptr, &show_inspector)) {
        //EditorLayer::get().toggle_window("Inspector", show_inspector);
      }

      if (ImGui::MenuItem("Hierarchy", nullptr, &show_hierarchy)) {
        //EditorLayer::get().toggle_window("Hierarchy", show_hierarchy);
      }

      if (ImGui::MenuItem("Project Browser", nullptr, &show_project)) {
        //EditorLayer::get().toggle_window("Project", show_project);
      }

      if (ImGui::MenuItem("Console", nullptr, &show_console)) {
        //EditorLayer::get().toggle_window("Console", show_console);
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Statistics", nullptr, &show_stats)) {
        //EditorLayer::get().toggle_window("Statistics", show_stats);
      }

      if (ImGui::MenuItem("Debug Tools", nullptr, &show_debug)) {
        //EditorLayer::get().toggle_window("Debug", show_debug);
      }

      ImGui::Separator();

      if (ImGui::BeginMenu("Layouts")) {
        if (ImGui::MenuItem("Default Layout")) {
          //EditorLayer::get().load_layout("default");
        }

        if (ImGui::MenuItem("Programming Layout")) {
          //EditorLayer::get().load_layout("programming");
        }

        if (ImGui::MenuItem("Design Layout")) {
          //EditorLayer::get().load_layout("design");
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Save Current Layout...")) {
          //EditorLayer::get().save_current_layout();
        }

        ImGui::EndMenu();
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Fullscreen", "F11")) {
        //EditorLayer::get().toggle_fullscreen();
      }

      ImGui::EndMenu();
    }

    // Create Menu
    if (ImGui::BeginMenu("Create")) {
      if (ImGui::BeginMenu("3D Objects")) {
        if (ImGui::MenuItem("Cube")) {
          //EditorLayer::get().create_cube();
        }
        if (ImGui::MenuItem("Sphere")) {
          //EditorLayer::get().create_sphere();
        }
        if (ImGui::MenuItem("Plane")) {
          //EditorLayer::get().create_plane();
        }
        if (ImGui::MenuItem("Cylinder")) {
          //EditorLayer::get().create_cylinder();
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Lights")) {
        if (ImGui::MenuItem("Directional Light")) {
          //EditorLayer::get().create_directional_light();
        }
        if (ImGui::MenuItem("Point Light")) {
          //EditorLayer::get().create_point_light();
        }
        if (ImGui::MenuItem("Spot Light")) {
          //EditorLayer::get().create_spot_light();
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("UI")) {
        if (ImGui::MenuItem("Canvas")) {
          //EditorLayer::get().create_canvas();
        }
        if (ImGui::MenuItem("Button")) {
          //EditorLayer::get().create_button();
        }
        if (ImGui::MenuItem("Text")) {
          //EditorLayer::get().create_text();
        }
        ImGui::EndMenu();
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Empty Object")) {
        //EditorLayer::get().create_empty_object();
      }

      if (ImGui::MenuItem("Camera")) {
        //EditorLayer::get().create_camera();
      }

      ImGui::EndMenu();
    }

    // Run Menu
    if (ImGui::BeginMenu("Run")) {
      auto now = std::chrono::steady_clock::now();
      auto since_last_run = std::chrono::duration_cast<std::chrono::seconds>(
        now - state.last_run_time).count();

      // Run buttons with status
      ImGui::BeginDisabled(state.is_running);

      if (ImGui::MenuItem("Run Scene", "F5", false, !state.is_running)) {
        state.is_running = true;
        state.last_run_time = now;

        auto folder = FileDialogs::get_exe_folder();
        std::wstring app = folder + L"\\Sandbox.exe";

        // Run in a separate thread to avoid blocking
        /*std::thread([app, &state]() {
          FileDialogs::run_exe(app);
          state.is_running = false;
          }).detach();*/
      }

      if (ImGui::MenuItem("Run Current Scene", "Ctrl+F5", false, !state.is_running)) {
        run_current_scene();
        state.is_running = true;
        state.last_run_time = now;

        auto folder = FileDialogs::get_exe_folder();
        std::wstring app = folder + L"\\Sandbox.exe";

      /*  std::thread([app, &state]() {
          FileDialogs::run_exe(app);
          state.is_running = false;
          }).detach();*/
      }

      ImGui::EndDisabled();

      // Stop button
      ImGui::BeginDisabled(!state.is_running);
      if (ImGui::MenuItem("Stop", "Shift+F5", false, state.is_running)) {
        //EditorLayer::get().stop_running_scene();
        state.is_running = false;
      }
      ImGui::EndDisabled();

      ImGui::Separator();

      // Build options
      if (ImGui::BeginMenu("Build")) {
        if (ImGui::MenuItem("Build Project")) {
          //EditorLayer::get().build_project();
        }

        if (ImGui::MenuItem("Build and Run")) {
          //EditorLayer::get().build_and_run();
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Build Settings...")) {
          //EditorLayer::get().show_build_settings();
        }

        ImGui::EndMenu();
      }

      // Debug options
      if (ImGui::BeginMenu("Debug")) {
        if (ImGui::MenuItem("Attach Debugger", "F6")) {
          //EditorLayer::get().attach_debugger();
        }

        if (ImGui::MenuItem("Step Over", "F10")) {
          //EditorLayer::get().debug_step_over();
        }

        if (ImGui::MenuItem("Step Into", "F11")) {
          //EditorLayer::get().debug_step_into();
        }

        ImGui::EndMenu();
      }

      ImGui::Separator();

      // Simulation controls
      if (ImGui::MenuItem("Play", "Ctrl+P")) {
        //EditorLayer::get().play_scene();
      }

      if (ImGui::MenuItem("Pause", "Ctrl+Shift+P")) {
        //EditorLayer::get().pause_scene();
      }

      if (ImGui::MenuItem("Stop Simulation", "Ctrl+Alt+P")) {
        //EditorLayer::get().stop_simulation();
      }

      ImGui::EndMenu();
    }

    // Tools Menu
    if (ImGui::BeginMenu("Tools")) {
      if (ImGui::MenuItem("Texture Packer")) {
        //EditorLayer::get().show_texture_packer();
      }

      if (ImGui::MenuItem("Shader Editor")) {
        //EditorLayer::get().show_shader_editor();
      }

      if (ImGui::MenuItem("Material Editor")) {
        //EditorLayer::get().show_material_editor();
      }

      if (ImGui::MenuItem("Animation Editor")) {
        //EditorLayer::get().show_animation_editor();
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Asset Browser")) {
        //EditorLayer::get().show_asset_browser();
      }

      if (ImGui::MenuItem("Scene Manager")) {
        //EditorLayer::get().show_scene_manager();
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Profiler")) {
        //EditorLayer::get().show_profiler();
      }

      if (ImGui::MenuItem("Memory Viewer")) {
        //EditorLayer::get().show_memory_viewer();
      }

      ImGui::EndMenu();
    }

    // Window Menu (alternative to View for window management)
    if (ImGui::BeginMenu("Window")) {
      if (ImGui::MenuItem("Minimize", "Ctrl+M")) {
        //EditorLayer::get().minimize_window();
      }

      if (ImGui::MenuItem("Maximize", "Ctrl+Shift+M")) {
        //EditorLayer::get().maximize_window();
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Bring All to Front")) {
        //EditorLayer::get().bring_all_to_front();
      }

      if (ImGui::MenuItem("Reset Window Positions")) {
        //EditorLayer::get().reset_window_positions();
      }

      ImGui::EndMenu();
    }

    // Help Menu
    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("Documentation", "F1")) {
        //EditorLayer::get().open_documentation();
      }

      if (ImGui::MenuItem("Keyboard Shortcuts")) {
        state.show_shortcuts_dialog = true;
      }

      if (ImGui::MenuItem("Tutorials")) {
        //EditorLayer::get().open_tutorials();
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Check for Updates")) {
        //EditorLayer::get().check_for_updates();
      }

      ImGui::Separator();

      if (ImGui::MenuItem("About")) {
        state.show_about_dialog = true;
      }

      ImGui::EndMenu();
    }

    // Status info on the right
    ImGui::SameLine(ImGui::GetWindowWidth() - 300);

    // Show running status
    //if (state.is_running) {
    //  float time_running = std::chrono::duration<float>(
    //    std::chrono::steady_clock::now() - state.last_run_time).count();

    //  ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
    //    //ICON_FA_PLAY " Running (%.1fs)", time_running);
    //}
    //else if (since_last_run < 5) {
    //  ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f),
    //    //ICON_FA_STOP " Last run: %.0fs ago", since_last_run);
    //}

    // Show scene info
    auto scene = Scene::get_active_scene();
    if (scene && !state.is_running) {
      ImGui::SameLine();
      std::string scene_name = scene->get_name();
      if (scene_name.empty()) scene_name = "Untitled";

      /*if (EditorLayer::get().has_unsaved_changes()) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
          ICON_FA_FILE " %s*", scene_name.c_str());
      }
      else {
        ImGui::TextDisabled(ICON_FA_FILE " %s", scene_name.c_str());
      }*/
    }

    // Show FPS
    ImGui::SameLine();
    ImGui::TextDisabled("FPS: %.1f", ImGui::GetIO().Framerate);

    ImGui::EndMainMenuBar();

    // Handle dialogs
    handle_dialogs(state);
  }

  // Helper functions
  void run_current_scene() {
    auto scene = Scene::get_active_scene();
    auto project = Project::get_active_project();

    if (!scene || !project) return;

    std::string project_file = project->get_project_file_directory();

    // Make file writable
    Helper::makefile_read_only(project_file, false);

    // Read project file
    std::fstream file(project_file);
    if (!file.is_open()) {
      AERO_CORE_ERROR("Cannot open project file: {}", project_file);
      return;
    }

    json j;
    try {
      file >> j;
      file.close();

      // Update scene info
      Helper::save_json(j["Scene"], "Default", scene->get_name());
      Helper::save_json(j["Scene"], "Default Path", scene->get_directory());

      // Save back
      std::ofstream out_file(project_file);
      if (!out_file.is_open()) {
        AERO_CORE_ERROR("Cannot write to project file: {}", project_file);
        return;
      }

      out_file << j.dump(4);
      out_file.close();

      AERO_CORE_INFO("Updated scene for runtime: {}", scene->get_name());
      AERO_CORE_INFO("Scene directory: {}", scene->get_directory());

    }
    catch (const json::exception& e) {
      AERO_CORE_ERROR("JSON error in project file: {}", e.what());
    }

  }

  void handle_dialogs(MenuState& state) {
    // New Scene dialog
    if (state.show_new_scene_dialog) {
      ImGui::OpenPopup("New Scene");
      state.show_new_scene_dialog = false;
    }

    if (ImGui::BeginPopupModal("New Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::Text("You have unsaved changes. Save before creating new scene?");
      ImGui::Separator();

      if (ImGui::Button("Save and New")) {
        EditorLayer::get().save_scene();
        EditorLayer::get().create_new_scene();
        ImGui::CloseCurrentPopup();
      }

      ImGui::SameLine();
      if (ImGui::Button("Discard and New")) {
        EditorLayer::get().create_new_scene();
        ImGui::CloseCurrentPopup();
      }

      ImGui::SameLine();
      if (ImGui::Button("Cancel")) {
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }

    // Save As dialog
    if (state.show_save_as_dialog) {
      auto result = FileDialogs::save_file("Aero Scene (*.aero)\0*.aero\0");
      if (!result.empty()) {
       /* EditorLayer::get().save_scene_as(result);*/
      }
      state.show_save_as_dialog = false;
    }

    // About dialog
    if (state.show_about_dialog) {
      ImGui::OpenPopup("About");
      state.show_about_dialog = false;
    }

    if (ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::Text("Aero Engine Editor");
      ImGui::Text("Version 1.0.0");
      ImGui::Separator();
      ImGui::Text("A modern game engine editor");
      ImGui::Text("Built with C++20, OpenGL, and Dear ImGui");
      ImGui::Separator();

      if (ImGui::Button("Close")) {
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }

    // Shortcuts dialog
    if (state.show_shortcuts_dialog) {
      ImGui::OpenPopup("Keyboard Shortcuts");
      state.show_shortcuts_dialog = false;
    }

    if (ImGui::BeginPopupModal("Keyboard Shortcuts", nullptr,
      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
      ImGui::BeginTable("shortcuts", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("Action");
      ImGui::TableSetupColumn("Shortcut");
      ImGui::TableHeadersRow();

      // File shortcuts
      add_shortcut_row("New Scene", "Ctrl+N");
      add_shortcut_row("Open Scene", "Ctrl+O");
      add_shortcut_row("Save Scene", "Ctrl+S");
      add_shortcut_row("Save Scene As", "Ctrl+Shift+S");
      add_shortcut_row("Exit", "Alt+F4");

      ImGui::TableNextRow();
      ImGui::TableNextColumn(); ImGui::Text("Edit");
      ImGui::TableNextColumn(); ImGui::Text("");

      add_shortcut_row("Undo", "Ctrl+Z");
      add_shortcut_row("Redo", "Ctrl+Y");
      add_shortcut_row("Cut", "Ctrl+X");
      add_shortcut_row("Copy", "Ctrl+C");
      add_shortcut_row("Paste", "Ctrl+V");
      add_shortcut_row("Delete", "Del");

      ImGui::TableNextRow();
      ImGui::TableNextColumn(); ImGui::Text("Run");
      ImGui::TableNextColumn(); ImGui::Text("");

      add_shortcut_row("Run Scene", "F5");
      add_shortcut_row("Run Current Scene", "Ctrl+F5");
      add_shortcut_row("Stop", "Shift+F5");
      add_shortcut_row("Play Simulation", "Ctrl+P");

      ImGui::TableNextRow();
      ImGui::TableNextColumn(); ImGui::Text("View");
      ImGui::TableNextColumn(); ImGui::Text("");

      add_shortcut_row("Toggle Fullscreen", "F11");
      add_shortcut_row("Toggle Scene View", "F2");
      add_shortcut_row("Toggle Inspector", "F3");

      ImGui::EndTable();

      ImGui::Separator();

      if (ImGui::Button("Close")) {
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }
  }

  void add_shortcut_row(const char* action, const char* shortcut) {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("%s", action);
    ImGui::TableNextColumn();
    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "%s", shortcut);
  }

	/*void draw_menu_bar()
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

      if (ImGui::BeginMenu("Run"))
      {
        if (ImGui::MenuItem("Run Scene"))
        {
          auto folder = FileDialogs::get_exe_folder();
          std::wstring app = folder + L"\\Sandbox.exe";

          FileDialogs::run_exe(app);
        }

        if (ImGui::MenuItem("Run Current Scene"))
        {
          auto scene = Scene::get_active_scene();
          auto project = Project::get_active_project();

          std::string project_file = project->get_project_file_directory();
          Helper::makefile_read_only(project_file, false);
          std::fstream file(project_file);
          json j;
          if (!file.is_open())
          {
            AERO_CORE_INFO("Cannot Open File {0}", project_file);
          }
          file >> j;
          file.close();

          Helper::save_json(j["Scene"], "Default", scene->get_name());
          Helper::save_json(j["Scene"], "Default Path", scene->get_directory());

          AERO_CORE_INFO("Scene Name: {0}", scene->get_name());
          AERO_CORE_INFO("Scene Directory: {0}", scene->get_directory());

          std::ofstream out_file(project_file);
          if (!out_file.is_open())
          {
            AERO_CORE_INFO("Cannot Open File: {0}", project_file);
          }
          out_file << j.dump(4);
          out_file.close();
          Helper::makefile_read_only(project_file);

          auto folder = FileDialogs::get_exe_folder();
          std::wstring app = folder + L"\\Sandbox.exe";

          FileDialogs::run_exe(app);
        }
        
        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }
	}*/

  bool texture_selector(const AG_ref<Texture>& texture, uint_rect& texture_rect) {
    static Selector state;

    static bool window_open = true;
    bool selected = false;

    if (!texture) return false;

    // Initialize safe area if texture size is known
    if (state.safe_area.size.x == 0 && texture->get_size().x > 0) {
      auto tex_size = texture->get_size();
      state.safe_area = {
          {50, 50},   
          {tex_size.x - 100, tex_size.y - 100}
      };
      state.aspect_ratio = static_cast<float>(tex_size.x) / tex_size.y;
    }

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoScrollWithMouse |
      ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("Texture Selector", &window_open, flags)) {
      // Toolbar
      ImGui::BeginGroup();
      {
        // Zoom controls
        ImGui::PushItemWidth(80.0f);
        if (ImGui::SliderFloat("Zoom", &state.zoom, state.min_zoom, state.max_zoom, "%.2fx")) {
          state.zoom = std::clamp(state.zoom, state.min_zoom, state.max_zoom);
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        if (ImGui::Button("Reset##zoom")) {
          state.zoom = 1.0f;
          state.pan_offset;
        }

        ImGui::SameLine();
        ImGui::Checkbox("Grid", &state.show_grid);

        ImGui::SameLine();
        if (ImGui::Checkbox("Pixel Grid", &state.show_pixel_grid)) {
          if (state.show_pixel_grid) {
            state.grid_size = 1;
          }
          else {
            state.grid_size = 32;
          }
        }

        ImGui::SameLine();
        ImGui::Checkbox("Safe Area", &state.show_safe_area);

        ImGui::SameLine();
        ImGui::Checkbox("Lock Aspect", &state.lock_aspect_ratio);

        ImGui::SameLine();
        if (ImGui::Button("Clear##selection")) {
          state.has_selection = false;
          state.selecting = false;
        }
      }
      ImGui::EndGroup();

      ImGui::Separator();

      // Image display area
      {
        ImGui::BeginChild("ImageArea", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2),
          true, ImGuiWindowFlags_NoScrollbar);

        const vec2f tex_size = texture->get_size();
        const vec2f image_size = vec2f(tex_size.x * state.zoom, tex_size.y * state.zoom);
        const vec2f available_size = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

        // Calculate centered position with pan offset
        vec2f center = available_size * 0.5f + state.pan_offset;
        vec2f offset = center - (image_size * 0.5f);

        ImGui::SetCursorPos(offset.to_imvec2());

        // Get image screen coordinates
        const vec2f image_min = { ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y };
        const vec2f image_max = image_min + image_size;

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        // Draw background
        draw_list->AddRectFilled(image_min.to_imvec2(), image_max.to_imvec2(),
          IM_COL32(40, 40, 40, 255));

        // Draw grid if enabled
        if (state.show_grid && state.zoom > 0.5f) {
          draw_grid(draw_list, image_min, image_max, tex_size, state.zoom,
            state.grid_size, state.show_pixel_grid);
        }

        // Draw safe area if enabled
        if (state.show_safe_area) {
          draw_safe_area(draw_list, image_min, image_max, tex_size,
            state.zoom, state.safe_area);
        }

        // Draw texture
        ImGui::Image((void*)texture->get_texture_id(), image_size.to_imvec2());

        // Handle mouse interactions
        handle_mouse_interactions(image_min, image_max, tex_size, state);

        // Draw selection rectangle
        if (state.selecting || state.has_selection) {
          draw_selection(draw_list, state, image_min, image_max, tex_size);
        }

        // Draw info overlay
        draw_info_overlay(draw_list, image_min, tex_size, state, available_size);

        // Convert selection to texture coordinates if complete
        if (state.has_selection && !state.selecting) {
          selected = convert_selection_to_rect(state, image_min, image_max,
            tex_size, texture_rect);
          if (selected) {
            state.status_message = fmt::format("Selected: {}x{} at ({}, {})",
              texture_rect.size.x,
              texture_rect.size.y,
              texture_rect.position.x,
              texture_rect.position.y);
            state.status_timer = 3.0f;
          }
          state.has_selection = false;
        }

        ImGui::EndChild();
      }

      // Status bar
      render_status_bar(state);

      // Selection info panel
      if (selected) {
        render_selection_info(texture_rect);
      }
    }
    ImGui::End();

    return selected;
  }
  
  void draw_grid(ImDrawList* draw_list, const vec2f& image_min, const vec2f& image_max,
    const vec2f& tex_size, float zoom, int grid_size, bool pixel_grid) {
    const ImU32 grid_color = IM_COL32(60, 60, 60, 150);
    const ImU32 major_grid_color = IM_COL32(80, 80, 80, 200);
    const float grid_spacing = grid_size * zoom;

    if (grid_spacing < 2.0f) return;

    // Vertical lines
    for (float x = image_min.x + grid_spacing; x < image_max.x; x += grid_spacing) {
      bool is_major = (static_cast<int>((x - image_min.x) / grid_spacing) % 8 == 0);
      draw_list->AddLine(ImVec2(x, image_min.y), ImVec2(x, image_max.y),
        is_major ? major_grid_color : grid_color, 1.0f);
    }

    // Horizontal lines
    for (float y = image_min.y + grid_spacing; y < image_max.y; y += grid_spacing) {
      bool is_major = (static_cast<int>((y - image_min.y) / grid_spacing) % 8 == 0);
      draw_list->AddLine(ImVec2(image_min.x, y), ImVec2(image_max.x, y),
        is_major ? major_grid_color : grid_color, 1.0f);
    }

    // Pixel grid (only at high zoom)
    if (pixel_grid && zoom > 4.0f) {
      const ImU32 pixel_grid_color = IM_COL32(100, 100, 100, 80);
      const float pixel_spacing = zoom;

      for (float x = image_min.x + pixel_spacing; x < image_max.x; x += pixel_spacing) {
        draw_list->AddLine(ImVec2(x, image_min.y), ImVec2(x, image_max.y),
          pixel_grid_color, 0.5f);
      }
      for (float y = image_min.y + pixel_spacing; y < image_max.y; y += pixel_spacing) {
        draw_list->AddLine(ImVec2(image_min.x, y), ImVec2(image_max.x, y),
          pixel_grid_color, 0.5f);
      }
    }
  }

  void draw_safe_area(ImDrawList* draw_list, const vec2f& image_min, const vec2f& image_max,
    const vec2f& tex_size, float zoom, const uint_rect& safe_area) {
    if (safe_area.size.x == 0 || safe_area.size.y == 0) return;

    vec2f safe_min = image_min + vec2f(safe_area.position.x * zoom,
      safe_area.position.y * zoom);
    vec2f safe_max = safe_min + vec2f(safe_area.size.x * zoom,
      safe_area.size.y * zoom);

    // Clamp to image bounds
    safe_min.x = std::clamp(safe_min.x, image_min.x, image_max.x);
    safe_min.y = std::clamp(safe_min.y, image_min.y, image_max.y);
    safe_max.x = std::clamp(safe_max.x, image_min.x, image_max.x);
    safe_max.y = std::clamp(safe_max.y, image_min.y, image_max.y);

    draw_list->AddRect(safe_min.to_imvec2(), safe_max.to_imvec2(),
      IM_COL32(255, 50, 50, 100), 0.0f, 0, 2.0f);

    // Label
    ImVec2 text_pos = ImVec2(safe_min.x + 5, safe_min.y + 5);
    draw_list->AddText(text_pos, IM_COL32(255, 100, 100, 200), "Safe Area");
  }

  void handle_mouse_interactions(const vec2f& image_min, const vec2f& image_max,
    const vec2f& tex_size, Selector& state) {
    bool is_hovered = ImGui::IsItemHovered();
    bool is_window_hovered = ImGui::IsWindowHovered();
    ImGuiIO& io = ImGui::GetIO();

    // Zoom with mouse wheel
    if (is_hovered && io.MouseWheel != 0.0f) {
      vec2f mouse_pos = { io.MousePos.x, io.MousePos.y };
      vec2f image_rel_before = (mouse_pos - image_min) / state.zoom;

      float new_zoom = state.zoom + io.MouseWheel * state.zoom_speed;
      new_zoom = std::clamp(new_zoom, state.min_zoom, state.max_zoom);

      if (state.zoom != new_zoom) {
        // Zoom towards mouse position
        vec2f image_rel_after = (mouse_pos - image_min) / new_zoom;
        vec2f zoom_offset = (image_rel_before - image_rel_after) * new_zoom;
        state.pan_offset += zoom_offset;

        state.zoom = new_zoom;
      }
    }

    // Pan with middle mouse button
    if (is_window_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
      state.panning = true;
      state.drag_start = { io.MousePos.x, io.MousePos.y };
    }

    if (state.panning) {
      if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
        state.drag_current = { io.MousePos.x, io.MousePos.y };
        vec2f delta = state.drag_current - state.drag_start;
        state.pan_offset += delta;
        state.drag_start = state.drag_current;
      }
      else {
        state.panning = false;
      }
    }

    // Selection with left mouse button
    if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
      state.selecting = true;
      state.has_selection = false;
      state.select_start = { io.MousePos.x, io.MousePos.y };
      state.select_end = state.select_start;
    }

    if (state.selecting) {
      if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        state.select_end = { io.MousePos.x, io.MousePos.y };

        // Apply aspect ratio lock
        if (state.lock_aspect_ratio) {
          vec2f delta = state.select_end - state.select_start;
          if (std::abs(delta.x) > std::abs(delta.y)) {
            delta.y = delta.x / state.aspect_ratio;
          }
          else {
            delta.x = delta.y * state.aspect_ratio;
          }
          state.select_end = state.select_start + delta;
        }
      }
      else {
        state.selecting = false;
        state.has_selection = true;
      }
    }
  }

  void draw_selection(ImDrawList* draw_list, Selector& state, const vec2f& image_min,
    const vec2f& image_max, const vec2f& tex_size) {
    vec2f sel_min(std::min(state.select_start.x, state.select_end.x),
      std::min(state.select_start.y, state.select_end.y));
    vec2f sel_max(std::max(state.select_start.x, state.select_end.x),
      std::max(state.select_start.y, state.select_end.y));

    // Clamp to image bounds
    sel_min.x = std::clamp(sel_min.x, image_min.x, image_max.x);
    sel_min.y = std::clamp(sel_min.y, image_min.y, image_max.y);
    sel_max.x = std::clamp(sel_max.x, image_min.x, image_max.x);
    sel_max.y = std::clamp(sel_max.y, image_min.y, image_max.y);

    // Draw selection rectangle
    draw_list->AddRect(sel_min.to_imvec2(), sel_max.to_imvec2(),
      IM_COL32(255, 200, 0, 255), 0.0f, 0, 2.0f);

    // Draw resize handles
    if (state.selecting) {
      const float handle_size = 6.0f;
      const ImU32 handle_color = IM_COL32(255, 255, 255, 255);

      // Four corners
      draw_list->AddCircleFilled(sel_min.to_imvec2(), handle_size, handle_color);
      draw_list->AddCircleFilled(ImVec2(sel_max.x, sel_min.y), handle_size, handle_color);
      draw_list->AddCircleFilled(ImVec2(sel_min.x, sel_max.y), handle_size, handle_color);
      draw_list->AddCircleFilled(sel_max.to_imvec2(), handle_size, handle_color);
    }

    // Draw dimensions text
    vec2f pixel_min = (sel_min - image_min) / state.zoom;
    vec2f pixel_max = (sel_max - image_min) / state.zoom;
    int width = static_cast<int>(pixel_max.x - pixel_min.x);
    int height = static_cast<int>(pixel_max.y - pixel_min.y);

    std::string dim_text = fmt::format("{} x {}", width, height);
    ImVec2 text_size = ImGui::CalcTextSize(dim_text.c_str());
    ImVec2 text_pos = ImVec2((sel_min.x + sel_max.x - text_size.x) * 0.5f,
      sel_min.y - text_size.y - 5);

    if (text_pos.y > image_min.y) {
      draw_list->AddRectFilled(text_pos,
        ImVec2(text_pos.x + text_size.x + 4,
          text_pos.y + text_size.y + 4),
        IM_COL32(0, 0, 0, 180));
      draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), dim_text.c_str());
    }
  }

  void draw_info_overlay(ImDrawList* draw_list, const vec2f& image_min,
    const vec2f& tex_size, Selector& state, const vec2f& available_size) {
    if (!ImGui::IsItemHovered()) return;

    ImGuiIO& io = ImGui::GetIO();
    vec2f mouse_pos = { io.MousePos.x, io.MousePos.y };

    // Only show pixel info when over the image
    if (mouse_pos.x >= image_min.x && mouse_pos.x < image_min.x + tex_size.x * state.zoom &&
      mouse_pos.y >= image_min.y && mouse_pos.y < image_min.y + tex_size.y * state.zoom) {

      // Convert to pixel coordinates
      vec2f pixel_pos = (mouse_pos - image_min) / state.zoom;
      int pixel_x = static_cast<int>(std::floor(pixel_pos.x));
      int pixel_y = static_cast<int>(std::floor(pixel_pos.y));

      // Clamp to texture bounds
      pixel_x = std::clamp(pixel_x, 0, static_cast<int>(tex_size.x) - 1);
      pixel_y = std::clamp(pixel_y, 0, static_cast<int>(tex_size.y) - 1);

      // Draw pixel info
      std::string pixel_info = fmt::format("Pixel: ({}, {})", pixel_x, pixel_y);
      ImVec2 text_size = ImGui::CalcTextSize(pixel_info.c_str());
      ImVec2 text_pos = ImVec2(available_size.x - text_size.x - 10, 10);

      draw_list->AddRectFilled(text_pos,
        ImVec2(text_pos.x + text_size.x + 8,
          text_pos.y + text_size.y + 8),
        IM_COL32(0, 0, 0, 180));
      draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), pixel_info.c_str());
    }
  }

  bool convert_selection_to_rect(Selector& state, const vec2f& image_min, const vec2f& image_max,
    const vec2f& tex_size, uint_rect& texture_rect) {
    vec2f sel_min(std::min(state.select_start.x, state.select_end.x),
      std::min(state.select_start.y, state.select_end.y));
    vec2f sel_max(std::max(state.select_start.x, state.select_end.x),
      std::max(state.select_start.y, state.select_end.y));

    // Clamp to image bounds
    sel_min.x = std::clamp(sel_min.x, image_min.x, image_max.x);
    sel_min.y = std::clamp(sel_min.y, image_min.y, image_max.y);
    sel_max.x = std::clamp(sel_max.x, image_min.x, image_max.x);
    sel_max.y = std::clamp(sel_max.y, image_min.y, image_max.y);

    // Convert to pixel coordinates
    vec2f pixel_min = (sel_min - image_min) / state.zoom;
    vec2f pixel_max = (sel_max - image_min) / state.zoom;

    // Pixel-perfect rounding
    uint32_t x0 = static_cast<uint32_t>(std::floor(pixel_min.x));
    uint32_t y0 = static_cast<uint32_t>(std::floor(pixel_min.y));
    uint32_t x1 = static_cast<uint32_t>(std::ceil(pixel_max.x));
    uint32_t y1 = static_cast<uint32_t>(std::ceil(pixel_max.y));

    // Clamp to texture bounds
    x0 = std::min(x0, static_cast<uint32_t>(tex_size.x) - 1);
    y0 = std::min(y0, static_cast<uint32_t>(tex_size.y) - 1);
    x1 = std::min(x1, static_cast<uint32_t>(tex_size.x));
    y1 = std::min(y1, static_cast<uint32_t>(tex_size.y));

    uint32_t width = x1 - x0;
    uint32_t height = y1 - y0;

    const uint32_t min_size = 1; // Minimum 1x1 pixel selection

    if (width >= min_size && height >= min_size) {
      texture_rect.position.x = x0;
      texture_rect.position.y = y0;
      texture_rect.size.x = width;
      texture_rect.size.y = height;
      return true;
    }

    return false;
  }

  void render_status_bar(Selector& state) {
    ImGui::BeginChild("StatusBar", ImVec2(0, 0), true);

    // Texture info
    //ImGui::Text("Texture: %dx%d",
    //  static_cast<int>(tex_size.x),
    //  static_cast<int>(tex_size.y));

    //ImGui::SameLine(ImGui::GetWindowWidth() - 200);

    //// Selection info
    //if (state.has_selection || state.selecting) {
    //  vec2f pixel_min = (sel_min - image_min) / state.zoom;
    //  vec2f pixel_max = (sel_max - image_min) / state.zoom;
    //  int width = static_cast<int>(pixel_max.x - pixel_min.x);
    //  int height = static_cast<int>(pixel_max.y - pixel_min.y);
    //  ImGui::Text("Selection: %dx%d", width, height);
    //}
    //else {
    //  ImGui::TextDisabled("No selection");
    //}

    //// Status message with timer
    //if (state.status_timer > 0.0f) {
    //  state.status_timer -= ImGui::GetIO().DeltaTime;
    //  ImGui::SameLine();
    //  ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", state.status_message.c_str());
    //}

    ImGui::EndChild();
  }

  void render_selection_info(const uint_rect& texture_rect) {
    if (ImGui::CollapsingHeader("Selection Details", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("Position: (%d, %d)", texture_rect.position.x, texture_rect.position.y);
      ImGui::Text("Size: %dx%d", texture_rect.size.x, texture_rect.size.y);
      ImGui::Text("Area: %d pixels", texture_rect.size.x * texture_rect.size.y);

      // Aspect ratio
      float aspect = static_cast<float>(texture_rect.size.x) / texture_rect.size.y;
      ImGui::Text("Aspect Ratio: %.2f:1", aspect);

      // Common resolutions
      ImGui::Separator();
      ImGui::Text("Common Sizes:");
      if (texture_rect.size == vec2u(16, 16)) ImGui::TextDisabled("  16x16 - Icon");
      if (texture_rect.size == vec2u(32, 32)) ImGui::TextDisabled("  32x32 - Small Icon");
      if (texture_rect.size == vec2u(64, 64)) ImGui::TextDisabled("  64x64 - Medium Icon");
      if (texture_rect.size == vec2u(128, 128)) ImGui::TextDisabled("  128x128 - Large Icon");
      if (texture_rect.size == vec2u(256, 256)) ImGui::TextDisabled("  256x256 - Texture");
      if (texture_rect.size == vec2u(512, 512)) ImGui::TextDisabled("  512x512 - HD Texture");
      if (texture_rect.size == vec2u(1024, 1024)) ImGui::TextDisabled("  1024x1024 - Full HD");
    }
  }


  void draw_console()
  {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Console", nullptr, flags);
    static bool show_console = true;
    Log::draw_console("Console", &show_console);

    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("View"))
      {
        if (ImGui::MenuItem("Console", "Ctrl+`", &show_console))
        {
        }
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }
    ImGui::End();
  }
}