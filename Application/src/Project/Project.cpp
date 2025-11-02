#include <Project/Project.hpp>
#include <Apch.hpp>
#include <Core/Application.hpp>
#include <Helper.hpp>
#include <Project/SaveScene.hpp>

namespace fs = std::filesystem;

namespace ag
{
	AG_ref<Project> Project::new_project(const std::string& path)
	{
		auto project_path = Helper::normalize_path(path);
		AERO_CORE_INFO("Project Path:{0}", project_path);
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

		set_active_project(project);
		project->save();
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

		json j;
		Helper::makefile_read_only(file_path, false);
		std::ifstream in(file_path);
		if (!in.is_open())
			return nullptr;
		in >> j;
		in.close();
		Helper::makefile_read_only(file_path);

		auto project = AG_cref<Project>();
		{
			Helper::load_json(j["Project"], "Name", project->m_name);
			Helper::load_json(j["Project"], "Directory", project->m_directory);
			Helper::load_json(j["Project"], "File Path", project->m_project_file_path);
			Helper::load_json(j["Project"], "Assets", project->m_assets_directory);
			Helper::load_json(j["Project"], "Scenes", project->m_scenes_directory);
			Helper::load_json(j["Project"], "Scripts", project->m_scripts_directory);
			set_active_project(project);
		}
		return project;
	}

	AG_ref<Project> Project::save_project()
	{
		auto project = get_active_project();
		json j;

		Helper::makefile_read_only(project->get_project_file_directory() ,false);
		std::ifstream in(project->get_project_file_directory());
		in >> j;
		in.close();
		Helper::makefile_read_only(project->get_project_file_directory());

		auto& window = Application::get().get_window();
		j["Window"] = {
			{ "Title", window.get_title() },
			{ "Size", window.get_size().save() },
			{ "FullScreen", window.is_full_screen() },
			{ "Center", false },
			{ "Position", window.get_position().save() },
			{ "Vsync", window.is_vsync() }
		};

		Helper::makefile_read_only(project->get_project_file_directory(), false);
		std::ofstream out(project->get_project_file_directory());
		out << j.dump(4);
		out.close();
		Helper::makefile_read_only(project->get_project_file_directory());
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
		auto p = Project::get_active_project();
		Helper::save_json(j["Project"], "Name", p->m_name);
		Helper::save_json(j["Project"], "Directory", p->m_directory);
		Helper::save_json(j["Project"], "File Path", p->m_project_file_path);
		Helper::save_json(j["Project"], "Assets", p->m_assets_directory);
		Helper::save_json(j["Project"], "Scenes", p->m_scenes_directory);
		Helper::save_json(j["Project"], "Scripts", p->m_scripts_directory);

		Helper::save_json(j["Window"], "Title", p->m_name);
		Helper::save_json(j["Window"], "Size", vec2u(1280, 720));
		Helper::save_json(j["Window"], "FullScreen", false);
		Helper::save_json(j["Window"], "Center", true);
		Helper::save_json(j["Window"], "Position", vec2i(0, 0));
		Helper::save_json(j["Window"], "Vsync", true);

		auto scene = AG_cref<Scene>();
		Scene::set_active_scene(scene);
		scene->set_name("main");
		scene->set_directory("/" + scene->get_name() + ".aeroscene");
		Helper::save_json(j["Scene"], "Last Loaded", scene->get_name());
		Helper::save_json(j["Scene"], "Last Loaded Path", scene->get_directory());
		Helper::save_json(j["Scene"], "Default", scene->get_name());
		Helper::save_json(j["Scene"], "Default Path", scene->get_directory());

		std::string scene_path = p->m_directory + p->m_scenes_directory + "/" + scene->get_directory();
		SaveScene::save_scene(scene, scene_path);

		Helper::makefile_read_only(p->m_project_file_path, false);
		std::ofstream out(p->m_project_file_path);
		out << j.dump(4);
		out.close();
		Helper::makefile_read_only(p->m_project_file_path);
	}
}

