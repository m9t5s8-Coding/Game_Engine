#include <UI/UI.hpp>
#include <Aero.hpp>
#include <Application/EditorLayer.hpp>
#include <Panels/ScenePanel.hpp>
#include <windows.h> 
#include <shellapi.h>

namespace ag::UI
{
	static const uint16_t bit_lookup[3][3] =
	{
			{ TL,  T,  TR },
			{ L,   M,  R  },
			{ BL,  B,  BR }
	};

	void draw_menu_bar()
	{
		if (!ImGui::BeginMainMenuBar()) return;


		// File Menu
		if (ImGui::BeginMenu("File")) {
			// New Scene
			if (ImGui::MenuItem("New Scene", "Ctrl+N"))
			{
				EditorLayer::get().create_new_scene();
			}

			// Open Scene
			if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
			{
				EditorLayer::get().open_scene();
			}

			// Recent Scenes submenu
			if (ImGui::BeginMenu("Recent Scenes")) {

				ImGui::EndMenu();
			}

			ImGui::Separator();

			// Save Scene
			if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
			{
				EditorLayer::get().save_scene();
			}

			// Save Scene As
			if (ImGui::MenuItem("Save Scene As Default", "Ctrl+Shift+S"))
			{
				//EditorLayer::get().save_scene_as_default();
			}

			// Save All
			if (ImGui::MenuItem("Save All", "Ctrl+Alt+S"))
			{
				// EditorLayer::get().save_all_scene();
			}

			ImGui::Separator();

			// Project submenu
			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("New Project..."))
				{
				}
				if (ImGui::MenuItem("Open Project..."))
				{
				}
				if (ImGui::MenuItem("Project Settings"))
				{
					//state.show_project_settings = true;
				}
				ImGui::EndMenu();
			}

			ImGui::Separator();

			ImGui::Separator();

			// Exit
			if (ImGui::MenuItem("Exit", "Alt+F4"))
			{
				//EditorLayer::get().try_exit();
			}

			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu("Edit")) {
			//bool can_undo = EditorLayer::get().can_undo();
			//bool can_redo = EditorLayer::get().can_redo();
			bool can_redo = false;
			bool can_undo = false;

			if (ImGui::MenuItem("Undo", "Ctrl+Z", false, can_undo))
			{
				//EditorLayer::get().undo();
			}

			if (ImGui::MenuItem("Redo", "Ctrl+Y", false, can_redo))
			{
				//EditorLayer::get().redo();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Cut", "Ctrl+X"))
			{
				//EditorLayer::get().cut_selection();
			}

			if (ImGui::MenuItem("Copy", "Ctrl+C"))
			{
				//EditorLayer::get().copy_selection();
			}

			if (ImGui::MenuItem("Paste", "Ctrl+V"))
			{
				//EditorLayer::get().paste();
			}

			if (ImGui::MenuItem("Duplicate", "Ctrl+D"))
			{
				//EditorLayer::get().duplicate_selection();
			}

			if (ImGui::MenuItem("Delete", "Del"))
			{
				//EditorLayer::get().delete_selection();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Select All", "Ctrl+A"))
			{
				//EditorLayer::get().select_all();
			}

			if (ImGui::MenuItem("Deselect All", "Ctrl+Shift+A"))
			{
				//EditorLayer::get().deselect_all();
			}


			ImGui::Separator();

			if (ImGui::MenuItem("Editor Settings"))
			{
				//state.show_editor_settings = true;
			}

			if (ImGui::MenuItem("Preferences", "Ctrl+,"))
			{
				//EditorLayer::get().show_preferences();
			}

			ImGui::EndMenu();
		}

		// View Menu
		if (ImGui::BeginMenu("View"))
		{

			if (ImGui::MenuItem("Scene", nullptr, &show_panels.scene_panel))
			{
				//EditorLayer::get().toggle_window("Scene", show_scene);
			}

			if (ImGui::MenuItem("Properties", nullptr, &show_panels.properties_panel))
			{
				//EditorLayer::get().toggle_window("Inspector", show_inspector);
			}

			if (ImGui::MenuItem("Console", nullptr, &show_panels.console_panel))
			{
				//EditorLayer::get().toggle_window("Console", show_console);
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Layouts")) {
				if (ImGui::MenuItem("Default Layout"))
				{
					//EditorLayer::get().load_layout("default");
				}

				if (ImGui::MenuItem("Programming Layout"))
				{
					//EditorLayer::get().load_layout("programming");
				}

				if (ImGui::MenuItem("Design Layout"))
				{
					//EditorLayer::get().load_layout("design");
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save Current Layout..."))
				{
					//EditorLayer::get().save_current_layout();
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Fullscreen", "F11"))
			{
				//EditorLayer::get().toggle_fullscreen();
			}

			ImGui::EndMenu();
		}

		// Create Menu
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::BeginMenu("2D Nodes"))
			{
				if (ImGui::MenuItem("Scene2D"))
				{
					//EditorLayer::get().create_scene2d();
				}
				if (ImGui::MenuItem("Rectangle"))
				{
					//EditorLayer::get().create_rectangle();
				}
				if (ImGui::MenuItem("Circle"))
				{
					//EditorLayer::get().create_circle();
				}
				if (ImGui::MenuItem("Sprite"))
				{
					//EditorLayer::get().create_sprite();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		// Run Menu
		if (ImGui::BeginMenu("Run"))
		{
			if (ImGui::MenuItem("Run Default Scene", "F5", false))
			{
				auto folder = FileDialogs::get_exe_folder();
				std::wstring app = folder + L"\\Sandbox.exe";

				FileDialogs::run_exe(app);
			}

			if (ImGui::MenuItem("Run Current Scene", "Ctrl+F5", false))
			{
				run_current_scene();
				auto folder = FileDialogs::get_exe_folder();
				std::wstring app = folder + L"\\Sandbox.exe";

				FileDialogs::run_exe(app);
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window")) {
			if (ImGui::MenuItem("Minimize", "Ctrl+M"))
			{
				//EditorLayer::get().minimize_window();
			}

			if (ImGui::MenuItem("Maximize", "Ctrl+Shift+M"))
			{
				//EditorLayer::get().maximize_window();
			}
			ImGui::EndMenu();
		}

		// Help Menu
		if (ImGui::BeginMenu("Help")) {
			if (ImGui::MenuItem("Documentation", "F1"))
			{
				//EditorLayer::get().open_documentation();
			}

			if (ImGui::MenuItem("Keyboard Shortcuts"))
			{
				//state.show_shortcuts_dialog = true;
			}
			ImGui::Separator();

			if (ImGui::MenuItem("Check for Updates"))
			{
				//EditorLayer::get().check_for_updates();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("About"))
			{
				//state.show_about_dialog = true;
			}

			ImGui::EndMenu();
		}

		// Status info on the right
		ImGui::SameLine(ImGui::GetWindowWidth() - 300);
		// Show FPS
		ImGui::SameLine();
		ImGui::TextDisabled("FPS: %.1f", ImGui::GetIO().Framerate);

		ImGui::EndMainMenuBar();

	}

	void draw_texture(Entity entity)
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
			width = props.texture->get_width();
			height = props.texture->get_height();
		}
		else
		{
			width = 128.0f;
			height = 64.0f;
		}
		float aspect = width / height;
		ImVec2 preview_size(128.0f, 128.0f);
		if (aspect > 1.0f)
			preview_size.y = 128.0f / aspect;
		else
			preview_size.x = 128.0f * aspect;

		const float padding = 10.0f;
		ImVec2 frame_size = ImVec2(preview_size.x + padding * 2,
			preview_size.y + padding * 2);

		float available_width = ImGui::GetContentRegionAvail().x;
		float frame_pos_x = (available_width - frame_size.x) / 2.0f;

		ImGui::BeginChild("TexturePreviewFrame",
			ImVec2(0, frame_size.y + 5),
			false,
			ImGuiWindowFlags_NoScrollbar);
		{
			ImGui::SetCursorPosX(frame_pos_x);

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			ImVec2 frame_min = ImGui::GetCursorScreenPos();
			ImVec2 frame_max = ImVec2(frame_min.x + frame_size.x,
				frame_min.y + frame_size.y);


			draw_list->AddRectFilled(frame_min, frame_max,
				ImGui::GetColorU32(ImGuiCol_FrameBg),
				4.0f);

			draw_list->AddRect(frame_min, frame_max,
				ImGui::GetColorU32(ImGuiCol_Border),
				4.0f, 0, 1.5f);

			ImVec2 image_min = ImVec2(frame_min.x + padding, frame_min.y + padding);
			ImVec2 image_max = ImVec2(image_min.x + preview_size.x, image_min.y + preview_size.y);

			ImGui::SetCursorScreenPos(image_min);

			ImGui::InvisibleButton("##TextureDropTarget", preview_size);

			if (ImGui::BeginDragDropTarget())
			{
				if (ImGui::IsMouseHoveringRect(image_min, image_max))
				{
					draw_list->AddRect(image_min, image_max,
						ImGui::GetColorU32(ImVec4(0.26f, 0.59f, 0.98f, 0.67f)),
						4.0f, 0, 2.0f);

					const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");
					if (payload)
					{
						const char* dropped_path = (const char*)payload->Data;

						std::string extension = std::filesystem::path(dropped_path).extension().string();
						std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

						if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
							extension == ".bmp" || extension == ".tga" || extension == ".hdr")
						{
							props.path = dropped_path;
							try
							{
								props.path = dropped_path;
								props.texture = NodeHelper::load_texture(props.path);
							}
							catch (const std::exception& e)
							{
								props.texture.reset();
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
							props.path = dropped_path;
							try
							{
								props.texture = NodeHelper::load_texture(props.path);
							}
							catch (const std::exception& e)
							{
								props.texture.reset();
							}
						}
					}

					ImGui::EndDragDropTarget();
				}
			}



			if (props.texture)
			{
				ImGui::SetCursorScreenPos(image_min);
				ImGui::Image((void*)(intptr_t)props.texture->get_texture_id(),
					preview_size);

				if (!props.texture && ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("Drop texture here");
				}
			}
			else
			{
				// Draw a placeholder when no texture is loaded
				ImGui::SetCursorScreenPos(image_min);

				// Draw a checkerboard pattern as placeholder
				const float checker_size = 16.0f;
				const ImU32 col1 = ImGui::GetColorU32(ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
				const ImU32 col2 = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

				for (float y = 0; y < preview_size.y; y += checker_size)
				{
					for (float x = 0; x < preview_size.x; x += checker_size)
					{
						bool is_dark = ((int)(x / checker_size) + (int)(y / checker_size)) % 2 == 0;
						draw_list->AddRectFilled(
							ImVec2(image_min.x + x, image_min.y + y),
							ImVec2(image_min.x + x + checker_size, image_min.y + y + checker_size),
							is_dark ? col1 : col2
						);
					}
				}

				// Draw a centered "+" icon or text
				const char* drop_text = "Drop Texture Here";
				ImVec2 text_size = ImGui::CalcTextSize(drop_text);
				ImVec2 text_pos = ImVec2(
					image_min.x + (preview_size.x - text_size.x) * 0.5f,
					image_min.y + (preview_size.y - text_size.y) * 0.5f
				);

				// Draw text with shadow for better visibility
				draw_list->AddText(ImVec2(text_pos.x + 1, text_pos.y + 1),
					ImGui::GetColorU32(ImVec4(0, 0, 0, 0.5f)), drop_text);
				draw_list->AddText(text_pos, ImGui::GetColorU32(ImVec4(1, 1, 1, 0.8f)), drop_text);
			}

		}
		ImGui::EndChild();
		if (props.texture)
		{
			ImGui::Text("Size: %dx%d",
				props.texture->get_width(),
				props.texture->get_height());

			ImGui::SameLine();
			ImGui::TextDisabled("| %s",
				std::filesystem::path(props.path).extension().string().c_str());
		}
		else if (!props.path.empty())
		{
			ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "Failed to load: %s",
				std::filesystem::path(props.path).filename().string().c_str());
		}
		else
		{
			ImGui::TextDisabled("No texture loaded");
		}
		ImGui::Spacing();
		ImGui::Separator();

		float button_width = ImGui::GetContentRegionAvail().x * 0.48f;

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.25f, 0.29f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.67f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

		if (ImGui::Button("Load Texture", ImVec2(button_width, 0)))
		{
			EditorLayer::get().load_texture(entity);
		}

		ImGui::SameLine();

		if (!props.path.empty())
		{
			if (ImGui::Button("Reload", ImVec2(button_width, 0))) {
				try
				{
					props.texture = NodeHelper::load_texture(props.path);
				}
				catch (const std::exception& e)
				{
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



	}

	void content_browser()
	{
		auto project = Project::get_active_project();
		std::filesystem::path root = project->get_directory();

		ImGui::Begin("Content Browser");

		draw_folder_node(root);

		ImGui::End();
	}

	bool is_right_file(const std::filesystem::path& path)
	{
		std::string ext = path.extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		return ext == ".png" || ext == ".jpg" || ext == ".jpeg" ||
			ext == ".bmp" || ext == ".tga" || ext == ".hdr" ||
			ext == ".tiff" || ext == ".tif" || ext == ".webp" || ext == ".aeroscene" || ext == ".lua";
	}

	bool is_image(const std::filesystem::path& path)
	{
		std::string ext = path.extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		return ext == ".png" || ext == ".jpg" || ext == ".jpeg" ||
			ext == ".bmp" || ext == ".tga" || ext == ".hdr" ||
			ext == ".tiff" || ext == ".tif" || ext == ".webp";
	}

	void draw_folder_node(const std::filesystem::path& directory)
	{
		for (const auto& entry : std::filesystem::directory_iterator(directory))
		{
			const auto& path = entry.path();
			std::string name = path.filename().string();

			ImGui::PushID(path.string().c_str());

			if (entry.is_directory())
			{
				ImGuiTreeNodeFlags flags =
					ImGuiTreeNodeFlags_OpenOnArrow |
					ImGuiTreeNodeFlags_SpanAvailWidth;

				bool open = ImGui::TreeNodeEx(
					(std::string(ICON_FOLDER) + " " + name).c_str(),
					flags
				);

				// Right-click folder
				if (ImGui::BeginPopupContextItem())
				{
					ImGui::MenuItem("Open");
					ImGui::MenuItem("Rename");
					ImGui::MenuItem("Delete");
					ImGui::EndPopup();
				}

				if (open)
				{
					draw_folder_node(path);   // recursion
					ImGui::TreePop();
				}
			}
			else if (is_right_file(path))
			{
				ImGui::Selectable((std::string(ICON_FOLDER) + " " + name).c_str());

				// Right-click file
				if (ImGui::BeginPopupContextItem())
				{
					ImGui::MenuItem("Open");
					ImGui::MenuItem("Delete");
					ImGui::EndPopup();
				}

				// Drag & drop
				if (ImGui::BeginDragDropSource())
				{
					std::string p = path.string();
					ImGui::SetDragDropPayload(
						"CONTENT_BROWSER_ITEM",
						p.c_str(),
						p.size() + 1
					);
					ImGui::Text("%s", name.c_str());
					ImGui::EndDragDropSource();
				}
			}

			ImGui::PopID();
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
		static std::vector<vec2u> solid_tiles;
		static vec2i tile_size = { 32, 32 };
		static vec2f last_mouse_pos;
		static vec2f current_mouse_pos;
		static bool is_dragging = false;
		static bool is_for_solid = false;


		if (ImGui::Button("Register TileSet"))
		{
			if (!entity.get_component<Texture_Component>().texture)
				return;
			ImGui::OpenPopup("TileSet Register");
			show_register = true;
			selected_tiles.clear();
			solid_tiles.clear();
			is_for_solid = false;

			if (entity.has_component<TileSet_Component>())
			{
				auto& tile_set = entity.get_component<TileSet_Component>();
				for (const auto& [id, def] : tile_set.tile_definitions)
				{
					selected_tiles.push_back(id);
					if (def.is_solid)
					{
						solid_tiles.push_back(id);
					}
				}
				if (tile_set.tile_size != 0)
				{
					tile_size = tile_set.tile_size;
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

									bool is_solid = std::find(solid_tiles.begin(), solid_tiles.end(), tile_id) != solid_tiles.end() && is_selected;

									vec2f scale = screen_size / texture_size;
									vec2f cell_min = screen_pos + vec2f(x * tile_size.x, y * tile_size.y) * scale;
									vec2f cell_max = screen_pos + vec2f((x + 1) * tile_size.x, (y + 1) * tile_size.y) * scale;

									ImU32 selected_color = is_selected ? IM_COL32(200, 200, 200, 50)
										: IM_COL32(0, 0, 0, 150);


									ImU32 solid_color = is_solid ? IM_COL32(200, 50, 50, 100)
										: IM_COL32(0, 0, 0, 150);

									draw_list->AddRectFilled(cell_min.to_imvec2(), cell_max.to_imvec2(), selected_color);

									if (is_for_solid)
										draw_list->AddRectFilled(cell_min.to_imvec2(), cell_max.to_imvec2(), solid_color);

									ImGui::PushID(("tile_" + std::to_string(x) + "_" + std::to_string(y)).c_str());
									ImGui::SetCursorScreenPos(cell_min.to_imvec2());

									if (ImGui::InvisibleButton("##Cell", button_size.to_imvec2()))
									{
										bool control = ImGui::GetIO().KeyCtrl;
										bool shift = ImGui::GetIO().KeyShift;

										if (control)
										{
											if (!is_for_solid)
											{
												auto it = std::find(selected_tiles.begin(),
													selected_tiles.end(),
													tile_id);
												if (it != selected_tiles.end())
												{
													selected_tiles.erase(it);
												}
												else
												{
													selected_tiles.push_back(tile_id);
												}
											}
											else
											{
												auto selected = std::find(
													selected_tiles.begin(),
													selected_tiles.end(),
													tile_id
												);
												if (selected != selected_tiles.end())
												{
													auto it = std::find(
														solid_tiles.begin(),
														solid_tiles.end(),
														tile_id
													);

													if (it != solid_tiles.end())
													{
														solid_tiles.erase(it);
													}
													else
													{
														solid_tiles.push_back(tile_id);
													}
												}

											}
										}
										else if ((shift && !selected_tiles.empty() && !is_for_solid) || (shift && !solid_tiles.empty() && is_for_solid))
										{
											if (!is_for_solid)
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
												vec2u last_selected = solid_tiles.back();
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
															id) != selected_tiles.end())
														{
															if (std::find(solid_tiles.begin(),
																solid_tiles.end(),
																id) == solid_tiles.end())
															{
																solid_tiles.push_back(id);
															}
														}
													}
												}
											}
										}
										else
										{
											if (!is_for_solid)
											{
												selected_tiles.clear();
												selected_tiles.push_back(tile_id);
											}
											else
											{
												if (std::find(selected_tiles.begin(),
													selected_tiles.end(),
													tile_id) != selected_tiles.end())
												{
													solid_tiles.clear();
													solid_tiles.push_back(tile_id);
												}
											}
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
										if (!is_for_solid)
										{
											if (is_selected)
												ImGui::TextColored(ImVec4(0, 1, 0, 1), "Selected");
										}
										else
										{
											if (is_solid)
												ImGui::TextColored(ImVec4(0, 1, 0, 1), "Solid Tile");
										}
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
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
						std::string switch_label;
						{
							if (!is_for_solid)
							{
								switch_label = "Solid Tile View";
							}
							else
							{
								switch_label = "Tile Register View";
							}
						}
						float button_height = 35.0f;
						if (ImGui::Button(switch_label.c_str(), ImVec2(0, button_height)))
						{
							is_for_solid = !is_for_solid;
						}


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

								def.is_solid = std::find(solid_tiles.begin(),
									solid_tiles.end(),
									ids) != solid_tiles.end();
								tile_set.tile_definitions[ids] = def;

								tile_set.is_tile_registered = true;
							}
							tile_set.tile_size = tile_size;
							show_register = false;
							tile_set.tile_changed = true;
							selected_tiles.clear();
							solid_tiles.clear();
							ImGui::CloseCurrentPopup();
						}
						ImGui::SameLine(0.0f, 10.0f);
						if (ImGui::Button("Cancel", ImVec2(button_width, button_height)))
						{
							show_register = false;
							selected_tiles.clear();
							solid_tiles.clear();
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
					ImGui::CloseCurrentPopup();
				});
		}
	}




	void draw_autotiling_register(Entity entity)
	{
		static bool show_register = false;
		static std::string active_set;
		

		static char buffer[128] = "";
		buffer[sizeof(buffer) - 1] = '\0';

		ImGui::InputTextWithHint("##NewSetName", "New Set Name", buffer, sizeof(buffer));
		ImGui::SameLine();
		if (ImGui::Button("Add") && buffer[0] != '\0')
		{
			auto& comps = entity.get_component<AutoTiling_Component>();
			std::string name = buffer;
			if (comps.auto_tiles.find(name) == comps.auto_tiles.end())
			{
				Auto_Tiles tiles;
				comps.next_id++;
				tiles.set_id = comps.next_id;
				comps.auto_tiles[name] = tiles;
				buffer[0] = '\0';
			}
			else
			{
				ImGui::OpenPopup("Set Name Exists");
			}
		}
		{

			if (ImGui::BeginCombo("Current Set",
				active_set.empty() ? "None" : active_set.c_str()))
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

		static std::vector<vec2u> selected_tiles;
		static std::unordered_map<vec2u, uint16_t, vec2_hash<AG_uint>> temp_bitmask;
		static vec2i tile_size = { 32, 32 };
		static vec2f last_mouse_pos;
		static vec2f current_mouse_pos;
		static bool is_dragging = false;
		static std::string current_set_name = "";
		static std::string set_name = "";
		static bool creating_new_set = false;
		static bool editing_existing_set = false;

		float total_width = ImGui::GetContentRegionAvail().x;
		float spacing = 10.0f;
		float width = (total_width - 5.0f) * 0.5f;
		if (ImGui::Button("Edit Set", ImVec2(width, 30.0f)))
		{
			if (!entity.has_component<Texture_Component>() || !entity.get_component<Texture_Component>().texture ||
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
				auto it = comps.auto_tiles.find(active_set);
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
			auto it = comps.auto_tiles.find(active_set);
			if (it != comps.auto_tiles.end())
			{
				comps.auto_tiles.erase(it);
				active_set = "";
			}
		}


		if (show_register)
		{
			custom_popup("AutoTiling Register", "AutoTiling Register",
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

									uint16_t mask = 0;
									auto it = temp_bitmask.find(tile_id);
									if (it != temp_bitmask.end())
										mask = it->second;


									vec2f scale = screen_size / texture_size;
									vec2f cell_min = screen_pos + vec2f(x * tile_size.x, y * tile_size.y) * scale;
									vec2f cell_max = screen_pos + vec2f((x + 1) * tile_size.x, (y + 1) * tile_size.y) * scale;

									ImU32 selected_color = is_selected ? IM_COL32(200, 200, 200, 50)
										: IM_COL32(0, 0, 0, 150);


									draw_list->AddRectFilled(cell_min.to_imvec2(), cell_max.to_imvec2(), selected_color);


									ImGui::SetCursorScreenPos(cell_min.to_imvec2());

									vec2f sub_size = (cell_max - cell_min) / 3.0f;
									ImGui::PushID(tile_id.x);
									ImGui::PushID(tile_id.y);
									for (int sy = 0; sy < 3; sy++)
									{
										for (int sx = 0; sx < 3; sx++)
										{


											uint16_t bit = bit_lookup[sy][sx];
											if (bit == 0) continue;


											vec2f sub_min = cell_min + vec2f(sx, sy) * sub_size;
											vec2f sub_max = sub_min + sub_size;

											bool selected = (mask & bit) != 0;

											ImU32 color = selected
												? IM_COL32(255, 0, 0, 180)
												: IM_COL32(50, 50, 50, 0);

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
						if (ImGui::BeginPopupModal("Set Name Exists", NULL, ImGuiWindowFlags_AlwaysAutoResize))
						{
							ImGui::Text("An Set with that name already exists!");
							if (ImGui::Button("OK"))
							{
								ImGui::CloseCurrentPopup();
							}
							ImGui::EndPopup();
						}




						float button_width = ImGui::GetContentRegionAvail().x * 0.5f - 5.0f;
						float button_height = 35.0f;
						ImGui::SetCursorPosY(available_height - button_height);

						if (ImGui::Button("Register Tiles", ImVec2(button_width, button_height)) && !selected_tiles.empty())
						{
							if (!active_set.empty())
							{
								auto& comps = entity.get_component<AutoTiling_Component>();
								auto it = comps.auto_tiles.find(active_set);
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
							current_set_name = "";
							creating_new_set = false;
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
					current_set_name = "";
					creating_new_set = false;
					editing_existing_set = false;
					temp_bitmask.clear();
					ImGui::CloseCurrentPopup();
				});
		}
	}





	bool draw_tilemap_selector(Entity entity, vec2u& id, std::string& set_name, bool& use_autotile)
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
					tile_size = def.texture_rect.size;
				}
				tile_set.tile_changed = false;
				if (tile_set.tile_size != 0)
				{
					tile_size = tile_set.tile_size;
				}
			}

		}
		if (!entity.has_component<Tile_Component>() || !entity.has_component<Texture_Component>() || !entity.get_component<Texture_Component>().texture || !entity.has_component<TileSet_Component>())
		{
			return false;
		}

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
				if(entity.has_component<AutoTiling_Component>())
				{
					ImGui::BeginChild("AutoTileColumn", ImVec2(0, 80), true);
					{
						if (ImGui::BeginCombo("Current Set",
							set_name.empty() ? "None" : set_name.c_str()))
						{
							if (ImGui::Selectable("None", set_name.empty()))
							{
								set_name = "";
							}
							auto& comps = entity.get_component<AutoTiling_Component>();
							for (const auto& [name, _] : comps.auto_tiles)
							{
								bool is_selected = (set_name == name);
								if (ImGui::Selectable(name.c_str(), is_selected))
								{
									set_name = name;
								}
								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
					}
					ImGui::Dummy(ImVec2(0, 3));
					UI::draw_bool("Use Auto Tile", use_autotile);
					ImGui::EndChild();
				}

				ImGui::BeginChild("TextureContainer", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
				{
					static vec2f view_size = ImGui::GetContentRegionAvail();
					static vec2f view_center;
					static vec2f window_size = ImGui::GetWindowSize();

					vec2f current_size = ImGui::GetWindowSize();
					if(current_size.x != 0 && current_size.y != 0)
					{// On Resize

						if (current_size != window_size)
						{
							vec2f scale = current_size / window_size;
							view_size *= scale;
							window_size = current_size;
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
		static vec2f starting_pixel;
		static vec2f ending_pixel;
		static bool is_dragging = false;

		bool selected = false;
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
					static vec2f view_size = ImGui::GetWindowSize();
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

					current_mouse_pos = mouse_screen - child_pos;


					vec2f container = ImGui::GetContentRegionAvail();
					vec2f container_start_pos = ImGui::GetCursorScreenPos();
					bool is_hovering_container = ImGui::IsMouseHoveringRect(
						container_start_pos.to_imvec2(),
						(container_start_pos + container).to_imvec2());

					vec2f image_pos = vec2f(0, 0) - texture_size * 0.5f;
					vec2f padding = { 10.0f, 10.0f };

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
						if (view_size < texture_size)
						{
							vec2f world_after_zoom = Math::screen_to_world(current_mouse_pos, Math::get_float_rect(view_size, view_center), container);
							vec2f offset = world_before_zoom - world_after_zoom;
							view_center += offset;
						}
						else
						{
							view_center = vec2f();
						}
					}

					{
						vec2f half_view = view_size * 0.5f;
						if (view_size.x < texture_size.x + padding.x)
						{
							float min_x = image_pos.x - padding.x + half_view.x;
							float max_x = image_pos.x + texture_size.x + padding.x - half_view.x;
							view_center.x = std::clamp(view_center.x, min_x, max_x);
						}
						else
						{
							view_center.x = image_pos.x + texture_size.x * 0.5f;
						}

						if (view_size.y < texture_size.y + padding.y)
						{
							float min_y = image_pos.y - padding.y + half_view.y;
							float max_y = image_pos.y + texture_size.y + padding.y - half_view.y;
							view_center.y = std::clamp(view_center.y, min_y, max_y);
						}
						else
						{
							view_center.y = image_pos.y + texture_size.y * 0.5f;
						}
					}

					vec2f screen_pos = Math::world_to_screen(image_pos, Math::get_float_rect(view_size, view_center), container);
					vec2f screen_size = Math::world_size_to_screen_size(texture_size, view_size, container);
					ImGui::SetCursorPos(screen_pos.to_imvec2());

					vec2f window_pos = ImGui::GetWindowPos();
					screen_pos += window_pos;
					ImDrawList* draw_list = ImGui::GetWindowDrawList();

					vec2f padding_size = Math::world_size_to_screen_size(padding, view_size, container);
					draw_list->AddRectFilled(
						(screen_pos - padding_size).to_imvec2(),
						(screen_pos + screen_size + padding_size).to_imvec2(),
						IM_COL32(50, 50, 50, 200)
					);

					ImGui::Image(texture_id, screen_size.to_imvec2());
					bool is_hovered = ImGui::IsItemHovered();

					draw_list->AddRectFilled(
						screen_pos.to_imvec2(),
						(screen_pos + screen_size).to_imvec2(),
						IM_COL32(0, 0, 0, 50)
					);

					if (is_hovered && ImGui::IsMouseClicked(0))
					{
						is_selecting = true;
						selection_finished = false;
						vec2f world_pos = Math::screen_to_world(current_mouse_pos, Math::get_float_rect(view_size, view_center), container);
						vec2f local = world_pos - image_pos;
						starting_pixel = local;
						starting_pixel.floor();
					}
					if (is_hovered && ImGui::IsMouseDown(0))
					{
						{
							vec2f world_pos = Math::screen_to_world(current_mouse_pos, Math::get_float_rect(view_size, view_center), container);
							vec2f local = world_pos - image_pos;
							ending_pixel = local;
							ending_pixel.floor();
						}

						vec2f start_pos{
								(std::min(starting_pixel.x, ending_pixel.x)),
								(std::min(starting_pixel.y, ending_pixel.y))
						};
						vec2f end_pos{
								(std::max(starting_pixel.x, ending_pixel.x)) + 1.0f,
								(std::max(starting_pixel.y, ending_pixel.y)) + 1.0f
						};

						vec2f start = Math::world_to_screen(start_pos + image_pos, Math::get_float_rect(view_size, view_center), container);
						vec2f end = Math::world_to_screen(end_pos + image_pos, Math::get_float_rect(view_size, view_center), container);

						vec2f uv0 = starting_pixel / texture_size;
						vec2f uv1 = (ending_pixel + 1.0f) / texture_size;

						vec2f uv_min{
								std::min(uv0.x, uv1.x),
								std::min(uv0.y, uv1.y)
						};
						vec2f uv_max{
								std::max(uv0.x, uv1.x),
								std::max(uv0.y, uv1.y)
						};

						draw_list->AddImage(
							texture_id,
							(window_pos + start).to_imvec2(),
							(window_pos + end).to_imvec2(),
							uv_min.to_imvec2(),
							uv_max.to_imvec2()
						);

						draw_list->AddRect(
							(window_pos + start).to_imvec2(),
							(window_pos + end).to_imvec2(),
							IM_COL32(255, 255, 255, 200)
						);

					}
					else if (!ImGui::IsMouseDown(0) && is_selecting)
					{
						{
							selection_finished = true;
							vec2f world_pos = Math::screen_to_world(current_mouse_pos, Math::get_float_rect(view_size, view_center), container);
							vec2f local = world_pos - image_pos;
							ending_pixel = local;
							ending_pixel.floor();
							is_selecting = false;

							vec2f start_pos{
								(std::min(starting_pixel.x, ending_pixel.x)),
								(std::min(starting_pixel.y, ending_pixel.y))
							};
							vec2f end_pos{
									(std::max(starting_pixel.x, ending_pixel.x)) + 1.0f,
									(std::max(starting_pixel.y, ending_pixel.y)) + 1.0f
							};

							texture_rect.position = start_pos;
							texture_rect.size = end_pos - start_pos;
							selected = true;
						}

					}
					else if (!ImGui::IsMouseDown(0) && selection_finished)
					{
						vec2f start_pos{
								(std::min(starting_pixel.x, ending_pixel.x)),
								(std::min(starting_pixel.y, ending_pixel.y))
						};
						vec2f end_pos{
								(std::max(starting_pixel.x, ending_pixel.x)) + 1.0f,
								(std::max(starting_pixel.y, ending_pixel.y)) + 1.0f
						};

						vec2f start = Math::world_to_screen(start_pos + image_pos, Math::get_float_rect(view_size, view_center), container);
						vec2f end = Math::world_to_screen(end_pos + image_pos, Math::get_float_rect(view_size, view_center), container);

						vec2f uv0 = starting_pixel / texture_size;
						vec2f uv1 = (ending_pixel + 1.0f) / texture_size;

						vec2f uv_min{
								std::min(uv0.x, uv1.x),
								std::min(uv0.y, uv1.y)
						};
						vec2f uv_max{
								std::max(uv0.x, uv1.x),
								std::max(uv0.y, uv1.y)
						};

						draw_list->AddImage(
							texture_id,
							(window_pos + start).to_imvec2(),
							(window_pos + end).to_imvec2(),
							uv_min.to_imvec2(),
							uv_max.to_imvec2()
						);

						draw_list->AddRect(
							(window_pos + start).to_imvec2(),
							(window_pos + end).to_imvec2(),
							IM_COL32(255, 255, 255, 200)
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

	void draw_script_selector(Entity entity)
	{
		if (!entity.has_component<Script_Component>())
			return;

		static bool show_create_model = false;

		auto& props = entity.get_component<Script_Component>();

		if (!props.path.empty())
		{
			ImGui::Text("Script:%s", props.path.c_str());

		}
		ImGui::Dummy(ImVec2(0, 10));
		float button_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2.0f;
		if (ImGui::Button("Create New", ImVec2(button_width, 0)))
		{
			show_create_model = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("Add Existing", ImVec2(button_width, 0)))
		{
			auto full_path = FileDialogs::open_file("Lua Scripts(*.lua)\0 * .lua\0All Files(*.*)\0 * .*\0");
			if (!full_path.empty())
			{
				auto project = Project::get_active_project();
				Helper::normalize_path(full_path);

				std::string project_dir = project->get_directory();
				std::string script_dir = project->get_scripts_directory();

				std::string base_path = project_dir + script_dir + "/";

				std::string relative_path = full_path;
				if (relative_path.find(base_path) == 0)
					relative_path = relative_path.substr(base_path.size());

				Helper::normalize_path(relative_path);

				std::filesystem::path p(full_path);
				std::string script_path = "/" + relative_path;

				if (!entity.has_component<Script_Component>())
				{
					Script_Component comp;
					comp.path = script_path;
					entity.add_component<Script_Component>(comp);
				}
				else
				{
					auto& comp = entity.get_component<Script_Component>();
					comp.path = script_path;
				}
			}
		}

		ImGui::Dummy(ImVec2(0, 5));

		draw_create_script_model(show_create_model, entity);
	}

	void draw_create_script_model(bool& show_model, Entity entity)
	{
		if (!show_model)
			return;
		static std::string new_script_name;

		ImGui::OpenPopup("Create New Script");
		custom_popup("Create New Script", "Create New Script",
			[&]() mutable
			{
				draw_string("Script Name", new_script_name);

				float button_width = (ImGui::GetContentRegionAvail().x - 10.0f) * 0.5f;

				if (ImGui::Button("Create Script"))
				{
					if (!new_script_name.empty())
					{
						auto project = Project::get_active_project();
						std::string script_path = "/" + new_script_name + ".lua";
						std::string full_path = project->get_directory() + project->get_scripts_directory() + script_path;

						std::ofstream file(full_path);
						if (file.is_open())
						{
							file << "--- " << new_script_name << ".lua ---\n\n";
							file << "local entity = get_entity()\n\n";
							file << "function on_create()\n";
							file << "  aero_print(\"Entity Created\")\n";
							file << "end\n\n";
							file << "function on_update(dt)\n";
							file << "  aero_print(\"Entity Updated\")\n";
							file << "end\n\n";
							file << "function on_delete()\n";
							file << "  aero_print(\"Entity Deleted\")\n";
							file << "end\n\n";
							file << "function on_event(event)\n";
							file << "  aero_print(\"Entity Events\")\n";
							file << "end\n\n";
						}
						file.close();
						auto& comps = entity.get_component<Script_Component>();
						comps.path = script_path;
						show_model = false;
					}
				}
				ImGui::SameLine(0, 10.0f);
				if (ImGui::Button("Cancel"))
				{
					show_model = false;
				}
			},

			[&]() mutable
			{
				show_model = false;
			});
	}

	void custom_popup(PopUpModel& model)
	{
		ImGuiWindowFlags flag = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove;

		ImVec4 original_dim_color = ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg];

		ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);



		ImVec2 center = ImGui::GetMainViewport()->GetCenter();


		ImGui::SetNextWindowPos(ImVec2(center.x - model.window_size.x * 0.5f,
			center.y - model.window_size.y * 0.5f),
			ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(model.window_size.to_imvec2(), ImGuiCond_Appearing);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0.0f, 0.0f, 0.0f, 0.85f));

		if (ImGui::BeginPopupModal(model.id.c_str(), NULL, flag))
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
			ImGui::TextColored(ImVec4(1, 1, 1, 1), model.name.c_str());

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
				model.on_close();
			}
			ImGui::SetWindowFontScale(1.0f);
			ImGui::PopStyleColor(3);

			ImGui::SetCursorPosY(title_bar_height + 5);
			ImGui::Dummy(ImVec2(0.0f, 0.0f));

			model.draw_content();

			ImGui::EndPopup();
		}

		ImGui::PopStyleColor(2);

		ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg] = original_dim_color;
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