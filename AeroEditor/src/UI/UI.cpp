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

				FileDialogs::run_exe(app);
			}

			if (ImGui::MenuItem("Run Current Scene", "Ctrl+F5", false, !state.is_running)) {
				run_current_scene();
				state.is_running = true;
				state.last_run_time = now;

				auto folder = FileDialogs::get_exe_folder();
				std::wstring app = folder + L"\\Sandbox.exe";

				FileDialogs::run_exe(app);
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

	void draw_texture(Entity entity)
	{
		auto& props = entity.get_component<Texture_Component>();

		ImGui::Text("Texture");
		ImGui::SameLine();

		if (props.texture && !props.path.empty())
		{
			ImGui::TextDisabled("(Loaded)");
		}
		else if (!props.path.empty())
		{
			ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "(Not Loaded)");
		}
		else
		{
			ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "(No Texture)");
		}

		ImGui::Spacing();

		if (!props.path.empty()) {
			ImGui::TextWrapped("Path: %s", props.path.c_str());
			ImGui::Spacing();
		}

		// --- Texture Preview ---
		if (props.texture && !props.path.empty()) {
			ImGui::Separator();
			ImGui::Text("Preview:");

			// Calculate aspect ratio preserving size
			float width = props.texture->get_width();
			float height = props.texture->get_height();
			float aspect = width / height;

			ImVec2 preview_size(128.0f, 128.0f);
			if (aspect > 1.0f) {
				preview_size.y = 128.0f / aspect;
			}
			else {
				preview_size.x = 128.0f * aspect;
			}

			// Add padding around the image (frame larger than image)
			const float padding = 10.0f; // Extra space around image
			ImVec2 frame_size = ImVec2(preview_size.x + padding * 2,
				preview_size.y + padding * 2);

			// Center the frame horizontally
			float available_width = ImGui::GetContentRegionAvail().x;
			float frame_pos_x = (available_width - frame_size.x) / 2.0f;

			// Create a child frame/group for the image with padding
			ImGui::BeginChild("TexturePreviewFrame",
				ImVec2(0, frame_size.y + 5), // Height of frame + spacing
				false,
				ImGuiWindowFlags_NoScrollbar);

			// Set cursor to center the frame
			ImGui::SetCursorPosX(frame_pos_x);

			// Draw a background frame/rectangle
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			ImVec2 frame_min = ImGui::GetCursorScreenPos();
			ImVec2 frame_max = ImVec2(frame_min.x + frame_size.x,
				frame_min.y + frame_size.y);

			// Draw background rectangle (slightly darker than window bg)
			draw_list->AddRectFilled(frame_min, frame_max,
				ImGui::GetColorU32(ImGuiCol_FrameBg),
				4.0f); // Rounded corners

			// Draw border
			draw_list->AddRect(frame_min, frame_max,
				ImGui::GetColorU32(ImGuiCol_Border),
				4.0f, 0, 1.5f); // Thicker border

			// Center image inside the frame
			ImVec2 image_pos = ImVec2(frame_min.x + padding,
				frame_min.y + padding);
			ImGui::SetCursorScreenPos(image_pos);

			// Draw the image
			ImGui::Image((void*)(intptr_t)props.texture->get_texture_id(),
				preview_size);

			ImGui::EndChild(); // End TexturePreviewFrame

			// Texture info below the frame
			ImGui::Text("Size: %dx%d",
				props.texture->get_width(),
				props.texture->get_height());

			// Texture format/info if available
			ImGui::SameLine();
			ImGui::TextDisabled("| %s",
				std::filesystem::path(props.path).extension().string().c_str());

			ImGui::Spacing();
			ImGui::Separator();
		}

		float button_width = ImGui::GetContentRegionAvail().x * 0.48f;

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.25f, 0.29f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.67f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

		// Load Texture Button
		if (ImGui::Button("Load Texture", ImVec2(button_width, 0))) {
			std::string selected_path = FileDialogs::open_file(
				"Image Files\0*.png;*.jpg;*.jpeg;*.bmp;*.tga\0All Files\0*.*\0"
			);

			if (!selected_path.empty()) {
				try {
					props.path = selected_path;
					props.texture = NodeHelper::load_texture(props.path);
					if (entity.has_component<Render2D_Component>())
					{
						auto& render = entity.get_component<Render2D_Component>();
						render.size = props.texture->get_size();
					}
				}
				catch (const std::exception& e)
				{
					props.texture.reset();
					props.path.clear();
				}
			}
		}

		ImGui::SameLine();

		if (!props.path.empty())
		{
			if (ImGui::Button("Reload", ImVec2(button_width, 0))) {
				try {
					props.texture = NodeHelper::load_texture(props.path);
				}
				catch (const std::exception& e) {
					props.texture.reset();
				}
			}
		}
		else
		{
			if (ImGui::Button("Clear", ImVec2(button_width, 0))) {
				props.texture.reset();
				props.path.clear();
			}
		}

		ImGui::PopStyleColor(3);
		ImGui::Spacing();

		// --- Drag and Drop Support ---
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
				const char* dropped_path = (const char*)payload->Data;

				std::string extension = std::filesystem::path(dropped_path).extension().string();
				std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

				if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
					extension == ".bmp" || extension == ".tga") {
					props.path = dropped_path;
					props.texture = NodeHelper::load_texture(props.path);
				}
			}
			ImGui::EndDragDropTarget();
		}
	}

	void draw_animation(Entity entity)
	{
		// To show Frame Selector
		static bool show_frame_selector = false;
		static std::string current_anim_for_frames = "";
		static std::vector<int> selected_frames;
		static int grid_columns = 4;
		static int grid_rows = 4;
		static float cell_width = 100.0f;
		static float cell_height = 100.0f;
		static Texture_Component* texture_for_selection = nullptr;

		std::string anim_to_delete;
		std::pair<std::string, std::string> anim_to_rename;

		auto& anim = entity.get_component<Animation_Component>();

		ImGui::SeparatorText("Current Animation");

		UI::draw_bool("Playing", anim.playing);
		ImGui::SameLine();

		// Restart Button
		if (ImGui::Button("Restart"))
		{
			anim.timer = 0.0f;
			anim.current_frame = 0;
			anim.current_animation_completed = false;
		}
		ImGui::Dummy(ImVec2(0.0f, 2.0f));

		//To show frame
		ImGui::Text("Frame: %d / %s",
			anim.current_frame,
			anim.current_animation.empty() ? "N/A" :
			std::to_string(anim.animations[anim.current_animation].frames.size()).c_str());


		ImGui::Text("Timer: %.2fs", anim.timer);
		ImGui::Text("Completed: %s", anim.current_animation_completed ? "Yes" : "No");

		ImGui::Dummy(ImVec2(0.0f, 2.0f));
		ImGui::SeparatorText("Animation Selection");

		ImGui::Dummy(ImVec2(0.0f, 2.0f));
		// To Show Current Animation
		if (ImGui::BeginCombo("Current Animation",
			anim.current_animation.empty() ? "None" : anim.current_animation.c_str()))
		{
			if (ImGui::Selectable("None", anim.current_animation.empty()))
			{
				anim.current_animation = "";
				anim.current_frame = 0;
				anim.timer = 0.0f;
			}

			for (const auto& [name, _] : anim.animations)
			{
				bool is_selected = (anim.current_animation == name);
				if (ImGui::Selectable(name.c_str(), is_selected))
				{
					anim.current_animation = name;
					anim.current_frame = 0;
					anim.timer = 0.0f;
					anim.current_animation_completed = false;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}


		ImGui::SeparatorText("Animation Management");

		ImGui::Dummy(ImVec2(0.0f, 2.0f));
		// Animation Add Button
		static char new_anim_name[128] = "";
		ImGui::InputTextWithHint("##NewAnimName", "New Animation Name", new_anim_name, IM_ARRAYSIZE(new_anim_name));
		ImGui::SameLine();
		if (ImGui::Button("Add") && strlen(new_anim_name) > 0)
		{
			if (anim.animations.find(new_anim_name) == anim.animations.end())
			{
				Animation new_anim;
				new_anim.name = new_anim_name;
				anim.animations[new_anim_name] = new_anim;
				new_anim_name[0] = '\0';
			}
			else
			{
				ImGui::OpenPopup("Animation Exists");
			}
		}

		ImGui::Dummy(ImVec2(0.0f, 2.0f));
		// If Animation Exists
		if (ImGui::BeginPopupModal("Animation Exists", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("An animation with that name already exists!");
			if (ImGui::Button("OK"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}



		//To see How many Animation are there
		ImGui::Text("Animations (%d):", (int)anim.animations.size());


		ImGui::Dummy(ImVec2(0.0f, 2.0f));

		std::vector<std::string> animation_names;
		animation_names.reserve(anim.animations.size());
		for (const auto& [name, _] : anim.animations)
			animation_names.push_back(name);

		for (const auto& name : animation_names)
		{
			if (anim.animations.find(name) == anim.animations.end())
				continue;

			Animation& animation = anim.animations[name];
			ImGui::PushID(name.c_str());

			float available_width = ImGui::GetContentRegionAvail().x;
			float start_x = ImGui::GetCursorPosX();

			bool anim_open = ImGui::TreeNodeEx(name.c_str(),
				ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap);

			float button_width = 26.0f;
			float button_x = start_x + available_width - button_width - ImGui::GetStyle().FramePadding.x * 2;

			// Save cursor position
			float saved_cursor_y = ImGui::GetCursorPosY();
			float saved_cursor_x = ImGui::GetCursorPosX();

			ImGui::SetCursorPosX(button_x);
			ImGui::SetCursorPosY(saved_cursor_y - ImGui::GetFrameHeight() - ImGui::GetStyle().ItemSpacing.y);
			ImGui::Dummy(ImVec2(button_width * 2.0f, 28));

			ImGui::SetCursorPosX(button_x);
			ImGui::SetCursorPosY(saved_cursor_y - ImGui::GetFrameHeight() - ImGui::GetStyle().ItemSpacing.y);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.25f, 0.29f, 1.00f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.67f));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));

			if (ImGui::Button(" X ", ImVec2(button_width * 2.0f, 28)))
			{
				ImGui::OpenPopup("AnimationRemove##Component");
			}



			ImGui::Dummy(ImVec2(0.0f, 0.0f));
			ImGui::PopStyleVar();
			ImGui::PopStyleColor(3);

			// Restore cursor position for tree content
			ImGui::SetCursorPosY(saved_cursor_y);
			ImGui::SetCursorPosX(saved_cursor_x);

			if (ImGui::BeginPopup("AnimationRemove##Component"))
			{
				ImGui::Text("Remove Animation!");
				ImGui::Dummy(ImVec2(0, 3));
				ImGui::Text("%s", name.c_str());
				ImGui::Separator();

				if (ImGui::Button("Yes", ImVec2(50, 0)))
				{
					anim_to_delete = name;
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (ImGui::Button("No", ImVec2(50, 0)))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}


			if (anim_open)
			{
				std::string original_name = name;
				std::string display_name = animation.name;

				if (UI::draw_string("Name", display_name))
				{
					if (!display_name.empty() && display_name != original_name)
					{
						if (anim.animations.find(display_name) == anim.animations.end())
						{
							animation.name = display_name;
							anim.animations[display_name] = animation;
							if (anim.current_animation == original_name)
							{
								anim.current_animation = display_name;
							}
							anim_to_rename = { original_name, display_name };

						}
					}
				}
				ImGui::Dummy(ImVec2(0.0f, 2.0f));
				ImGui::Checkbox("Loop", &animation.loop);
				ImGui::SameLine();
				ImGui::Checkbox("Ping Pong", &animation.ping_pong);
				ImGui::Dummy(ImVec2(0.0f, 2.0f));
				UI::draw_value("FPS", animation.fps);
				ImGui::Dummy(ImVec2(0.0f, 2.0f));
				ImGui::Separator();
				ImGui::Text("Frames (%d):", (int)animation.frames.size());
				ImGui::Dummy(ImVec2(0.0f, 2.0f));
				ImGui::BeginGroup();

				if (ImGui::Button("Select Frames from Image"))
				{
					if (entity.has_component<Texture_Component>())
					{
						auto& texture_comp = entity.get_component<Texture_Component>();
						if (texture_comp.texture)
						{
							current_anim_for_frames = name;
							texture_for_selection = &texture_comp;
							selected_frames.clear();
							show_frame_selector = true;
						}
						else
						{
							ImGui::OpenPopup("No Texture##FrameSelector");
						}
					}
					else
					{
						ImGui::OpenPopup("No Texture##FrameSelector");
					}
				}

				ImGui::SameLine();
				if (ImGui::Button("Add Blank Frame"))
				{
					Frame new_frame;
					new_frame.frame_rect = { 0, 0, 100, 100 };
					animation.frames.push_back(new_frame);
				}

				ImGui::SameLine();
				if (ImGui::Button("Clear All"))
				{
					animation.frames.clear();
				}


				ImGui::EndGroup();

				if (ImGui::BeginPopup("No Texture##FrameSelector"))
				{
					ImGui::Text("Entity needs a Texture Component with a loaded texture!");
					if (ImGui::Button("OK"))
					{
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}



				if (!animation.frames.empty())
				{
					if (ImGui::Button("View Frames"))
					{
						ImGui::OpenPopup(("Frames_" + name + "_Popup").c_str());
					}

					bool has_texture = entity.has_component<Texture_Component>();
					Texture_Component* texture_comp = nullptr;
					AG_uint texture_id = 0;
					if (has_texture)
					{
						texture_comp = &entity.get_component<Texture_Component>();
						if (texture_comp->texture)
						{
							texture_id = texture_comp->texture->get_texture_id();
						}
						else
						{
							has_texture = false;
						}
					}

					if (ImGui::BeginPopupModal(("Frames_" + name + "_Popup").c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::Text("Frames for animation: %s", name.c_str());
						ImGui::Separator();

						float popup_width = ImGui::GetContentRegionAvail().x;
						float frame_size = 80.0f;
						float spacing = 5.0f;
						int columns = std::max(1, (int)(popup_width / (frame_size + spacing)));

						for (int i = 0; i < animation.frames.size(); i++)
						{
							ImGui::PushID(i);
							Frame& frame = animation.frames[i];
							bool is_current = (anim.current_animation == name && anim.current_frame == i);

							if (is_current)
							{
								ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
								ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 3.0f);
							}

							if (has_texture && texture_id != 0)
							{
								auto tex_size = texture_comp->texture->get_size();
								float tex_w = tex_size.x;
								float tex_h = tex_size.y;

								ImVec2 uv0(frame.frame_rect.position.x / tex_w, frame.frame_rect.position.y / tex_h);
								ImVec2 uv1((frame.frame_rect.position.x + frame.frame_rect.size.x) / tex_w,
									(frame.frame_rect.position.y + frame.frame_rect.size.y) / tex_h);

								if (ImGui::ImageButton(("Frame_" + std::to_string(i)).c_str(),
									(ImTextureID)(intptr_t)texture_id,
									ImVec2(frame_size, frame_size),
									uv0, uv1))
								{
									anim.current_frame = i;
									anim.timer = 0.0f;
								}
							}
							else
							{
								if (ImGui::Button(std::to_string(i).c_str(), ImVec2(frame_size, frame_size)))
								{
									anim.current_frame = i;
									anim.timer = 0.0f;
								}
							}

							if (is_current)
							{
								ImGui::PopStyleVar();
								ImGui::PopStyleColor();
							}

							ImGui::PopID();

							if ((i + 1) % columns != 0)
								ImGui::SameLine();
						}

						ImGui::Separator();
						if (ImGui::Button("Close"))
							ImGui::CloseCurrentPopup();

						ImGui::EndPopup();
					}


				}


				if (!animation.frames.empty())
				{
					float frame_duration = 1.0f / animation.fps;
					float total_duration = animation.frames.size() * frame_duration;
					ImGui::Text("Frame Duration: %.3fs | Total: %.2fs", frame_duration, total_duration);
				}

				ImGui::TreePop();
			}
			ImGui::PopID();
		}


		if (!anim_to_rename.first.empty())
		{
			auto node = anim.animations.extract(anim_to_rename.first);
			node.key() = anim_to_rename.second;
			anim.animations.insert(std::move(node));

			if (anim.current_animation == anim_to_rename.first)
				anim.current_animation = anim_to_rename.second;

			return;
		}

		if (!anim_to_delete.empty())
		{
			anim.animations.erase(anim_to_delete);

			if (anim.current_animation == anim_to_delete)
				anim.current_animation.clear();

			return;
		}









		if (show_frame_selector && texture_for_selection)
		{
			ImGui::OpenPopup("Select Frames from Texture");

			UI::custom_popup("Select Frames from Texture", "Frame Selection",
				[&]()
				{
					auto texture_size = texture_for_selection->texture->get_size();
					float texture_width = texture_size.x;
					float texture_height = texture_size.y;
					ImTextureID texture_id = (ImTextureID)(intptr_t)texture_for_selection->texture->get_texture_id();

					float available_width = ImGui::GetContentRegionAvail().x;
					float left_width = available_width * 0.65f - 5.0f;
					float right_width = available_width * 0.35f - 5.0f;


					ImGui::BeginChild("TextureColumn", ImVec2(left_width, 0), true);
					{
						ImGui::Text("Texture Preview");
						ImGui::Separator();


						static float zoom_level = 1.0f;
						static ImVec2 texture_offset = ImVec2(0, 0);
						static bool is_panning = false;
						static ImVec2 pan_start_mouse_pos;
						static ImVec2 pan_start_offset;

						/*ImGui::BeginChild("TextureColumn", ImVec2(left_width, 50), true);
						ImGui::Text("Zoom: %.0f%%", zoom_level * 100.0f);
						ImGui::SameLine();
						ImGui::SetWindowFontScale(1.5f);
						if (ImGui::Button("-", ImVec2(30, 30))) zoom_level = std::max(0.1f, zoom_level - 0.1f);
						ImGui::SameLine();
						if (ImGui::Button("+", ImVec2(30, 30))) zoom_level = std::min(5.0f, zoom_level + 0.1f);
						ImGui::SetWindowFontScale(1.0f);
						ImGui::SameLine();
						if (ImGui::Button("Reset", ImVec2(60, 30)))
						{
							zoom_level = 1.0f;
							texture_offset = ImVec2(0, 0);
						}
						ImGui::EndChild();*/


						ImGui::BeginChild("TextureContainer", ImVec2(0, 0), false);
						{
							float max_display_width = left_width - 20.0f;
							float max_display_height = ImGui::GetContentRegionAvail().y - 50.0f;

							// Apply zoom to texture dimensions
							float zoomed_texture_width = texture_width * zoom_level;
							float zoomed_texture_height = texture_height * zoom_level;

							float display_width = zoomed_texture_width;
							float display_height = zoomed_texture_height;
							float scale = zoom_level;

							if (zoomed_texture_width > max_display_width || zoomed_texture_height > max_display_height)
							{
								display_width = std::min(zoomed_texture_width, max_display_width);
								display_height = std::min(zoomed_texture_height, max_display_height);
							}

							float cell_display_width = cell_width * zoom_level;
							float cell_display_height = cell_height * zoom_level;



							ImVec2 container_start_pos = ImGui::GetCursorScreenPos();
							float container_width = ImGui::GetContentRegionAvail().x;
							float container_height = ImGui::GetContentRegionAvail().y;

							bool is_hovering_container = ImGui::IsMouseHoveringRect(
								container_start_pos,
								ImVec2(container_start_pos.x + container_width,
									container_start_pos.y + container_height)
							);

							if (is_hovering_container && ImGui::IsMouseClicked(1))
							{
								is_panning = true;
								pan_start_mouse_pos = ImGui::GetMousePos();
								pan_start_offset = texture_offset;
							}

							if (is_panning)
							{
								if (ImGui::IsMouseDown(1))
								{
									ImVec2 mouse_delta = { ImGui::GetMousePos().x - pan_start_mouse_pos.x, ImGui::GetMousePos().y - pan_start_mouse_pos.y };
									texture_offset.x = pan_start_offset.x + mouse_delta.x;
									texture_offset.y = pan_start_offset.y + mouse_delta.y;

									float max_offset_x = std::max(0.0f, zoomed_texture_width - max_display_width);
									float max_offset_y = std::max(0.0f, zoomed_texture_height - max_display_height);
									texture_offset.x = std::clamp(texture_offset.x, -max_offset_x, max_offset_x);
									texture_offset.y = std::clamp(texture_offset.y, -max_offset_y, max_offset_y);

									ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
								}
								else
								{
									is_panning = false;
								}
							}

							if (is_hovering_container && ImGui::GetIO().MouseWheel != 0)
							{
								float zoom_speed = 0.1f;
								float old_zoom = zoom_level;


								zoom_level += ImGui::GetIO().MouseWheel * zoom_speed;
								zoom_level = std::clamp(zoom_level, 0.1f, 5.0f);

								if (zoomed_texture_width > max_display_width || zoomed_texture_height > max_display_height)
								{
									ImVec2 mouse_pos_rel = { ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x,
																					 ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y };
									float zoom_factor = zoom_level / old_zoom;

									texture_offset.x = texture_offset.x * zoom_factor + mouse_pos_rel.x * (1 - zoom_factor);
									texture_offset.y = texture_offset.y * zoom_factor + mouse_pos_rel.y * (1 - zoom_factor);
								}
							}

							float actual_max_display_width = container_width;
							float actual_max_display_height = container_height;

							float texture_x = (container_width - std::min(zoomed_texture_width, actual_max_display_width)) * 0.5f;
							float texture_y = (container_height - std::min(zoomed_texture_height, actual_max_display_height)) * 0.5f;

							ImGui::SetCursorPosX(texture_x);
							ImGui::SetCursorPosY(texture_y);

							ImVec2 texture_start_pos = ImGui::GetCursorScreenPos();

							// Calculate visible portion of texture
							ImVec2 visible_min = ImVec2(
								std::max(0.0f, -texture_offset.x),
								std::max(0.0f, -texture_offset.y)
							);
							ImVec2 visible_max = ImVec2(
								std::min(zoomed_texture_width, -texture_offset.x + actual_max_display_width),
								std::min(zoomed_texture_height, -texture_offset.y + actual_max_display_height)
							);

							// Calculate UV coordinates for visible portion
							ImVec2 uv0 = ImVec2(
								visible_min.x / zoomed_texture_width,
								visible_min.y / zoomed_texture_height
							);
							ImVec2 uv1 = ImVec2(
								visible_max.x / zoomed_texture_width,
								visible_max.y / zoomed_texture_height
							);

							// Draw the visible portion of texture
							ImGui::Image(texture_id,
								ImVec2(visible_max.x - visible_min.x, visible_max.y - visible_min.y),
								uv0, uv1);

							ImVec2 texture_pos = ImGui::GetItemRectMin();

							// Draw grid overlay
							ImDrawList* draw_list = ImGui::GetWindowDrawList();



							if (zoom_level > 0.6f)
							{
								int start_col = std::max(0, (int)(-texture_offset.x / cell_display_width));
								int end_col = std::min(grid_columns,
									(int)((-texture_offset.x + actual_max_display_width) / cell_display_width) + 1);

								for (int col = start_col; col <= end_col; col++)
								{
									float x = texture_start_pos.x + texture_offset.x + col * cell_display_width;
									if (x >= texture_start_pos.x && x <= texture_start_pos.x + actual_max_display_width)
									{
										float line_start_y = texture_start_pos.y + texture_offset.y;
										float line_end_y = line_start_y + zoomed_texture_height;

										// Clip to visible area
										line_start_y = std::max(line_start_y, texture_start_pos.y);
										line_end_y = std::min(line_end_y, texture_start_pos.y + actual_max_display_height);

										draw_list->AddLine(
											ImVec2(x, line_start_y),
											ImVec2(x, line_end_y),
											IM_COL32(255, 255, 255, 150),
											std::max(1.0f, 2.0f / zoom_level) // Thinner lines when zoomed out
										);
									}
								}


								int start_row = std::max(0, (int)(-texture_offset.y / cell_display_height));
								int end_row = std::min(grid_rows,
									(int)((-texture_offset.y + actual_max_display_height) / cell_display_height) + 1);

								for (int row = start_row; row <= end_row; row++)
								{
									float y = texture_start_pos.y + texture_offset.y + row * cell_display_height;
									if (y >= texture_start_pos.y && y <= texture_start_pos.y + actual_max_display_height)
									{
										float line_start_x = texture_start_pos.x + texture_offset.x;
										float line_end_x = line_start_x + zoomed_texture_width;

										line_start_x = std::max(line_start_x, texture_start_pos.x);
										line_end_x = std::min(line_end_x, texture_start_pos.x + actual_max_display_width);

										draw_list->AddLine(
											ImVec2(line_start_x, y),
											ImVec2(line_end_x, y),
											IM_COL32(255, 255, 255, 150),
											std::max(1.0f, 2.0f / zoom_level)
										);
									}
								}

								for (int row = start_row; row < end_row; row++)
								{
									for (int col = start_col; col < end_col; col++)
									{
										int cell_index = row * grid_columns + col;
										ImVec2 cell_min = ImVec2(
											texture_start_pos.x + texture_offset.x + col * cell_display_width,
											texture_start_pos.y + texture_offset.y + row * cell_display_height
										);
										ImVec2 cell_max = ImVec2(
											cell_min.x + cell_display_width,
											cell_min.y + cell_display_height
										);

										bool is_selected = std::find(selected_frames.begin(), selected_frames.end(), cell_index) != selected_frames.end();

										if (is_selected)
										{
											draw_list->AddRectFilled(cell_min, cell_max, IM_COL32(0, 255, 0, 80));
											draw_list->AddRect(cell_min, cell_max, IM_COL32(0, 255, 0, 200), 0.0f, 0, 3.0f);
										}

										ImGui::PushID(cell_index);
										ImGui::SetCursorScreenPos(cell_min);
										if (ImGui::InvisibleButton("##Cell", ImVec2(cell_display_width, cell_display_height)))
										{
											bool ctrl_down = ImGui::GetIO().KeyCtrl;
											bool shift_down = ImGui::GetIO().KeyShift;

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
												int last_selected = selected_frames.back();
												int start = std::min(last_selected, cell_index);
												int end = std::max(last_selected, cell_index);

												for (int i = start; i <= end; i++)
												{
													if (std::find(selected_frames.begin(), selected_frames.end(), i) == selected_frames.end())
													{
														selected_frames.push_back(i);
													}
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
											ImGui::BeginTooltip();
											ImGui::Text("Cell %d", cell_index);
											ImGui::Text("Grid Position: (%d, %d)", col, row);
											ImGui::Text("Texture Rect:");
											ImGui::Text("  Position: (%.0f, %.0f)", col * cell_width, row * cell_height);
											ImGui::Text("  Size: %.0fx%.0f", cell_width, cell_height);
											ImGui::Text("Zoom: %.0f%%", zoom_level * 100.0f);
											if (is_selected)
											{
												ImGui::TextColored(ImVec4(0, 1, 0, 1), "Selected");
											}
											ImGui::EndTooltip();
										}

										ImGui::PopID();
									}
								}
							}

						}

						ImGui::EndChild();
					}
					ImGui::EndChild();
					ImGui::SameLine(0, 10.0f);



					ImGui::BeginChild("ControlsColumn", ImVec2(right_width, 0), true);
					{
						float available_height = ImGui::GetContentRegionAvail().y;
						ImGui::Text("Frame Selection Controls");
						ImGui::Separator();
						ImGui::Dummy(ImVec2(0, 3));
						// Grid settings
						ImGui::Text("Grid Settings:");
						if (UI::draw_value("Columns", grid_columns, 0, 20))
						{
							grid_columns = std::clamp(grid_columns, 1, 100);
							cell_width = texture_width / grid_columns;
						}
						ImGui::Dummy(ImVec2(0, 3));
						if (UI::draw_value("Rows", grid_rows, 0, 20))
						{
							grid_rows = std::clamp(grid_rows, 1, 100);
							cell_height = texture_height / grid_rows;
						}
						ImGui::Dummy(ImVec2(0, 3));
						// Cell info
						cell_width = texture_width / grid_columns;
						cell_height = texture_height / grid_rows;

						ImGui::Text("Cell Size: %.0f x %.0f", cell_width, cell_height);
						ImGui::Dummy(ImVec2(0, 3));
						ImGui::Text("Total Cells: %d", grid_columns * grid_rows);
						ImGui::Dummy(ImVec2(0, 3));
						if (cell_width < 10 || cell_height < 10)
						{
							ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "Warning: Cells are very small!");
						}

						ImGui::Separator();

						ImGui::Text("Selection:");
						ImGui::Text("Selected: %d/%d frames",
							(int)selected_frames.size(), grid_columns * grid_rows);

						float button_width = ImGui::GetContentRegionAvail().x * 0.5f - 5.0f;
						if (!selected_frames.empty())
						{
							ImGui::Text("Selection Order:");
							std::string selection_display;
							const int max_display = 8;

							for (size_t i = 0; i < std::min(selected_frames.size(), (size_t)max_display); i++)
							{
								if (i > 0) selection_display += " -> ";
								selection_display += std::to_string(selected_frames[i]);
							}

							if (selected_frames.size() > max_display)
							{
								selection_display += " -> ...";
							}

							ImGui::TextWrapped("%s", selection_display.c_str());



							// Selection management buttons
							ImGui::BeginGroup();
							if (ImGui::Button("Select All", ImVec2(button_width, 0)))
							{
								selected_frames.clear();
								for (int i = 0; i < grid_columns * grid_rows; i++)
								{
									selected_frames.push_back(i);
								}
							}
							ImGui::SameLine(0.0f, 5.0f);
							if (ImGui::Button("Clear All", ImVec2(button_width, 0)))
							{
								selected_frames.clear();
							}

							if (ImGui::Button("Reverse Order", ImVec2(button_width, 0)) && selected_frames.size() > 1)
							{
								std::reverse(selected_frames.begin(), selected_frames.end());
							}
							ImGui::SameLine(0.0f, 5.0f);
							if (ImGui::Button("Sort Numerically", ImVec2(button_width, 0)))
							{
								std::sort(selected_frames.begin(), selected_frames.end());
							}
							ImGui::EndGroup();









							ImGui::Separator();
							ImGui::Text("Quick Select:");

							if (ImGui::Button("First Row", ImVec2(button_width, 0)))
							{
								selected_frames.clear();
								for (int col = 0; col < grid_columns; col++)
								{
									selected_frames.push_back(col);
								}
							}
							ImGui::SameLine(0.0f, 5.0f);
							if (ImGui::Button("First Column", ImVec2(button_width, 0)))
							{
								selected_frames.clear();
								for (int row = 0; row < grid_rows; row++)
								{
									selected_frames.push_back(row * grid_columns);
								}
							}

							if (ImGui::Button("Diagonal Right", ImVec2(button_width, 0)))
							{
								selected_frames.clear();
								int cells = std::min(grid_columns, grid_rows);
								for (int i = 0; i < cells; i++)
								{
									selected_frames.push_back(i * grid_columns + i);
								}
							}
							ImGui::SameLine(0.0f, 5.0f);
							if (ImGui::Button("Diagonal Left", ImVec2(button_width, 0)))
							{
								selected_frames.clear();
								int cells = std::min(grid_columns, grid_rows);
								for (int i = cells - 1; i >= 0; i--)
								{
									selected_frames.push_back(i * grid_columns + (cells - 1 - i));
								}
							}
						}
						else
						{
							ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "No frames selected");
							ImGui::TextWrapped("Click on cells in the texture to select frames");
							ImGui::TextWrapped("• Click: Select single");
							ImGui::TextWrapped("• Ctrl+Click: Toggle selection");
							ImGui::TextWrapped("• Shift+Click: Select range");
						}

						ImGui::Separator();


						float button_height = 35.0f;
						ImGui::SetCursorPosY(available_height - button_height);

						if (ImGui::Button("Add Frames", ImVec2(button_width, button_height)) && !selected_frames.empty())
						{
							auto it = anim.animations.find(current_anim_for_frames);
							if (it != anim.animations.end())
							{
								Animation& target_anim = it->second;

								for (int cell_index : selected_frames)
								{
									int row = cell_index / grid_columns;
									int col = cell_index % grid_columns;

									Frame new_frame;
									new_frame.frame_rect.position.x = col * cell_width;
									new_frame.frame_rect.position.y = row * cell_height;
									new_frame.frame_rect.size.x = cell_width;
									new_frame.frame_rect.size.y = cell_height;
									target_anim.frames.push_back(new_frame);
								}
								show_frame_selector = false;
								selected_frames.clear();
							}
						}
						ImGui::SameLine(0.0f, 10.0f);
						if (ImGui::Button("Cancel", ImVec2(button_width, button_height)))
						{
							show_frame_selector = false;
							selected_frames.clear();
						}
					}
					ImGui::EndChild();
				}, []() { show_frame_selector = false; selected_frames.clear(); });
		}
	}

	void draw_tilemap_register(Entity entity)
	{
		static bool show_register = false;
		static std::vector<vec2u> selected_tiles;
		static vec2i tile_size = { 32, 32 };
		static vec2f last_mouse_pos;
		static vec2f current_mouse_pos;
		static bool is_dragging = false;


		if (ImGui::Button("Register TileSet"))
		{
			if (!entity.get_component<Texture_Component>().texture)
				return;
			ImGui::OpenPopup("TileSet Register");
			show_register = true;
			selected_tiles.clear();

			if (entity.has_component<TileSet_Component>())
			{
				auto& tile_set = entity.get_component<TileSet_Component>();
				for (const auto& [id, def] : tile_set.tile_definitions)
				{
					selected_tiles.push_back(id);
				}
			}
		}

		if (show_register)
		{
			custom_popup("TileSet Register", "TileSet Register",
				[&]() mutable
				{
					auto& texture = entity.get_component<Texture_Component>().texture;
					vec2f texture_size = texture->get_size();
					ImTextureID texture_id = (ImTextureID)(intptr_t)texture->get_texture_id();

					float available_width = ImGui::GetContentRegionAvail().x;
					float left_width = available_width * 0.65f - 5.0f;
					float right_width = available_width * 0.35f - 5.0f;

					ImGui::BeginChild("TextureColumn", ImVec2(left_width, 0), true);
					{
						ImGui::Text("Texture Preview");
						ImGui::Separator();

						ImGui::BeginChild("TextureContainer", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
						{
							static vec2f view_size = ImGui::GetContentRegionAvail();
							static vec2f view_center;

							vec2f mouse_screen = ImGui::GetMousePos();
							vec2f child_pos = ImGui::GetWindowPos();
							vec2f child_size = ImGui::GetWindowSize();

							current_mouse_pos = mouse_screen - child_pos;


							vec2f container = ImGui::GetContentRegionAvail();
							vec2f container_start_pos = ImGui::GetCursorScreenPos();
							bool is_hovering_container = ImGui::IsMouseHoveringRect(
								container_start_pos.to_imvec2(),
								(container_start_pos + container).to_imvec2());

							if (is_hovering_container && ImGui::IsMouseClicked(2))
							{
								is_dragging = true;
								last_mouse_pos = current_mouse_pos;
							}
							if (is_dragging)
							{
								if (ImGui::IsMouseDown(2))
								{
									vec2f start = Math::screen_to_world(last_mouse_pos, Math::get_float_rect(view_size, view_center), container);
									vec2f end = Math::screen_to_world(current_mouse_pos, Math::get_float_rect(view_size, view_center), container);
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
								vec2f world_before_zoom = Math::screen_to_world(current_mouse_pos, Math::get_float_rect(view_size, view_center), container);
								view_size *= scale_factor;
								vec2f world_after_zoom = Math::screen_to_world(current_mouse_pos, Math::get_float_rect(view_size, view_center), container);
								vec2f offset = world_before_zoom - world_after_zoom;
								view_center += offset;
							}
							vec2f world_pos = vec2f(0, 0) - texture_size * 0.5f;
							vec2f screen_pos = Math::world_to_screen(world_pos, Math::get_float_rect(view_size, view_center), container);
							vec2f screen_size = Math::world_size_to_screen_size(texture_size, view_size, container);
							ImGui::SetCursorPos(screen_pos.to_imvec2());
							ImGui::Image(texture_id, screen_size.to_imvec2());
							ImDrawList* draw_list = ImGui::GetWindowDrawList();


							vec2f window_pos = ImGui::GetWindowPos();
							screen_pos += window_pos;
							vec2i total_dim = texture_size / tile_size;
							for (int x = 0; x <= total_dim.x; x++)
							{// Vertical
								float px = screen_pos.x + x * tile_size.x * (screen_size.x / texture_size.x);
								draw_list->AddLine(ImVec2(px, screen_pos.y),
									ImVec2(px, screen_pos.y + screen_size.y),
									IM_COL32(255, 255, 255, 100), 2.0f);
							}

							for (int y = 0; y <= total_dim.y; y++)
							{// Horizontal
								float py = screen_pos.y + y * tile_size.y * (screen_size.y / texture_size.y);
								draw_list->AddLine(
									ImVec2(screen_pos.x, py),
									ImVec2(screen_pos.x + screen_size.x, py),
									IM_COL32(255, 255, 255, 100), 2.0f);
							}

							vec2f button_size = Math::world_size_to_screen_size(tile_size, view_size, container);
							for (int x = 0; x < total_dim.x; x++)
							{
								for (int y = 0; y < total_dim.y; y++)
								{
									vec2u tile_id = { (AG_uint)x, (AG_uint)y };

									bool is_selected = std::find(selected_tiles.begin(),
										selected_tiles.end(),
										tile_id) != selected_tiles.end();

									vec2f scale = screen_size / texture_size;
									vec2f cell_min = screen_pos + vec2f(x * tile_size.x, y * tile_size.y) * scale;
									vec2f cell_max = screen_pos + vec2f((x + 1) * tile_size.x, (y + 1) * tile_size.y) * scale;

									ImU32 color = is_selected ? IM_COL32(200, 200, 200, 50)
										: IM_COL32(0, 0, 0, 150);

									draw_list->AddRectFilled(cell_min.to_imvec2(), cell_max.to_imvec2(), color);

									ImGui::PushID(("tile_" + std::to_string(x) + "_" + std::to_string(y)).c_str());
									ImGui::SetCursorScreenPos(cell_min.to_imvec2());

									if (ImGui::InvisibleButton("##Cell", button_size.to_imvec2()))
									{
										bool control = ImGui::GetIO().KeyCtrl;
										bool shift = ImGui::GetIO().KeyShift;

										if (control)
										{
											auto it = std::find(selected_tiles.begin(),
												selected_tiles.end(),
												tile_id);
											if (it != selected_tiles.end())
												selected_tiles.erase(it);
											else
												selected_tiles.push_back(tile_id);
										}
										else if (shift && !selected_tiles.empty())
										{
											vec2u last_selected = selected_tiles.back();
											int start_x = std::min(last_selected.x, tile_id.x);
											int end_x = std::max(last_selected.x, tile_id.x);
											int start_y = std::min(last_selected.y, tile_id.y);
											int end_y = std::max(last_selected.y, tile_id.y);

											for (int y = start_y; y <= end_y; y++)
											{
												for (int x = start_x; x <= end_x; x++)
												{
													vec2u id = { (AG_uint)x, (AG_uint)y };
													if (std::find(selected_tiles.begin(),
														selected_tiles.end(),
														id) == selected_tiles.end())
													{
														selected_tiles.push_back(id);
													}
												}
											}
										}
										else
										{
											selected_tiles.clear();
											selected_tiles.push_back(tile_id);
										}
									}

									if (ImGui::IsItemHovered())
									{
										ImGui::BeginTooltip();
										ImGui::Text("Tile (%d, %d)", x, y);
										ImGui::Text("Position: (%d, %d)",
											x * (int)tile_size.x,
											y * (int)tile_size.y);
										ImGui::Text("Size: %.0fx%.0f", tile_size.x, tile_size.y);
										if (is_selected)
											ImGui::TextColored(ImVec4(0, 1, 0, 1), "Selected");
										ImGui::EndTooltip();
									}
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

						float button_height = 35.0f;
						float button_width = ImGui::GetContentRegionAvail().x * 0.5f - 5.0f;
						ImGui::SetCursorPosY(available_height - button_height);

						if (ImGui::Button("Register Tiles", ImVec2(button_width, button_height)) && !selected_tiles.empty())
						{
							if (!entity.has_component<TileSet_Component>())
							{
								entity.add_component<TileSet_Component>();
							}
							auto& tile_set = entity.get_component<TileSet_Component>();
							vec2u size = tile_size;
							tile_set.tile_definitions.clear();
							for (const auto& ids : selected_tiles)
							{
								Tile_Defination def;

								def.texture_rect = { ids * size, size };
								def.is_solid = false;
								tile_set.tile_definitions[ids] = def;
							}
							if (!selected_tiles.empty())
							{
								tile_set.is_tile_registered = true;
								tile_set.tile_changed = true;
							}
								
							show_register = false;
							selected_tiles.clear();
							ImGui::CloseCurrentPopup();
						}
						ImGui::SameLine(0.0f, 10.0f);
						if (ImGui::Button("Cancel", ImVec2(button_width, button_height)))
						{
							show_register = false;
							selected_tiles.clear();
							ImGui::CloseCurrentPopup();
						}
					}
					ImGui::EndChild();
				},
				[]()
				{
					show_register = false;
					selected_tiles.clear();
					ImGui::CloseCurrentPopup();
				});
		}
	}

	bool draw_tilemap_selector(Entity entity, vec2u& id)
	{
		static bool window_open = true;
		static std::vector<vec2u> registered_tiles;
		static vec2f current_mouse_pos;
		static vec2f last_mouse_pos;
		static Entity e;
		static bool is_dragging = false;
		static vec2f tile_size = { 32, 32 };
		static bool selected = false;
		if (entity.has_component<TileSet_Component>())
		{
			auto& tile_set = entity.get_component<TileSet_Component>();
			if (!tile_set.is_tile_registered)
			{
				return false;
			}
			if (tile_set.tile_changed)
			{
				registered_tiles.clear();
				selected = false;
				const auto& tile_def = tile_set.tile_definitions;
				for (const auto& [id, def] : tile_def)
				{
					registered_tiles.push_back(id);
				}
				tile_set.tile_changed = false;
				AERO_CORE_INFO("Tile Changed");
			}
			
		}
		if (!entity.has_component<Tile_Component>() || !entity.has_component<Texture_Component>() || !entity.get_component<Texture_Component>().texture)
			return false;
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoCollapse;

		if (ImGui::Begin("TileSet Selector", &window_open, flags))
		{
			auto& texture = entity.get_component<Texture_Component>().texture;
			vec2f texture_size = texture->get_size();
			ImTextureID texture_id = (ImTextureID)(intptr_t)texture->get_texture_id();


			float available_width = ImGui::GetContentRegionAvail().x;
			ImGui::BeginChild("TextureColumn", ImVec2(0, 0), true);
			{
				ImGui::BeginChild("TextureContainer", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
				{
					static vec2f view_size = ImGui::GetContentRegionAvail();
					static vec2f view_center;
					static vec2f window_size = ImGui::GetWindowSize();
					{// On Resize
						vec2f current_size = ImGui::GetWindowSize();
						if (current_size != window_size)
						{
							vec2f scale = current_size / window_size;
							view_size *= scale;
							window_size = current_size;
						}

					}

					vec2f mouse_screen = ImGui::GetMousePos();
					vec2f child_pos = ImGui::GetWindowPos();
					vec2f child_size = ImGui::GetWindowSize();

					current_mouse_pos = mouse_screen - child_pos;


					vec2f container = ImGui::GetContentRegionAvail();
					vec2f container_start_pos = ImGui::GetCursorScreenPos();
					bool is_hovering_container = ImGui::IsMouseHoveringRect(
						container_start_pos.to_imvec2(),
						(container_start_pos + container).to_imvec2());

					if (is_hovering_container && ImGui::IsMouseClicked(2))
					{
						is_dragging = true;
						last_mouse_pos = current_mouse_pos;
					}
					if (is_dragging)
					{
						if (ImGui::IsMouseDown(2))
						{
							vec2f start = Math::screen_to_world(last_mouse_pos, Math::get_float_rect(view_size, view_center), container);
							vec2f end = Math::screen_to_world(current_mouse_pos, Math::get_float_rect(view_size, view_center), container);
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
						vec2f world_before_zoom = Math::screen_to_world(current_mouse_pos, Math::get_float_rect(view_size, view_center), container);
						view_size *= scale_factor;
						vec2f world_after_zoom = Math::screen_to_world(current_mouse_pos, Math::get_float_rect(view_size, view_center), container);
						vec2f offset = world_before_zoom - world_after_zoom;
						view_center += offset;
					}
					vec2f world_pos = vec2f(0, 0) - texture_size * 0.5f;
					vec2f screen_pos = Math::world_to_screen(world_pos, Math::get_float_rect(view_size, view_center), container);
					vec2f screen_size = Math::world_size_to_screen_size(texture_size, view_size, container);
					ImGui::SetCursorPos(screen_pos.to_imvec2());
					ImGui::Image(texture_id, screen_size.to_imvec2());
					ImDrawList* draw_list = ImGui::GetWindowDrawList();


					vec2f window_pos = ImGui::GetWindowPos();
					screen_pos += window_pos;
					vec2i total_dim = texture_size / tile_size;
					for (int x = 0; x <= total_dim.x; x++)
					{// Vertical
						float px = screen_pos.x + x * tile_size.x * (screen_size.x / texture_size.x);
						draw_list->AddLine(ImVec2(px, screen_pos.y),
							ImVec2(px, screen_pos.y + screen_size.y),
							IM_COL32(255, 255, 255, 100), 2.0f);
					}

					for (int y = 0; y <= total_dim.y; y++)
					{// Horizontal
						float py = screen_pos.y + y * tile_size.y * (screen_size.y / texture_size.y);
						draw_list->AddLine(
							ImVec2(screen_pos.x, py),
							ImVec2(screen_pos.x + screen_size.x, py),
							IM_COL32(255, 255, 255, 100), 2.0f);
					}

					vec2f button_size = Math::world_size_to_screen_size(tile_size, view_size, container);
					for (int x = 0; x < total_dim.x; x++)
					{
						for (int y = 0; y < total_dim.y; y++)
						{
							vec2u tile_id = { (AG_uint)x, (AG_uint)y };

							bool is_registered = std::find(registered_tiles.begin(),
								registered_tiles.end(),
								tile_id) != registered_tiles.end();

							bool selected = (tile_id == id) && is_registered;

							vec2f scale = screen_size / texture_size;
							vec2f cell_min = screen_pos + vec2f(x * tile_size.x, y * tile_size.y) * scale;
							vec2f cell_max = screen_pos + vec2f((x + 1) * tile_size.x, (y + 1) * tile_size.y) * scale;

							ImU32 color;

							if (!is_registered)
								color = IM_COL32(0, 0, 0, 150);
							else if (selected)
								color = IM_COL32(100, 0, 0, 150);
							else
								color = IM_COL32(100, 100, 100, 80);

							draw_list->AddRectFilled(cell_min.to_imvec2(), cell_max.to_imvec2(), color);

							if (is_registered)
							{
								ImGui::PushID(("tile_" + std::to_string(x) + "_" + std::to_string(y)).c_str());
								ImGui::SetCursorScreenPos(cell_min.to_imvec2());

								if (ImGui::InvisibleButton("##Cell", button_size.to_imvec2()))
								{
									id = tile_id;
									selected = true;
								}

								if (ImGui::IsItemHovered())
								{
									ImGui::BeginTooltip();
									ImGui::Text("Tile (%d, %d)", x, y);
									ImGui::Text("Position: (%d, %d)",
										x * (int)tile_size.x,
										y * (int)tile_size.y);
									ImGui::Text("Size: %.0fx%.0f", tile_size.x, tile_size.y);
									if (is_registered)
										ImGui::TextColored(ImVec4(0, 1, 0, 1), "Registered");
									ImGui::EndTooltip();
								}
								ImGui::PopID();
							}


						}
					}

					ImGui::EndChild();
				}
				ImGui::EndChild();
			}

		}
		ImGui::End();
		return selected;

	}

	bool texture_selector(Entity entity, uint_rect& texture_rect)
	{
		static bool window_open = true;
		static vec2f current_mouse_pos;
		static vec2f last_mouse_pos;
		static vec2f starting_pos;
		static vec2f end_pos;
		static bool is_dragging = false;

		static bool selected = false;
		static bool selection_finished = false;
		static vec2f tile_size = { 16.0f, 16.0f };
		static bool is_selecting = false;

		if (!entity.has_component<TextureRect_Component>() || !entity.has_component<Texture_Component>() || !entity.get_component<Texture_Component>().texture)
			return false;

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoCollapse;

		if (ImGui::Begin("Texture Selector", &window_open, flags))
		{
			auto& texture = entity.get_component<Texture_Component>().texture;
			vec2f texture_size = texture->get_size();
			ImTextureID texture_id = (ImTextureID)(intptr_t)texture->get_texture_id();


			float available_width = ImGui::GetContentRegionAvail().x;
			ImGui::BeginChild("TextureColumn", ImVec2(0, 0), true);
			{
				ImGui::BeginChild("TextureContainer", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
				{
					static vec2f view_size = ImGui::GetContentRegionAvail();
					static vec2f view_center;
					static vec2f window_size = ImGui::GetWindowSize();
					{// On Resize
						vec2f current_size = ImGui::GetWindowSize();
						if (current_size != window_size)
						{
							vec2f scale = current_size / window_size;
							view_size *= scale;
							window_size = current_size;
						}
					}


					vec2f mouse_screen = ImGui::GetMousePos();
					vec2f child_pos = ImGui::GetWindowPos();
					vec2f child_size = ImGui::GetWindowSize();

					current_mouse_pos = mouse_screen - child_pos;


					vec2f container = ImGui::GetContentRegionAvail();
					vec2f container_start_pos = ImGui::GetCursorScreenPos();
					bool is_hovering_container = ImGui::IsMouseHoveringRect(
						container_start_pos.to_imvec2(),
						(container_start_pos + container).to_imvec2());

					if (is_hovering_container && ImGui::IsMouseClicked(2))
					{
						is_dragging = true;
						last_mouse_pos = current_mouse_pos;
					}
					if (is_dragging)
					{
						if (ImGui::IsMouseDown(2))
						{
							vec2f start = Math::screen_to_world(last_mouse_pos, Math::get_float_rect(view_size, view_center), container);
							vec2f end = Math::screen_to_world(current_mouse_pos, Math::get_float_rect(view_size, view_center), container);
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
						vec2f world_before_zoom = Math::screen_to_world(current_mouse_pos, Math::get_float_rect(view_size, view_center), container);
						view_size *= scale_factor;
						vec2f world_after_zoom = Math::screen_to_world(current_mouse_pos, Math::get_float_rect(view_size, view_center), container);
						vec2f offset = world_before_zoom - world_after_zoom;
						view_center += offset;
					}




					vec2f image_pos = vec2f(0, 0) - texture_size * 0.5f;
					vec2f screen_pos = Math::world_to_screen(image_pos, Math::get_float_rect(view_size, view_center), container);
					vec2f screen_size = Math::world_size_to_screen_size(texture_size, view_size, container);
					ImGui::SetCursorPos(screen_pos.to_imvec2());
					ImGui::Image(texture_id, screen_size.to_imvec2());
					bool is_hovered = ImGui::IsItemHovered();
					ImDrawList* draw_list = ImGui::GetWindowDrawList();

					vec2f window_pos = ImGui::GetWindowPos();
					screen_pos += window_pos;
					vec2i total_dim = texture_size / tile_size;
					for (int x = 0; x <= total_dim.x; x++)
					{// Vertical
						float px = screen_pos.x + x * tile_size.x * (screen_size.x / texture_size.x);
						draw_list->AddLine(ImVec2(px, screen_pos.y),
							ImVec2(px, screen_pos.y + screen_size.y),
							IM_COL32(255, 255, 255, 100), 2.0f);
					}

					for (int y = 0; y <= total_dim.y; y++)
					{// Horizontal
						float py = screen_pos.y + y * tile_size.y * (screen_size.y / texture_size.y);
						draw_list->AddLine(
							ImVec2(screen_pos.x, py),
							ImVec2(screen_pos.x + screen_size.x, py),
							IM_COL32(255, 255, 255, 100), 2.0f);
					}

					if (is_hovered && ImGui::IsMouseClicked(0))
					{
						is_selecting = true;
						selection_finished = false;
						vec2f world_pos = Math::screen_to_world(current_mouse_pos, Math::get_float_rect(view_size, view_center), container);
						AERO_CORE_INFO("World Position");
						world_pos.print();
						vec2f local = world_pos - image_pos;
						AERO_CORE_INFO("Local Position");
						local.print();
						vec2f image_size_pixel = texture_size;
						AERO_CORE_INFO("Image Size in Pixel");
						image_size_pixel.print();
						vec2f pixel_world_size = screen_size / image_size_pixel;
						AERO_CORE_INFO("Pixel World Size");
						pixel_world_size.print();


						local.x = std::floor(local.x / pixel_world_size.x) * pixel_world_size.x;
						local.y = std::floor(local.y / pixel_world_size.y) * pixel_world_size.y;
						AERO_CORE_INFO("Local Position");
						local.print();
						starting_pos = image_pos + local;
						AERO_CORE_INFO("Starting Position");
						starting_pos.print();
					}

					if (is_hovered && ImGui::IsMouseDown(0))
					{
						vec2f current = current_mouse_pos;

						vec2f start = Math::world_to_screen(starting_pos, Math::get_float_rect(view_size, view_center), container);
						draw_list->AddRectFilled(
							(window_pos + start).to_imvec2(),
							(window_pos + current).to_imvec2(),
							IM_COL32(255, 255, 255, 100)
						);
					}
					else if (!ImGui::IsMouseDown(0))
					{
						if (is_selecting)
							end_pos = Math::screen_to_world(current_mouse_pos, Math::get_float_rect(view_size, view_center), container);

						is_selecting = false;

						vec2f start = Math::world_to_screen(starting_pos, Math::get_float_rect(view_size, view_center), container);
						vec2f end = Math::world_to_screen(end_pos, Math::get_float_rect(view_size, view_center), container);
						draw_list->AddRectFilled(
							(window_pos + start).to_imvec2(),
							(window_pos + end).to_imvec2(),
							IM_COL32(255, 255, 255, 100)
						);
					}

					ImGui::EndChild();
				}
				ImGui::EndChild();
			}

		}
		ImGui::End();
		return selected;

	}



	void custom_popup(const std::string& popup_id, const std::string& popup_name, std::function<void()> draw_content, std::function<void()> close)
	{
		ImGuiWindowFlags flag = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove;

		ImVec4 original_dim_color = ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg];

		ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);
		vec2f window_size = Application::get().get_window().get_size();
		ImVec2 modal_size(window_size.x * 0.7f, window_size.y * 0.8f);


		ImVec2 center = ImGui::GetMainViewport()->GetCenter();


		ImGui::SetNextWindowPos(ImVec2(center.x - modal_size.x * 0.5f,
			center.y - modal_size.y * 0.5f),
			ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(modal_size, ImGuiCond_Appearing);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0.0f, 0.0f, 0.0f, 0.85f));

		if (ImGui::BeginPopupModal(popup_id.c_str(), NULL, flag))
		{
			ImVec2 window_pos = ImGui::GetWindowPos();
			ImVec2 window_size = ImGui::GetWindowSize();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			float title_bar_height = 40.0f;
			ImU32 title_bar_color = IM_COL32(50, 50, 50, 255);

			draw_list->AddRectFilled(
				window_pos,
				ImVec2(window_pos.x + window_size.x, window_pos.y + title_bar_height),
				title_bar_color
			);
			ImGui::SetWindowFontScale(1.2f);
			ImGui::SetCursorPos(ImVec2(10, (title_bar_height - ImGui::GetFontSize()) * 0.5f));
			ImGui::TextColored(ImVec4(1, 1, 1, 1), popup_name.c_str());

			float close_button_size = title_bar_height - 10;
			ImGui::SetCursorPos(ImVec2(
				window_size.x - close_button_size - 5,
				(title_bar_height - close_button_size) * 0.5f
			));


			ImGui::SetCursorPos(ImVec2(window_size.x - 60, 0));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));

			if (ImGui::Button(" X ", ImVec2(60, 40)))
			{
				close();
			}
			ImGui::SetWindowFontScale(1.0f);
			ImGui::PopStyleColor(3);

			ImGui::SetCursorPosY(title_bar_height + 5);
			ImGui::Dummy(ImVec2(0.0f, 0.0f));

			draw_content();

			ImGui::EndPopup();
		}

		ImGui::PopStyleColor(2);

		ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg] = original_dim_color;
	}


	// Helper functions
	void run_current_scene()
	{
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