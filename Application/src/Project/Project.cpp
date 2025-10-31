#include <Project/Project.hpp>
#include <Apch.hpp>
#include <Core/Application.hpp>
#include <Helper.hpp>

namespace fs = std::filesystem;

namespace ag
{
	AG_ref<Project> Project::new_project(const std::string& path)
	{
		auto project_path = Helper::normalize_path(path);
		auto project = AG_cref<Project>();

		project->set_directory(project_path);
		project->set_name(fs::path(path).filename().string());

		project->m_assets_directory = "/Assets";
		project->m_scenes_directory = "/Scenes";
		project->m_scripts_directory = "/Scripts";
		project->m_project_file_path = project_path + "/" + project->m_name + ".aeroproj";

		fs::create_directories(project_path + project->m_assets_directory);
		fs::create_directories(project_path + project->m_scenes_directory);
		fs::create_directories(project_path + project->m_scripts_directory);

		project->save();


		set_active_project(project);

		return project;
	}

	AG_ref<Project> Project::load_project(const std::string& path)
	{
		std::string file_path = Helper::normalize_path(path);

		if (std::filesystem::is_directory(path))
		{
			for (const auto& entry : std::filesystem::directory_iterator(path))
			{
				if (entry.path().extension() == ".aeroproj")
				{
					file_path = entry.path().string();
					break;
				}
			}
			if (file_path == path)
			{
				AERO_CORE_INFO("Project not Found!");
				return nullptr;
			}
		}

		std::ifstream in(file_path);
		if (!in.is_open())
			return nullptr;

		nlohmann::json j;
		in >> j;
		in.close();

		auto project = AG_cref<Project>();
		{
			auto proj = j["Project"];
			Helper::load_json(proj, "Name", project->m_name);
			Helper::load_json(proj, "Directory", project->m_directory);
			Helper::load_json(proj, "File Path", project->m_project_file_path);
			Helper::load_json(proj, "Assets", project->m_assets_directory);
			Helper::load_json(proj, "Scenes", project->m_scenes_directory);
			Helper::load_json(proj, "Scripts", project->m_scripts_directory);
			set_active_project(project);
		}

		return project;
	}

	AG_ref<Project> Project::save_project()
	{
		auto project = get_active_project();

		std::ifstream in(project->get_project_file_directory());

		json j;
		in >> j;
		in.close();

		auto& window = Application::get().get_window();
		j["Window"] = {
			{ "Title", window.get_title() },
			{ "Size", window.get_size().save() },
			{ "FullScreen", window.is_full_screen() },
			{ "Center", false },
			{ "Position", window.get_position().save() },
			{ "Vsync", window.is_vsync() }
		};

		std::ofstream out(project->get_project_file_directory());
		out << j.dump(4);
		out.close();
		return project;
	}

	void Project::save_active()
	{
		if (s_active_project)
			s_active_project->save();
	}

	void Project::save()
	{
		nlohmann::json j;


		j["Project"] = {
			{ "Name", m_name},
			{ "Directory", m_directory},
			{ "File Path" , m_project_file_path},
			{ "Assets", m_assets_directory},
			{ "Scenes", m_scenes_directory},
			{ "Scripts", m_scripts_directory}
		};

		j["Window"] = {
			{ "Title", "Aero Engine"},
			{ "Size", { 1280, 720 }},
			{ "FullScreen", false },
			{ "Center", true },
			{ "Position", { 0, 0 }},
			{ "Vsync", true }
		};



		std::ofstream out(m_project_file_path);
		out << j.dump(4);
	}
}

