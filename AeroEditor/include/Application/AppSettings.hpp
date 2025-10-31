#pragma once

#include <Aero.hpp>

namespace ag
{
	class AppSettings
	{
	public:
		enum class Mode
		{
			Editor, ProjectManager
		};

		static Mode get_mode() { return s_mode; }
		static void set_node(Mode mode) { s_mode = mode; }

		static void load();

		static const std::string& get_settings_path() { return s_settings_path; }
		static const std::string& get_recent_projects_path() { return s_recent_projects_path; }

		static void reload_app();

	private:
		static std::string get_appdata_path();
		static void create_app_folder();
		static std::string get_exe_path();

	private:
		static Mode s_mode;
		static std::string s_settings_path;
		static std::string s_recent_projects_path;
	};
}