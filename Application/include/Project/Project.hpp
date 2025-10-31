#pragma once


#include <Core/Core.hpp>
#include <string>

namespace ag
{
	class Project
	{
	public:
		static AG_ref<Project> new_project(const std::string& path);
		static AG_ref<Project> load_project(const std::string& path);
		static AG_ref<Project> save_project();
		static void save_active();

		static AG_ref<Project> get_active_project() { return s_active_project; }
		static void set_active_project(const AG_ref<Project> project) { s_active_project = project; }
		
		void set_name(const std::string& name) { m_name = name; }
		void set_directory(const std::string& directory) { m_directory = directory; }
		void set_project_file_directory(const std::string& directory) { m_project_file_path = m_directory; }
		void set_scene_directory(const std::string& scene_path) { m_scenes_directory = scene_path; }
		void set_assets_directory(const std::string& assets_path) { m_assets_directory = assets_path; }
		void set_scripts_directory(const std::string& scripts_path) { m_scripts_directory = scripts_path; }

		const std::string& get_name() const { return m_name; }
		const std::string& get_directory() const { return m_directory; }
		const std::string& get_project_file_directory() const { return m_project_file_path; }
		const std::string& get_scene_directory() const { return m_scenes_directory; }
		const std::string& get_assets_directory() const { return m_assets_directory; }
		const std::string& get_scripts_directory() const { return m_scripts_directory; }
		

		void save();

	private:
		std::string m_name;
		std::string m_directory;
		std::string m_project_file_path;
		std::string m_assets_directory;
		std::string m_scenes_directory;
		std::string m_scripts_directory;


		inline static AG_ref<Project> s_active_project;
	};
}
