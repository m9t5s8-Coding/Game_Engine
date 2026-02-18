#include <UI/UI.hpp>
#include <Aero.hpp>
#include <Application/EditorLayer.hpp>
#include <Panels/ScenePanel.hpp>


#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <shellapi.h>
#elif defined(PLATFORM_LINUX)

#endif



#include <UI/PopUp.hpp>
#include <UI/StyleScope.hpp>
#include <icons.h>
#include <codecvt>
#include <locale>

namespace ag
{
	static const uint16_t bit_lookup[3][3] =
	{
			{ TL,  T,  TR },
			{ L,   M,  R  },
			{ BL,  B,  BR }
	};
	Panels UI::s_show_panels{};
	PropertyStyle UI::s_property_style{};
	static const Color folder_color = Color::White;
	static std::string new_name;
	static std::string path;
	constexpr float button_height = 32.0f;

#ifdef PLATFORM_WINDOWS
constexpr const wchar_t* exe_ext = L".exe";
constexpr const wchar_t path_sep = L'\\';
#elif defined(PLATFORM_LINUX)
constexpr const wchar_t* exe_ext = L"";
constexpr const wchar_t path_sep = L'/';
#endif




	void UI::draw_menu_bar()
	{
		if (!ImGui::BeginMainMenuBar()) return;

		if (ImGui::BeginMenu("  File  "))
		{
			if (ImGui::MenuItem(ICON_FA_FILE "  New Scene", "Ctrl+N"))
			{
				UI::get_uistate_panels().create_new_scene = true;
			}

			// Open Scene
			if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN "  Open Scene", "Ctrl+O"))
			{
				EditorLayer::get().open_scene();
			}

			// Recent Scenes submenu
			if (ImGui::BeginMenu(ICON_FA_CLOCK "  Recent Scenes")) {

				ImGui::EndMenu();
			}

			ImGui::Separator();

			// Save Scene
			if (ImGui::MenuItem(ICON_FA_SAVE "  Save Scene", "Ctrl+S"))
			{
				EditorLayer::get().save_scene();
			}

			// Save Scene As
			if (ImGui::MenuItem(ICON_FA_SAVE "  Save Scene As Default", "Ctrl+Shift+S"))
			{
				EditorLayer::get().save_scene_as_default();
			}

			// Save All
			if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK "  Save All", "Ctrl+Alt+S"))
			{
				EditorLayer::get().save_all_scene();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu(ICON_FA_FOLDER "  Project"))
			{
				if (ImGui::MenuItem(ICON_FA_PLUS "  New Project..."))
				{
				}
				if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN "  Open Project..."))
				{
				}
				if (ImGui::MenuItem(ICON_FA_GEAR "  Project Settings"))
				{
					//state.show_project_settings = true;
				}
				ImGui::EndMenu();
			}

			ImGui::Separator();
			if (ImGui::MenuItem(ICON_FA_EXPAND"  Export Game"))
			{
				EditorLayer::get().open_export_panel();
			}
			ImGui::Separator();

			// Exit
			if (ImGui::MenuItem(ICON_FA_POWER_OFF "  Exit", "Alt+F4"))
			{
				EditorLayer::get().try_exit();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("  Edit  "))
		{
			bool can_redo = false;
			bool can_undo = false;

			if (ImGui::MenuItem(ICON_FA_ARROW_ROTATE_LEFT "  Undo", "Ctrl+Z", false, can_undo))
			{
				//EditorLayer::get().undo();
			}

			if (ImGui::MenuItem(ICON_FA_ARROW_ROTATE_RIGHT "  Redo", "Ctrl+Y", false, can_redo))
			{
				//EditorLayer::get().redo();
			}

			ImGui::Separator();

			if (ImGui::MenuItem(ICON_FA_SCISSORS "  Cut", "Ctrl+X"))
			{
				//EditorLayer::get().cut_selection();
			}

			if (ImGui::MenuItem(ICON_FA_COPY "  Copy", "Ctrl+C"))
			{
				//EditorLayer::get().copy_selection();
			}

			if (ImGui::MenuItem(ICON_FA_PASTE "  Paste", "Ctrl+V"))
			{
				//EditorLayer::get().paste();
			}

			if (ImGui::MenuItem(ICON_FA_CLONE "  Duplicate", "Ctrl+D"))
			{
				//EditorLayer::get().duplicate_selection();
			}

			if (ImGui::MenuItem(ICON_FA_TRASH "  Delete", "Del"))
			{
				//EditorLayer::get().delete_selection();
			}

			ImGui::Separator();

			if (ImGui::MenuItem(ICON_FA_CHECK_DOUBLE "  Select All", "Ctrl+A"))
			{
				//EditorLayer::get().select_all();
			}

			if (ImGui::MenuItem(ICON_FA_XMARK "  Deselect All", "Ctrl+Shift+A"))
			{
				//EditorLayer::get().deselect_all();
			}

			ImGui::Separator();

			if (ImGui::MenuItem(ICON_FA_SLIDERS "  Editor Settings"))
			{
				//state.show_editor_settings = true;
			}

			if (ImGui::MenuItem(ICON_FA_GEAR "  Preferences", "Ctrl+,"))
			{
				//EditorLayer::get().show_preferences();
			}

			ImGui::EndMenu();
		}

		// View Menu
		if (ImGui::BeginMenu("  View  "))
		{
			if (ImGui::MenuItem(ICON_FA_IMAGE "  Scene", nullptr, &s_show_panels.scene_panel))
			{
				//EditorLayer::get().toggle_window("Scene", show_scene);
			}

			if (ImGui::MenuItem(ICON_FA_LIST "  Properties", nullptr, &s_show_panels.properties_panel))
			{
				//EditorLayer::get().toggle_window("Inspector", show_inspector);
			}

			if (ImGui::MenuItem(ICON_FA_TERMINAL "  Console", nullptr, &s_show_panels.console_panel))
			{
				//EditorLayer::get().toggle_window("Console", show_console);
			}

			ImGui::Separator();

			if (ImGui::BeginMenu(ICON_FA_LAYER_GROUP "  Layouts")) {
				if (ImGui::MenuItem(ICON_FA_HOUSE "  Default Layout"))
				{
					//EditorLayer::get().load_layout("default");
				}

				if (ImGui::MenuItem(ICON_FA_CODE "  Programming Layout"))
				{
					//EditorLayer::get().load_layout("programming");
				}

				if (ImGui::MenuItem(ICON_FA_PAINTBRUSH "  Design Layout"))
				{
					//EditorLayer::get().load_layout("design");
				}

				ImGui::Separator();

				if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK "  Save Current Layout..."))
				{
					//EditorLayer::get().save_current_layout();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu(ICON_FA_PAINTBRUSH"  Themes"))
			{
				if (ImGui::MenuItem(" Default"))
					ImGui::StyleColorsDark();

				if (ImGui::MenuItem(" CyberPunk"))
					ImGuiLayer::set_cyberpunk_theme();

				if (ImGui::MenuItem(" Warm Amber"))
					ImGuiLayer::set_warm_amber_theme();

				if (ImGui::MenuItem(" Ocean Blue"))
					ImGuiLayer::set_ocean_blue_theme();

				if (ImGui::MenuItem(" Blood Red"))
					ImGuiLayer::set_blood_red_theme();

				if (ImGui::MenuItem(" Monochrome Slate"))
					ImGuiLayer::set_monochrome_slate_theme();

				if (ImGui::MenuItem(" Light Azure"))
					ImGuiLayer::set_light_azure_theme();

				if (ImGui::MenuItem(" Sunset Orange"))
					ImGuiLayer::set_sunset_orange_theme();

				if (ImGui::MenuItem(" Forest Green"))
					ImGuiLayer::set_forest_green_theme();

				if (ImGui::MenuItem(" Purple Nebula"))
					ImGuiLayer::set_purple_nebula_theme();

				if (ImGui::MenuItem(" Crimson Red"))
					ImGuiLayer::set_crimson_red_theme();

				if (ImGui::MenuItem(" Cyan Teal"))
					ImGuiLayer::set_cyan_teal_theme();

				if (ImGui::MenuItem(" Soft Pink"))
					ImGuiLayer::set_soft_pink_theme();

				if (ImGui::MenuItem(" Golden Amber"))
					ImGuiLayer::set_golden_amber_theme();

				if (ImGui::MenuItem(" Light Lavender"))
					ImGuiLayer::set_light_lavender_theme();

				if (ImGui::MenuItem(" MidNight Blue"))
					ImGuiLayer::set_midnight_blue_theme();

				if (ImGui::MenuItem(" Unity Light"))
					ImGuiLayer::set_unity_light_theme();

				if (ImGui::MenuItem(" Unity Dark"))
					ImGuiLayer::set_unity_dark_theme();

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::MenuItem(ICON_FA_EXPAND "  Fullscreen", "F11"))
			{
				//EditorLayer::get().toggle_fullscreen();
			}

			ImGui::EndMenu();
		}

		// Create Menu
		if (ImGui::BeginMenu("  Create  "))
		{
			if (ImGui::BeginMenu(ICON_FA_SQUARE "  2D Nodes"))
			{
				if (ImGui::MenuItem(ICON_FA_IMAGE "  Scene2D"))
				{
					//EditorLayer::get().create_scene2d();
				}
				if (ImGui::MenuItem(ICON_FA_SQUARE "  Rectangle"))
				{
					//EditorLayer::get().create_rectangle();
				}
				if (ImGui::MenuItem(ICON_FA_CIRCLE "  Circle"))
				{
					//EditorLayer::get().create_circle();
				}
				if (ImGui::MenuItem(ICON_FA_IMAGE "  Sprite"))
				{
					//EditorLayer::get().create_sprite();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		// Run Menu
		if (ImGui::BeginMenu("  Run  "))
		{


if (ImGui::MenuItem(ICON_FA_PLAY "  Run Default Scene", "F5", false))
{
    std::wstring folder = FileDialogs::get_exe_folder();
    std::wstring app = folder + std::wstring(1, path_sep) + L"Sandbox" + exe_ext;

    FileDialogs::run_exe(app);
}

if (ImGui::MenuItem(ICON_FA_CIRCLE_PLAY "  Run Current Scene", "Ctrl+F5", false))
{
    run_current_scene();

    std::wstring folder = FileDialogs::get_exe_folder();
    std::wstring app = folder + std::wstring(1, path_sep) + L"Sandbox" + exe_ext;

    FileDialogs::run_exe(app);
}
			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu("  Window  ")) {
			if (ImGui::MenuItem(ICON_FA_WINDOW_MINIMIZE "  Minimize", "Ctrl+M"))
			{
				//EditorLayer::get().minimize_window();
			}

			if (ImGui::MenuItem(ICON_FA_WINDOW_MAXIMIZE "  Maximize", "Ctrl+Shift+M"))
			{
				//EditorLayer::get().maximize_window();
			}
			ImGui::EndMenu();
		}

		// Help Menu
		if (ImGui::BeginMenu("  Help  "))
		{
			if (ImGui::MenuItem(ICON_FA_BOOK "  Documentation", "F1"))
			{
				//EditorLayer::get().open_documentation();
			}

			if (ImGui::MenuItem(ICON_FA_KEYBOARD "  Keyboard Shortcuts"))
			{
				//state.show_shortcuts_dialog = true;
			}
			ImGui::Separator();

			if (ImGui::MenuItem(ICON_FA_DOWNLOAD "  Check for Updates"))
			{
				//EditorLayer::get().check_for_updates();
			}

			ImGui::Separator();

			if (ImGui::MenuItem(ICON_FA_CIRCLE_INFO "  About"))
			{
				//state.show_about_dialog = true;
			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(ImGui::GetWindowWidth() - 300);

		ImGui::SameLine();
		ImGui::TextDisabled(ICON_FA_GAUGE "  FPS: %.1f", ImGui::GetIO().Framerate);

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
								props.texture = NodeHelper::load_texture(props.path, true, props.filter_mode);
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
				ImGui::SetCursorScreenPos(image_min);

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

				const char* drop_text = "Drop Texture Here";
				ImVec2 text_size = ImGui::CalcTextSize(drop_text);
				ImVec2 text_pos = ImVec2(
					image_min.x + (preview_size.x - text_size.x) * 0.5f,
					image_min.y + (preview_size.y - text_size.y) * 0.5f
				);

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


		{
			std::vector<std::string> load_mode;
			load_mode.push_back("Linear");
			load_mode.push_back("Nearest");
			draw_enum("Filter Mode", props.filter_mode, load_mode);
		}


		float button_spacing = 5.0f;
		float button_width = (ImGui::GetContentRegionAvail().x - button_spacing) * 0.5f;

		if (ImGui::Button(ICON_FA_DOWNLOAD"  Load Texture", ImVec2(button_width, button_height)))
		{
			EditorLayer::get().load_texture(entity);
		}

		ImGui::SameLine(0, 5.0f);

		if (!props.path.empty())
		{
			if (ImGui::Button(ICON_FA_UPLOAD"  Reload", ImVec2(button_width, button_height))) {
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
			if (ImGui::Button("Clear", ImVec2(button_width, 0))) {
				props.texture.reset();
				props.path.clear();
			}
		}

		ImGui::Spacing();



	}

	void UI::content_browser()
	{
		auto project = Project::get_active_project();
		std::filesystem::path root = project->get_directory();

		ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_NoScrollbar);

		draw_folder_node(root);

		ImGui::End();
	}

	bool UI::is_right_file(const std::filesystem::path& path)
	{
		std::string ext = path.extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" ||
			ext == ".bmp" || ext == ".tga" || ext == ".hdr" ||
			ext == ".tiff" || ext == ".tif" || ext == ".webp" ||
			ext == ".gif" || ext == ".psd" || ext == ".svg")
			return true;

		if (ext == ".aeroscene" || ext == ".scene" || ext == ".prefab")
			return true;

		if (ext == ".lua")
			return true;

		/*if (ext == ".lua" || ext == ".py" || ext == ".js")
			return true;*/

		/*if (ext == ".cpp" || ext == ".h" || ext == ".hpp" || ext == ".c" ||
			ext == ".cs" || ext == ".java")
			return true;*/

		/*if (ext == ".glsl" || ext == ".hlsl" || ext == ".vert" || ext == ".frag" ||
			ext == ".shader" || ext == ".compute")
			return true;*/

		/*if (ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb" ||
			ext == ".dae" || ext == ".blend" || ext == ".3ds")
			return true;*/

		if (ext == ".mp3" || ext == ".wav" || ext == ".ogg" || ext == ".flac" ||
			ext == ".aiff" || ext == ".wma")
			return true;

		/*if (ext == ".mp4" || ext == ".avi" || ext == ".mov" || ext == ".mkv" ||
			ext == ".webm" || ext == ".flv")
			return true;*/

		if (ext == ".ttf" || ext == ".otf" || ext == ".woff" || ext == ".woff2")
			return true;

		/*if (ext == ".json" || ext == ".xml" || ext == ".yaml" || ext == ".yml" ||
			ext == ".ini" || ext == ".cfg" || ext == ".toml")
			return true;*/

		/*if (ext == ".txt" || ext == ".md" || ext == ".log" || ext == ".csv")
			return true;*/

		/*if (ext == ".mat" || ext == ".material")
			return true;*/

		/*if (ext == ".anim" || ext == ".animation")
			return true;*/

		/*if (ext == ".zip" || ext == ".rar" || ext == ".7z")
			return true;*/

		return false;
	}

	bool UI::is_image(const std::filesystem::path& path)
	{
		std::string ext = path.extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		return ext == ".png" || ext == ".jpg" || ext == ".jpeg" ||
			ext == ".bmp" || ext == ".tga" || ext == ".hdr" ||
			ext == ".tiff" || ext == ".tif" || ext == ".webp";
	}

	void UI::draw_folder_node(const std::filesystem::path& directory, int depth)
	{
		std::vector<std::filesystem::directory_entry> entries;
		for (const auto& entry : std::filesystem::directory_iterator(directory))
		{
			entries.push_back(entry);
		}

		std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b)
			{
				if (a.is_directory() != b.is_directory())
				{
					return a.is_directory() > b.is_directory();
				}
				return a.path().filename().string() < b.path().filename().string();
			});

		for (const auto& entry : entries)
		{
			const auto& path = entry.path();
			std::string name = path.filename().string();
			std::string ext = path.extension().string();

			ImGui::PushID(path.string().c_str());

			static std::filesystem::path selected_path;
			bool is_selected = (selected_path == path);

			if (entry.is_directory())
			{
				ImGuiTreeNodeFlags flags =
					ImGuiTreeNodeFlags_Framed |
					ImGuiTreeNodeFlags_SpanAvailWidth |
					ImGuiTreeNodeFlags_AllowItemOverlap |
					ImGuiTreeNodeFlags_OpenOnArrow |
					(is_selected ? ImGuiTreeNodeFlags_Selected : 0);

				// Get folder icon based on open/closed state
				bool is_open = ImGui::TreeNodeEx(
					(std::string(ICON_FA_FOLDER) + "  " + name).c_str(),
					flags
				);

				if (ImGui::IsItemClicked())
				{
					selected_path = path;
				}

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem(ICON_FA_FOLDER_PLUS "  New Folder")) {}
					if (ImGui::MenuItem(ICON_FA_FILE "  New File")) {}
					ImGui::Separator();
					if (ImGui::MenuItem(ICON_FA_TRASH "  Delete")) {}
					if (ImGui::MenuItem(ICON_FA_PEN_TO_SQUARE "  Rename")) {}
					ImGui::EndPopup();
				}

				if (ImGui::BeginDragDropSource())
				{
					std::string p = path.string();
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", p.c_str(), p.size() + 1);
					ImGui::BeginTooltip();
					ImGui::Text(ICON_FA_FOLDER " %s", name.c_str());
					ImGui::Separator();
					ImGui::TextDisabled("Drag to move folder");
					ImGui::EndTooltip();
					ImGui::EndDragDropSource();
				}

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						// Handle drop
					}
					ImGui::EndDragDropTarget();
				}

				if (is_open)
				{
					draw_folder_node(path, depth + 1);
					ImGui::TreePop();
				}
			}
			else if (is_right_file(path))
			{
				ImGuiTreeNodeFlags flags =
					ImGuiTreeNodeFlags_Framed |
					ImGuiTreeNodeFlags_Leaf |
					ImGuiTreeNodeFlags_SpanAvailWidth |
					ImGuiTreeNodeFlags_AllowItemOverlap |
					ImGuiTreeNodeFlags_NoTreePushOnOpen |
					(is_selected ? ImGuiTreeNodeFlags_Selected : 0);

				const char* file_icon = get_file_icon(ext);

				ImGui::TreeNodeEx(
					(std::string(file_icon) + "  " + name).c_str(),
					flags
				);

				if (ImGui::IsItemClicked())
					selected_path = path;

				if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
				{
					ImGui::BeginTooltip();
					ImGui::Text("%s %s", file_icon, name.c_str());
					ImGui::EndTooltip();
				}

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN "  Open")) {}
					if (ImGui::MenuItem(ICON_FA_PEN_TO_SQUARE "  Rename")) {}
					ImGui::Separator();
					if (ImGui::MenuItem(ICON_FA_COPY "  Copy")) {}
					if (ImGui::MenuItem(ICON_FA_TRASH "  Delete")) {}
					ImGui::EndPopup();
				}

				if (ImGui::BeginDragDropSource())
				{
					std::string p = path.string();
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", p.c_str(), p.size() + 1);
					ImGui::BeginTooltip();
					ImGui::Text("%s  %s", file_icon, name.c_str());
					ImGui::Separator();
					ImGui::TextDisabled("Drag to move file");
					ImGui::EndTooltip();
					ImGui::EndDragDropSource();
				}
			}

			ImGui::PopID();
		}
	}

	const char* UI::get_file_icon(const std::string& extension)
	{
		// Convert to lowercase for comparison
		std::string ext = extension;
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		// Image files
		if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" ||
			ext == ".bmp" || ext == ".gif" || ext == ".tga")
			return ICON_FA_IMAGE;

		// Code files
		if (ext == ".cpp" || ext == ".h" || ext == ".hpp" ||
			ext == ".c" || ext == ".cs" || ext == ".java")
			return ICON_FA_FILE_CODE;

		// Script files
		if (ext == ".py" || ext == ".js" || ext == ".lua" || ext == ".sh")
			return ICON_FA_FILE_CODE;

		// Text files
		if (ext == ".txt" || ext == ".md" || ext == ".log")
			return ICON_FA_FILE_LINES;

		// Scene/prefab files
		if (ext == ".scene" || ext == ".prefab")
			return ICON_FA_CUBE;

		// Audio files
		if (ext == ".mp3" || ext == ".wav" || ext == ".ogg")
			return ICON_FA_FILE_AUDIO;

		// Video files
		if (ext == ".mp4" || ext == ".avi" || ext == ".mov")
			return ICON_FA_FILE_VIDEO;

		// 3D model files
		if (ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb")
			return ICON_FA_CUBE;

		// Shader files
		if (ext == ".glsl" || ext == ".vert" || ext == ".frag" || ext == ".shader")
			return ICON_FA_WAND_MAGIC_SPARKLES;

		// Config/data files
		if (ext == ".json" || ext == ".xml" || ext == ".yaml" || ext == ".ini")
			return ICON_FA_FILE_CODE;

		// Archive files
		if (ext == ".zip" || ext == ".rar" || ext == ".7z")
			return ICON_FA_FILE_ZIPPER;

		// PDF files
		if (ext == ".pdf")
			return ICON_FA_FILE_PDF;

		// Default file icon
		return ICON_FA_FILE;
	}

	void UI::draw_animation(Entity entity)
	{
		static std::string current_animation = "";

		std::string delete_animation;
		std::pair<std::string, std::string> rename_animation;

		auto& anim = entity.get_component<Animation_Component>();

		ImGui::SeparatorText("Current Animation");

		UI::draw_bool("Playing", anim.playing);
		ImGui::SameLine();

		if (ImGui::Button("Restart"))
		{
			anim.timer = 0.0f;
			anim.current_frame = 0;
			anim.current_animation_completed = false;
		}
		ImGui::Dummy(ImVec2(0.0f, 2.0f));

		ImGui::Text("Frame: %d / %s",
			anim.current_frame,
			anim.current_animation.empty() ? "N/A" :
			std::to_string(anim.animations[anim.current_animation].frames.size()).c_str());


		ImGui::Text("Timer: %.2fs", anim.timer);
		ImGui::Text("Completed: %s", anim.current_animation_completed ? "Yes" : "No");

		ImGui::Dummy(ImVec2(0.0f, 2.0f));
		ImGui::SeparatorText("Animation Selection");

		ImGui::Dummy(ImVec2(0.0f, 2.0f));
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
		if (ImGui::BeginPopupModal("Animation Exists", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("An animation with that name already exists!");
			if (ImGui::Button("OK"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}



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
					delete_animation = name;
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
							rename_animation = { original_name, display_name };

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

				float total_width = ImGui::GetContentRegionAvail().x;
				float button_spacing = 10.0f;

				GUI_Button button;

				button.size = { (total_width - button_spacing) * 0.5f , 35.0f };


				{
					button.label = "Select Frames";
					if (draw_button(button))
					{
						if (entity.has_component<Texture_Component>())
						{
							auto& texture_comp = entity.get_component<Texture_Component>();
							if (texture_comp.texture)
							{
								current_animation = name;
								s_show_panels.animation_selector = true;
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
				}
				ImGui::SameLine();
				{
					button.label = "Clear Frames";
					button.enabled = !animation.frames.empty();
					if (draw_button(button))
					{
						animation.frames.clear();
					}
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
					float frame_duration = 1.0f / animation.fps;
					float total_duration = animation.frames.size() * frame_duration;
					ImGui::Text("Frame Duration: %.3fs | Total: %.2fs", frame_duration, total_duration);
				}

				ImGui::TreePop();
			}
			ImGui::PopID();
		}


		// rename animation
		if (!rename_animation.first.empty())
		{
			auto node = anim.animations.extract(rename_animation.first);
			node.key() = rename_animation.second;
			anim.animations.insert(std::move(node));

			if (anim.current_animation == rename_animation.first)
				anim.current_animation = rename_animation.second;

			return;
		}

		// delete animation
		if (!delete_animation.empty())
		{
			anim.animations.erase(delete_animation);

			if (anim.current_animation == delete_animation)
				anim.current_animation.clear();

			return;
		}


		// show animation selector
		if (s_show_panels.animation_selector)
			draw_frame_selector(entity, current_animation);
	}

	void UI::draw_frame_selector(Entity entity, const std::string& current_animation)
	{
		static std::vector<int> selected_frames;
		static vec2u grid_size = { 10 ,10 };
		static bool use_size = false;

		vec2f cell_size;
		vec2i total_grid;

		auto& anim = entity.get_component<Animation_Component>();

		PopUpModel model;
		model.name = "Frame Selection";
		model.id = "Select Frames from Texture";
		model.confirm_name = "Add Frames";
		model.close_name = "Cancel";


		model.on_close = [&]()
			{
				s_show_panels.animation_selector = false;
				selected_frames.clear();
			};

		Extra_Settings extra_settings;
		extra_settings.size = grid_size;
		extra_settings.use_size = use_size;

		vec2f texture_size = entity.get_component<Texture_Component>().texture->get_size();

		extra_settings.button_controls = [&](const Button_Control_Value& details)
			{
				int cell_index = details.tile_id.y * details.line_count.x + details.tile_id.x;

				cell_size = details.size;
				total_grid = details.line_count;
				bool is_selected = std::find(selected_frames.begin(), selected_frames.end(), cell_index) != selected_frames.end();

				vec2f cell_min = details.screen_pos + vec2f(details.tile_id.x * details.size.x, details.tile_id.y * details.size.y) * details.scale;
				vec2f cell_max = details.screen_pos + vec2f((details.tile_id.x + 1) * details.size.x, (details.tile_id.y + 1) * details.size.y) * details.scale;


				if (is_selected)
				{
					details.draw_list->AddRectFilled(cell_min.to_imvec2(), cell_max.to_imvec2(), IM_COL32(0, 255, 0, 80));
					details.draw_list->AddRect(cell_min.to_imvec2(), cell_max.to_imvec2(), IM_COL32(0, 255, 0, 200), 0.0f, 0, 3.0f);
				}


				ImGui::PushID(cell_index);
				ImGui::SetCursorScreenPos(cell_min.to_imvec2());
				if (ImGui::InvisibleButton("##Cell", details.button_size.to_imvec2()))
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
					ImGui::Text("Grid Position: (%d, %d)", details.tile_id.x, details.tile_id.y);
					/*ImGui::Text("Texture Rect:");
					ImGui::Text("  Position: (%.0f, %.0f)", details.tile_id.y * cell_width, details.tile_id.x * cell_height);
					ImGui::Text("  Size: %.0fx%.0f", cell_width, cell_height);*/
					if (is_selected)
					{
						ImGui::TextColored(ImVec4(0, 1, 0, 1), "Selected");
					}
					ImGui::EndTooltip();
				}

				ImGui::PopID();
			};
		extra_settings.controls_panel = [&]()
			{
				float available_height = ImGui::GetContentRegionAvail().y;
				ImGui::Text("Frame Selection Controls");
				ImGui::Separator();
				ImGui::Dummy(ImVec2(0, 3));


				ImGui::Text("Grid Settings:");
				if (UI::draw_vec2("Size", grid_size))
				{
					selected_frames.clear();
				}
				ImGui::Dummy(ImVec2(0, 2));
				UI::draw_bool("Use Size", use_size);

				ImGui::Dummy(ImVec2(0, 3));


				ImGui::Text("Cell Size: %.0f x %.0f", cell_size.x, cell_size.y);
				ImGui::Dummy(ImVec2(0, 3));
				ImGui::Text("Total Cells: %d", total_grid.x * total_grid.y);
				ImGui::Dummy(ImVec2(0, 3));

				ImGui::Separator();

				ImGui::Text("Selection:");
				ImGui::Text("Selected: %d/%d frames",
					(int)selected_frames.size(), total_grid.x * total_grid.y);

				float total_width = ImGui::GetContentRegionAvail().x;
				float button_spacing = 6.0f;

				GUI_Button button;

				button.size = { (total_width - button_spacing) * 0.5f , 35.0f };


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




					ImGui::BeginGroup();
					button.label = "Select All";
					if (draw_button(button))
					{
						selected_frames.clear();
						for (int i = 0; i < total_grid.y * total_grid.x; i++)
						{
							selected_frames.push_back(i);
						}
					}
					ImGui::SameLine(0.0f, button_spacing);
					button.label = "Clear All";
					if (draw_button(button))
					{
						selected_frames.clear();
					}

					button.label = "Reverse Order";
					if (draw_button(button) && selected_frames.size() > 1)
					{
						std::reverse(selected_frames.begin(), selected_frames.end());
					}
					ImGui::SameLine(0.0f, 5.0f);
					button.label = "Sort Numerically";
					if (draw_button(button))
					{
						std::sort(selected_frames.begin(), selected_frames.end());
					}
					ImGui::EndGroup();


					ImGui::Separator();
					ImGui::Text("Quick Select:");

					button.label = "First Row";
					if (draw_button(button))
					{
						selected_frames.clear();
						for (int col = 0; col < total_grid.x; col++)
						{
							selected_frames.push_back(col);
						}
					}
					ImGui::SameLine(0.0f, button_spacing);
					button.label = "First Column";
					if (draw_button(button))
					{
						selected_frames.clear();
						for (int row = 0; row < total_grid.y; row++)
						{
							selected_frames.push_back(row * total_grid.x);
						}
					}

					button.label = "Diagonal Right";
					if (draw_button(button))
					{
						selected_frames.clear();
						int cells = std::min(total_grid.y, total_grid.x);
						for (int i = 0; i < cells; i++)
						{
							selected_frames.push_back(i * total_grid.x + i);
						}
					}
					ImGui::SameLine(0.0f, button_spacing);
					button.label = "Diagonal Left";
					if (draw_button(button))
					{
						selected_frames.clear();
						int cells = std::min(total_grid.y, total_grid.x);
						for (int i = cells - 1; i >= 0; i--)
						{
							selected_frames.push_back(i * total_grid.y + (cells - 1 - i));
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


				ImGui::SetCursorPosY(available_height - button.size.y);

				button.label = "Add Frames";
				if (draw_button(button) && !selected_frames.empty())
				{
					auto it = anim.animations.find(current_animation);
					if (it != anim.animations.end())
					{
						Animation& target_anim = it->second;
						target_anim.frames.clear();
						for (int cell_index : selected_frames)
						{
							int row = cell_index / total_grid.x;
							int col = cell_index % total_grid.x;

							Frame new_frame;
							new_frame.frame_rect.position.x = col * cell_size.x;
							new_frame.frame_rect.position.y = row * cell_size.y;
							new_frame.frame_rect.size.x = cell_size.x;
							new_frame.frame_rect.size.y = cell_size.y;
							target_anim.frames.push_back(new_frame);
						}
						s_show_panels.animation_selector = false;
						selected_frames.clear();
					}
				}
				ImGui::SameLine(0.0f, button_spacing);
				button.label = "Cancel";
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

	void UI::draw_autotiling_register(Entity entity)
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

	bool UI::draw_tilemap_selector(Entity entity, vec2u& id, std::string& set_name, bool& use_autotile)
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
				if (entity.has_component<AutoTiling_Component>())
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
					if (current_size.x != 0 && current_size.y != 0)
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

	bool UI::texture_selector(Entity entity, uint_rect& texture_rect)
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
		float button_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2.0f;
		if (ImGui::Button("Create New", ImVec2(button_width, 0)))
		{
			s_show_panels.create_new_script = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("Add Existing", ImVec2(button_width, 0)))
		{
			auto full_path = FileDialogs::open_file("Lua Scripts(*.lua)\0 * .lua\0All Files(*.*)\0 * .*\0");
			if (!full_path.empty())
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
		const float corner_radius = 8.0f;
		const ImVec4 accent_color = ImVec4(0.1f, 0.5f, 0.9f, 1.0f);
		const ImVec4 muted_color = ImVec4(0.5f, 0.5f, 0.55f, 1.0f);

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
		std::string filename = file_path.filename().string();

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
				auto project = Project::get_active_project();
				std::string full_path = project->get_directory() + project->get_scripts_directory() + props.path;
				open_in_vscode(full_path);
			}

			if (ImGui::MenuItem(ICON_FA_LINK "  Open Containing Folder", nullptr, false, !props.path.empty()))
			{
				auto project = Project::get_active_project();
				std::string full_path = project->get_directory() + project->get_scripts_directory() + props.path;
				show_in_explorer(full_path);
			}

			if (ImGui::MenuItem(ICON_FA_COPY "  Copy Path", nullptr, false, !props.path.empty()))
			{
				auto project = Project::get_active_project();
				std::string full_path = project->get_directory() + project->get_scripts_directory() + props.path;
				ImGui::SetClipboardText(full_path.c_str());
			}

			ImGui::Separator();

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
			if (ImGui::MenuItem(ICON_FA_TRASH "  Remove Script", nullptr, false, !props.path.empty()))
			{
				props.path.clear();
				Scene::save_required();
			}
			ImGui::PopStyleColor();

			ImGui::PopStyleVar();
			ImGui::EndPopup();
		}


		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM",
				ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

			if (payload)
			{
				const char* data = static_cast<const char*>(payload->Data);
				std::string file_path(data, payload->DataSize);
				script_drag_drop(file_path);
			}
			ImGui::EndDragDropTarget();
		}
	}

	void UI::script_drag_drop(std::string& file_path)
	{
		file_path.erase(std::find(file_path.begin(), file_path.end(), '\0'), file_path.end());
		file_path.erase(std::remove(file_path.begin(), file_path.end(), '\"'), file_path.end());

		Helper::normalize_path(file_path);

		std::filesystem::path path(file_path);
		std::string ext = path.extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		if (ext == ".lua")
		{
			EditorLayer::get().create_new_script(file_path);
		}
	}

	void UI::draw_script_drop_zone(Entity entity, Script_Component& props)
	{
		const float corner_radius = 10.0f;

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, corner_radius);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

		ImVec2 drop_zone_size = ImVec2(ImGui::GetContentRegionAvail().x, 120);
		ImGui::BeginChild("##drop_zone", drop_zone_size, true, ImGuiWindowFlags_NoScrollbar);

		ImVec2 window_size = ImGui::GetWindowSize();
		ImVec2 content_size = ImVec2(window_size.x - ImGui::GetStyle().WindowPadding.x * 2,
			window_size.y - ImGui::GetStyle().WindowPadding.y * 2);

		ImGui::SetCursorPos(ImVec2(
			(window_size.x - content_size.x) * 0.5f,
			(window_size.y - content_size.y) * 0.5f
		));

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
					script_drag_drop(file_path);
				}
			}
			ImGui::EndDragDropTarget();
		}

		// Optional: Add subtle hover effect
		if (ImGui::IsItemHovered() && !is_dragging_over)
		{
			ImGui::GetWindowDrawList()->AddRect(
				ImGui::GetItemRectMin(),
				ImGui::GetItemRectMax(),
				ImGui::GetColorU32(ImVec4(0.3f, 0.5f, 0.8f, 0.3f)),
				corner_radius,
				0,
				1.0f
			);
		}

		ImGui::PopStyleVar(2);
	}

	void UI::draw_script_actions(Entity entity, Script_Component& props, bool has_script)
	{
		ImGui::Dummy(ImVec2(0, 10));
		float button_width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2.0f;
		float button_height = 32.0f;
		if (ImGui::Button(ICON_FA_FILE "Create New", ImVec2(button_width, button_height)))
		{
			s_show_panels.create_new_script = true;
		}

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_PLUS "Add Existing", ImVec2(button_width, button_height)))
		{
			auto full_path = FileDialogs::open_file("Lua Scripts(*.lua)\0 * .lua\0All Files(*.*)\0 * .*\0");
			if (!full_path.empty())
			{
				EditorLayer::get().create_new_script(full_path);
			}
		}
	}


	void UI::open_in_vscode(const std::string& path)
	{
		if (path.empty()) return;


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
		if (file_path.empty()) return;

		std::filesystem::path abs_path = std::filesystem::absolute(file_path);
    std::filesystem::path folder = abs_path.parent_path();

#ifdef  PLATFORM_WINDOWS
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
			//return;
		}



		PopUpModel model;
		model.name = "Texture Selector";
		model.id = "1234567890";
		model.confirm_name = "Accept";
		model.close_name = "Cancel";

		Extra_Settings x_settings;
		x_settings.size = { 32, 32 };
		x_settings.draw_lines = true;
		x_settings.use_size = true;
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
			clicked = ImGui::Button(
				btn.label.c_str(),
				btn.size.to_imvec2()
			);
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
		create_new_scene();
		save_changes();
		EditorLayer::get().render_export_panel();
	}

	void UI::create_new_scene()
	{
		if (!s_show_panels.create_new_scene)
			return;

		PopUpModel model;
		model.id = "##CreateNewScene";
		model.name = "New Scene";
		model.window_size = { 500, 275 };

		model.draw_content = [&]() {
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
			strncpy(path_buffer, path.c_str(), sizeof(path_buffer));
			if (ImGui::InputText("##ScenePath", path_buffer, sizeof(path_buffer)))
			{
				path = path_buffer;
			}
			ImGui::PopStyleVar();
			ImGui::SameLine();

			GUI_Button button;

			{
				button.label = "...";
				button.size = { 40, 32 };
				button.enabled = true;
				if (draw_button(button))
				{
					path = ag::FileDialogs::select_folder("Select Folder for a new Scene");
				}
			}

			bool can_create = !new_name.empty() && !path.empty();

			ImGui::Dummy(ImVec2(0, 25));

			float available_width = ImGui::GetContentRegionAvail().x;
			float spacing = 10.0f;
			vec2f button_size;
			button_size.x = (available_width - (spacing * 3)) * 0.5f;
			button_size.y = 35.0f;

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + spacing);

			{
				button.label = "Create";
				button.size = button_size;
				button.enabled = can_create;
				if (draw_button(button))
				{
					if (can_create)
					{
						if (!path.empty())
						{
							std::string full_path = path + "/" + new_name + ".aeroscene";
							EditorLayer::get().create_new_scene(full_path);
						}
						new_name.clear();
						path.clear();
						s_show_panels.create_new_scene = false;
						ImGui::CloseCurrentPopup();
					}
				}
			}
			ImGui::SameLine(0, spacing);

			Color cancel_color = Color(94, 94, 94);
			Color cancel_hover_color = Color(112, 112, 112);
			{
				button.label = "Cancel";
				button.size = button_size;
				button.enabled = true;
				if (draw_button(button))
				{
					new_name.clear();
					path.clear();
					s_show_panels.create_new_scene = false;
					ImGui::CloseCurrentPopup();
				}
			}
			};

		model.on_close = []() {
			new_name.clear();
			path.clear();
			s_show_panels.create_new_scene = false;
			ImGui::CloseCurrentPopup();
			};

		Create_Open_Popup::draw_popup(model);
	}

	void UI::create_new_script(Entity entity)
	{
		if (!s_show_panels.create_new_script)
			return;

		PopUpModel model;
		model.id = "##CreateNewScript";
		model.name = "New Script";
		model.window_size = { 500, 280 };

		model.draw_content = [&]() {
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
			strncpy(path_buffer, path.c_str(), sizeof(path_buffer));
			if (ImGui::InputText("##ScriptPath", path_buffer, sizeof(path_buffer)))
			{
				path = path_buffer;
			}
			ImGui::PopStyleVar();
			ImGui::SameLine();

			GUI_Button button;


			{
				button.label = "...";
				button.size = { 40, 32 };
				button.enabled = true;
				if (draw_button(button))
				{
					path = ag::FileDialogs::select_folder("Select Folder for a new Scene");
				}
			}

			bool can_create = !new_name.empty() && !path.empty();

			ImGui::Dummy(ImVec2(0, 25));

			float available_width = ImGui::GetContentRegionAvail().x;
			float spacing = 10.0f;
			vec2f button_size;
			button_size.x = (available_width - (spacing * 3)) * 0.5f;
			button_size.y = 35.0f;

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + spacing);

			Color create_color = can_create ? Color(94, 94, 94) : Color(58, 58, 58);
			Color hover_color = can_create ? Color(112, 112, 112) : Color(58, 58, 58);
			{
				button.label = "Create";
				button.size = button_size;
				button.enabled = can_create;
				if (draw_button(button))
				{
					if (can_create)
					{
						if (!path.empty())
						{
							std::string full_path = path + "/" + new_name + ".lua";
							Helper::normalize_path(full_path);
							std::ofstream file(full_path);
							if (file.is_open())
							{
								file << "--- " << new_name << ".lua ---\n\n";
								file << "local entity = get_entity()\n\n";
								file << "function on_create()\n";
								file << "  aero_print(\"Entity Created\")\n";
								file << "end\n\n";
								file << "function on_update(dt)\n";
								file << "  aero_print(\"Entity Updated\")\n";
								file << "end\n\n";
								file << "function on_destroy()\n";
								file << "  aero_print(\"Entity Destroyed\")\n";
								file << "end\n\n";
								file << "function on_event(event)\n";
								file << "  aero_print(\"Entity Events\")\n";
								file << "end\n\n";
							}
							file.close();

							std::string base_path;
							std::string relative_path;
							{
								auto project = Project::get_active_project();
								std::string base_path = project->get_directory() + project->get_scripts_directory() + "/";

								relative_path = full_path;
								if (relative_path.find(base_path) == 0)
									relative_path = relative_path.substr(base_path.size());

								Helper::normalize_path(relative_path);
							}
							auto& comps = entity.get_component<Script_Component>();
							comps.path =  "/" + relative_path;
						}
						new_name.clear();
						path.clear();
						s_show_panels.create_new_script = false;
						ImGui::CloseCurrentPopup();
					}
				}
			}
			ImGui::SameLine(0, spacing);

			Color cancel_color = Color(94, 94, 94);
			Color cancel_hover_color = Color(112, 112, 112);
			{
				button.label = "Cancel";
				button.size = button_size;
				button.enabled = true;
				if (draw_button(button))
				{
					new_name.clear();
					path.clear();
					s_show_panels.create_new_script = false;
					ImGui::CloseCurrentPopup();
				}
			}
			};

		model.on_close = []() {
			new_name.clear();
			path.clear();
			s_show_panels.create_new_script = false;
			ImGui::CloseCurrentPopup();
			};

		Create_Open_Popup::draw_popup(model);
	}

	void UI::save_changes()
	{
		if (!s_show_panels.save_changes_panel)
			return;

		{
			auto scene = Scene::get_active_scene();
			std::string remove_scene_name = EditorLayer::get().get_remove_scene();
			std::string scene_name = scene->get_name();
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
		model.id = "##SaveChangesPanel";
		model.name = "Save Changes";
		model.window_size = { 600, 300 };

		model.draw_content = [&]() {
			ImGui::Text("Save Changes to the following items?");


			ImGui::Dummy(ImVec2(0, 10));


			float available_width = ImGui::GetContentRegionAvail().x;
			float spacing = 10.0f;

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
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetContentRegionAvail().y - button.size.y - spacing);

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

		Create_Open_Popup::draw_popup(model);
	}

	void UI::custom_popup(const std::string& popup_id, const std::string& popup_name, std::function<void()> draw_content, std::function<void()> close)
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

	void UI::run_current_scene()
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

	void UI::draw_console()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;

		ImGui::Begin("Console", nullptr, flags);
		Log::draw_console("Console", &s_show_panels.console_panel);
		ImGui::End();
	}
}
