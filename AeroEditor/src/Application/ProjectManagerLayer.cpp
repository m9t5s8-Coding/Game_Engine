#include <Application/ProjectManagerLayer.hpp>
#include <Application/AppSettings.hpp>
#include <imgui.h>


namespace ag
{
	ProjectManagerLayer::ProjectManagerLayer()
		: Layer("ProjectManagerLayer")
	{

	}

	void ProjectManagerLayer::on_attach()
	{
	}

	void ProjectManagerLayer::on_detach()
	{

	}

	void ProjectManagerLayer::on_update(ag::TimeStamp ts)
	{

	}

	void ProjectManagerLayer::on_imgui_render()
	{
		ag::vec2u window_size = ag::Application::get().get_window().get_size();
		ag::vec2f window_pos = ag::Application::get().get_window().get_position();

		ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse;

		ImGui::SetNextWindowPos({ window_pos.x, window_pos.y });
		ImGui::SetNextWindowSize({ (float)window_size.x, (float)window_size.y });

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.12f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));

		ImGui::Begin("MainWindow", nullptr, window_flags);
		ImGui::Spacing();
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGui::PushFont(io.Fonts->Fonts[1]);
			const char* text = "Welcome to AERO Engine";
			float text_width = ImGui::CalcTextSize(text).x;
			ImGui::SetCursorPosX((window_size.x - text_width) / 2.0f);

			ImGui::TextColored(ImVec4(0.8f, 0.9f, 1.0f, 1.0f), "%s", text);
			ImGui::PopFont();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		float total_width = 80 + 5 + 80 + 5 + 420;
		float window_center = window_size.x / 2.0f;
		ImGui::SetCursorPosX(window_center - total_width / 2.0f);

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 5));

		ImVec4 text_color = ImVec4(0.89f, 0.89f, 0.89f, 1.0f);
		ImVec4 bg_color = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);
		ImVec4 bg_hovered = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, bg_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bg_hovered);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, bg_hovered);
		ImGui::PushStyleColor(ImGuiCol_Text, text_color);

		if (ImGui::Button("Create", ImVec2(80, 35)))
		{
			create_new_project();
		}
		ImGui::SameLine(0, 5);
		if (ImGui::Button("Import", ImVec2(80, 35)))
		{
			open_existing_project();
		}

		ImGui::SameLine(0, 5);
		static char search_buffer[128] = "";

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.18f, 0.18f, 0.22f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 0.25f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.25f, 0.25f, 0.3f, 1.0f));

		ImGui::SetNextItemWidth(420);
		ImGui::InputTextWithHint("##SearchProjects", "Search Projects...", search_buffer, IM_ARRAYSIZE(search_buffer));

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();

		ImGui::PopStyleColor(4);
		ImGui::PopStyleVar(2);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		{
			ImGui::SetCursorPosX(window_center - total_width / 2.0f);
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			ImGui::TextColored(ImVec4(0.8f, 0.9f, 1.0f, 1.0f), "Recent Projects");
			ImGui::PopFont();

			ImGui::SetCursorPosX(window_center - total_width / 2.0f);
			ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.7f, 1.0f), "Select a project to open");
		}
		ImGui::Spacing();
		ImGui::Spacing();

		ImVec2 box_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
		ImGui::BeginChild("RecentProjectsBox", box_size, false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);

		struct ProjectEntry
		{
			const char* name;
			const char* path;
			const char* lastModified;
		};

		static ProjectEntry projects[] = {
		{ "AeroGameEngine", "C:\\Projects\\AeroGameEngine", "2 hours ago"},
		{ "Sandbox Editor", "D:\\Engines\\SandboxEditor", "Yesterday"},
		{ "Prototype 01", "D:\\Workspace\\Prototype01", "3 days ago"},
		{ "Game Demo", "D:\\Projects\\GameDemo", "Last week"},
		{ "Demo Test", "D:\\Tests\\PhysicsTest", "2 weeks ago"}
		};
		float item_width = ImGui::GetContentRegionAvail().x - 5.0f;
		float item_height = 70.0f;

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15.0f, 15.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f));


		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.18f, 0.18f, 0.22f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.25f, 0.25f, 0.30f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.35f, 0.35f, 0.40f, 1.0f));
		for (int i = 0; i < IM_ARRAYSIZE(projects); i++)
		{
			ImGui::PushID(i);

			ImVec2 item_size(item_width, item_height);

		
			bool selected = false;
			if (ImGui::Selectable("##ProjectItem", &selected,
				ImGuiSelectableFlags_AllowDoubleClick,
				item_size))
			{
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					// open_project(projects[i].path);
				}
			}

			ImVec2 min = ImGui::GetItemRectMin();
			ImVec2 max = ImGui::GetItemRectMax();


			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			ImU32 border_color = ImGui::GetColorU32(ImVec4(0.35f, 0.35f, 0.40f, 1.0f));
			float border_thickness = 1.0f;

			draw_list->AddRect(min, max, border_color, 0.0f, 0, border_thickness);



			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Open"))
				{
					// open_project(projects[i].path);
				}
				if (ImGui::MenuItem("Remove from list"))
				{

				}
				if (ImGui::MenuItem("Show in Explorer"))
				{

				}
				ImGui::EndPopup();
			}

		
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - item_width + 5.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

			ImGui::BeginGroup();

			ImGui::SetWindowFontScale(1.2f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
			ImGui::Text("%s", projects[i].name);
			ImGui::SetWindowFontScale(1.0f);
			ImGui::PopStyleColor();

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
			ImGui::Text("%s", projects[i].path);
			ImGui::PopStyleColor();

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.7f, 1.0f));
			std::string text = "Last opened: ";
			text += projects[i].lastModified;
			ImGui::SameLine();
			ImGui::SetWindowFontScale(0.9f);
			ImVec2 size = ImGui::CalcTextSize(text.c_str());
			float width = size.x;

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() +	ImGui::GetContentRegionAvail().x - width - 10.0f);
			ImGui::Text("Last opened: %s", projects[i].lastModified);
			ImGui::PopStyleColor();
			ImGui::SetWindowFontScale(1.f);
			ImGui::EndGroup();

			ImGui::Spacing();
			ImGui::PopID();
		}
		ImGui::PopStyleColor(3);

		ImGui::PopStyleVar(3);

		if (IM_ARRAYSIZE(projects) == 0)
		{
			float empty_text_width = ImGui::CalcTextSize("No recent projects").x;
			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - empty_text_width) * 0.5f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 50.0f);

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.6f, 1.0f));
			ImGui::Text("No recent projects");
			ImGui::PopStyleColor();

			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200.0f) * 0.5f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);

			if (ImGui::Button("Create Your First Project", ImVec2(200.0f, 40.0f)))
			{
				create_new_project();
			}
		}

		ImGui::EndChild();

		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}


	void ProjectManagerLayer::on_event(ag::Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(AERO_BIND_EVENT_FN(ProjectManagerLayer::on_window_close));

	}

	bool ProjectManagerLayer::on_window_close(WindowCloseEvent& e)
	{
		Application::get().m_running = false;

		return true;
	}


	void ProjectManagerLayer::create_new_project()
	{
		std::string newPath = ag::FileDialogs::select_folder("Select Folder for a new project");
		if (newPath.empty())
			return;

		auto project = ag::Project::new_project(newPath);
		json j;
		Helper::makefile_read_only(AppSettings::get_settings_path(), false);
		std::ifstream in_file(AppSettings::get_settings_path());
		if (in_file.is_open())
			in_file >> j;

		in_file.close();

		Helper::save_json(j["Project"], "Name", project->get_name());
		Helper::save_json(j["Project"], "Directory", project->get_directory());
		Helper::save_json(j["Project"], "File Path", project->get_project_file_directory());
		Helper::save_json(j, "Mode", static_cast<int>(AppSettings::Mode::Editor));

		AERO_CORE_INFO("Project File:{0}", project->get_project_file_directory());
		std::ofstream out_file(AppSettings::get_settings_path());
		out_file << j.dump(4);
		out_file.close();
		Helper::makefile_read_only(AppSettings::get_settings_path());

		AppSettings::reload_app();
	}
	void ProjectManagerLayer::open_existing_project()
	{

		std::string path = ag::FileDialogs::select_folder("Select Your Project Folder");
		if (!path.empty())
		{
			json j;
			auto project = ag::Project::load_project(path);
			if (!project->m_project_loaded)
			{
				Helper::makefile_read_only(AppSettings::get_settings_path(), false);
				std::ifstream in_file(AppSettings::get_settings_path());
				if (in_file.is_open())
					in_file >> j;

				in_file.close();
				Helper::save_json(j, "Mode", static_cast<int>(AppSettings::Mode::ProjectManager));
				std::ofstream out_file(AppSettings::get_settings_path());
				out_file << j.dump(4);
				out_file.close();
				Helper::makefile_read_only(AppSettings::get_settings_path());
				AppSettings::reload_app();
				return;
			}
			

			Helper::makefile_read_only(AppSettings::get_settings_path(), false);
			std::ifstream in_file(AppSettings::get_settings_path());
			if (in_file.is_open())
				in_file >> j;

			in_file.close();
			

			Helper::save_json(j["Project"], "Name", project->get_name());
			Helper::save_json(j["Project"], "Directory", project->get_directory());
			Helper::save_json(j["Project"], "File Path", project->get_project_file_directory());
			Helper::save_json(j, "Mode", static_cast<int>(AppSettings::Mode::Editor));


			std::ofstream out_file(AppSettings::get_settings_path());
			out_file << j.dump(4);
			out_file.close();

			Helper::makefile_read_only(AppSettings::get_settings_path());
			AppSettings::reload_app();
		}

	}
}

