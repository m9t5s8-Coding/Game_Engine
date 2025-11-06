#include <Application/EditorApp.hpp>

namespace ag
{
	EditorApp::EditorApp()
	{

	}

	void EditorApp::on_create()
	{
		NodeFactory::init();
		load_project_data();
		

		Renderer::init();
		LuaManager::init();

		{
			auto project = Project::get_active_project();
			std::string script_path = project->get_directory() + project->get_scripts_directory() + "/test.lua";
			LuaManager::load_script(script_path);
		}
		

		push_layer(new EditorLayer());
	}

	void EditorApp::on_destroy()
	{
		Project::save_project();
		save_json();

		Renderer2D::shut_down();
		LuaManager::shut_down();
		NodeFactory::shut_down();
	}

	void EditorApp::save_json()
	{
		json j;
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
		}
		{
			auto project = Project::get_active_project();
			auto project_file_path = project->get_project_file_directory();
			Helper::makefile_read_only(project_file_path, false);
			std::ifstream in_file(project_file_path);
			if (!in_file.is_open())
			{
				AERO_CORE_INFO("Cannot Open File: {0}", project_file_path);
			}
			in_file >> j;
			in_file.close();

			auto active_scene = Scene::get_active_scene();

			Helper::save_json(j["Scene"], "Last Loaded", active_scene->get_name());
			Helper::save_json(j["Scene"], "Last Loaded Path", active_scene->get_directory());

			std::ofstream out_file(project_file_path);
			if (!out_file.is_open())
			{
				AERO_CORE_INFO("Cannot Open File: {0}", project_file_path);
			}
			out_file << j.dump(4);
			out_file.close();
			Helper::makefile_read_only(project_file_path);
		}
	}

	void EditorApp::load_project_data()
	{
		json j;
		Helper::makefile_read_only(AppSettings::get_settings_path(), false);
		std::ifstream in_file(AppSettings::get_settings_path());

		if (!in_file.is_open())
		{
			AERO_CORE_INFO("Cannot Open File!");
			return;
		}
		in_file >> j;
		in_file.close();
		Helper::makefile_read_only(AppSettings::get_settings_path());

		std::string project_path;
		Helper::load_json(j["Project"], "Directory", project_path);
		auto project = Project::load_project(project_path);


		Helper::makefile_read_only(project->get_project_file_directory(), false);
		std::ifstream in_proj_file(project->get_project_file_directory());
		if (!in_proj_file.is_open())
		{
			AERO_CORE_INFO("Cannot Open File");
			return;
		}
		in_proj_file >> j;
		in_proj_file.close();
		Helper::makefile_read_only(project->get_project_file_directory());


		// Window Data Loaded and Created
		{
			WindowProps props;
			Helper::load_json(j["Window"], "Title", props.Title);
			Helper::load_json(j["Window"], "Size", props.Size);
			init(props);

			auto& window = Application::get().get_window();
			{
				bool fullscreen;
				Helper::load_json(j["Window"], "FullScreen", fullscreen);
				if (fullscreen)
					window.set_full_screen();
			}
			{
				bool center_window;
				Helper::load_json(j["Window"], "Center", center_window);
				if (center_window)
					window.center_window();
				else
				{
					vec2i position;
					Helper::load_json(j["Window"], "Position", position);
					window.set_position(position);
				}
			}
			{
				bool is_vsync;
				Helper::load_json(j["Window"], "Vsync", is_vsync);
				window.set_vsync(is_vsync);
			}
		}

		{
			std::string last_scene_path;

			Helper::load_json(j["Scene"], "Last Loaded Path", last_scene_path);

			std::string path = project->get_directory() + project->get_scene_directory()  + last_scene_path;

			auto scene = SaveScene::load_scene(path);
			Scene::set_active_scene(scene);
		}
	}
}