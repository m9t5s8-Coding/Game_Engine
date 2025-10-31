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
		// --- Get window size & position ---
		ag::vec2u window_size = ag::Application::get().get_window().get_size();
		ag::vec2f window_pos = ag::Application::get().get_window().get_position();

		// --- Window flags ---
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

		// --- Window style ---
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.12f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));

		ImGui::Begin("MainWindow", nullptr, window_flags);
		ImGui::Spacing();
		// --- Title ---
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

		// --- Center buttons + search bar ---
		float total_width = 80 + 5 + 80 + 5 + 420;
		float window_center = window_size.x / 2.0f;
		ImGui::SetCursorPosX(window_center - total_width / 2.0f);

		// --- Buttons style ---
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 5));

		ImVec4 text_color = ImVec4(0.89f, 0.89f, 0.89f, 1.0f);
		ImVec4 bg_color = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);
		ImVec4 bg_hovered = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, bg_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bg_hovered);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, bg_hovered);
		ImGui::PushStyleColor(ImGuiCol_Text, text_color);

		// --- Buttons ---
		if (ImGui::Button("Create", ImVec2(80, 35)))
		{
			create_new_project();
		}
		ImGui::SameLine(0, 5);
		if (ImGui::Button("Import", ImVec2(80, 35)))
		{
			open_existing_project();
		}

		// --- Search bar ---
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

		// --- Pop buttons style ---
		ImGui::PopStyleColor(4);
		ImGui::PopStyleVar(2);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		{
			ImGui::SetCursorPosX(window_center - total_width / 2.0f);
			ImGui::TextColored(ImVec4(0.8f, 0.9f, 1.0f, 1.0f), "Recent Projects");
		}

		// --- Example recent projects list ---
		ImVec2 box_size = ImVec2(ImGui::GetContentRegionAvail().x, 120);
		ImGui::BeginChild("RecentProjectsBox", box_size, false);

		// Title

		ImGui::Spacing();

		ImGui::BulletText("C:\\Projects\\AeroGameEngine");
		ImGui::BulletText("D:\\Engines\\SandboxEditor");
		ImGui::BulletText("E:\\Workspace\\Prototype01");

		ImGui::EndChild();

		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}


	void ProjectManagerLayer::on_event(ag::Event& e)
	{

	}




	void ProjectManagerLayer::create_new_project()
	{
		std::string newPath = ag::FileDialogs::select_folder("Select Folder for a new project");
		if (newPath.empty())
			return;

		auto project = ag::Project::new_project(newPath);
		json j;
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

		AppSettings::reload_app();
	}
	void ProjectManagerLayer::open_existing_project()
	{
		std::string path = ag::FileDialogs::select_folder("Select Your Project Folder");
		if (!path.empty())
		{
			auto project = ag::Project::load_project(path);
			json j;
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

			AppSettings::reload_app();
		}

	}
}

