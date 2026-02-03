#include <Packing/GameExporter.hpp>
#include <Project/GamePacker.hpp>
#include <fstream>
#include <iomanip>
#include <Project/Project.hpp>
#include <Core/Application.hpp>
#include <Project/FileDialogs.hpp>
#include <Helper.hpp>





namespace ag
{
	GameExporter::GameExporter()
	{
		const char* userprofile = std::getenv("USERPROFILE");
		if (userprofile)
			m_output_folder = std::string(userprofile) + "/Desktop";
		else
			m_output_folder = ".";
	}
	
	GameExporter::~GameExporter()
	{
		if (m_export_thread.joinable())
			m_export_thread.join();
	}

	void GameExporter::refresh_project_info()
	{
		auto project = Project::get_active_project();
		if (project)
		{
			m_project_dir = project->get_directory();
			m_project_name = project->get_name();
		}
	}


	void GameExporter::start_export()
	{
		if (is_busy()) return;
		if (m_project_dir.empty())
		{
			m_error_message = "No project loaded!";
			m_status = Status::Failed;
			return;
		}

		m_log.clear();
		m_error_message.clear();
		m_progress = 0.0f;
		m_status = Status::Preparing;

		m_export_thread = std::thread([this]() { export_thread_func(); });
	}

	void GameExporter::browse_output_folder()
	{
		std::string path = ag::FileDialogs::select_folder("Select Output Folder");
		if(!path.empty())
		{
			m_output_folder = path;
		}
	}


  void GameExporter::export_thread_func()
  {
    namespace fs = std::filesystem;

    try
    {
      log_msg("Starting export...");
      m_status = Status::Preparing;

      m_temp_dir = fs::temp_directory_path().string() + "/aero_export_tmp"; // FIX: Missing slash?
      // Should be: fs::temp_directory_path().string() + "/aero_export_tmp"

      if (fs::exists(m_temp_dir))
        fs::remove_all(m_temp_dir);
      fs::create_directories(m_temp_dir);

      // DEBUG: List project files (keep this for debugging)
      log_msg("=== DEBUG: Listing project files ===");
      for (auto& entry : fs::recursive_directory_iterator(m_project_dir))
      {
        log_msg("  " + entry.path().string());
      }
      log_msg("=== Total files: " + std::to_string(std::distance(
        fs::recursive_directory_iterator(m_project_dir),
        fs::recursive_directory_iterator{})) + " ===");

      log_msg("Temp: " + m_temp_dir);
      m_progress = 0.1f;

      // ============================================
      // STEP 2: FIXED - Copy files INTO PROJECT FOLDER
      // ============================================
      log_msg("Copying project files...");
      uint32_t file_count = 0;

      // Create project folder inside temp
      fs::path project_temp_dir = fs::path(m_temp_dir) / m_project_name;
      fs::create_directories(project_temp_dir);

      for (auto& entry : fs::recursive_directory_iterator(m_project_dir))
      {
        if (entry.is_directory()) continue;

        // Get relative path
        fs::path rel = fs::relative(entry.path(), m_project_dir);

        // Convert to forward slashes for .pak file
        std::string rel_str = rel.string();
        std::replace(rel_str.begin(), rel_str.end(), '\\', '/');

        // Also log with forward slashes
        log_msg("  + " + rel_str);

        // Copy file (filesystem still needs native path)
        fs::path dest = project_temp_dir / rel;  // Keep native for filesystem

        fs::create_directories(dest.parent_path());
        fs::copy_file(entry.path(), dest, fs::copy_options::overwrite_existing);

        file_count++;
      }

      log_msg("Copied " + std::to_string(file_count) + " files.");
      m_progress = 0.3f;

      // ============================================
      // STEP 3: FIXED - Create proper settings.json
      // ============================================
      log_msg("Creating settings.json...");

      // settings.json goes at ROOT of temp folder (outside project folder)
      fs::path settings_path = fs::path(m_temp_dir) / "settings.json";

      // Create fresh settings.json for distribution
      json settings;
      auto project = Project::get_active_project();

      // Store project info for PACKED mode
      Helper::save_json(settings["Project"], "Name", m_project_name);
      Helper::save_json(settings["Project"], "Directory", m_project_name);
      Helper::save_json(settings["Project"], "File Path", m_project_name + "/" + m_project_name + ".aeroproj");
      Helper::save_json(settings["Project"], "Assets", project->get_assets_directory());
      Helper::save_json(settings["Project"], "Scene", project->get_scene_directory());
      Helper::save_json(settings["Project"], "Scripts", project->get_scripts_directory());

      // Also embed the .aeroproj config for easier loading
      fs::path proj_file_path = fs::path(project_temp_dir) / (m_project_name + ".aeroproj");
      if (fs::exists(proj_file_path))
      {
        std::ifstream proj_in(proj_file_path);
        json proj_config = json::parse(proj_in);
        proj_in.close();

        
        if (proj_config.contains("Scene"))
        {
          settings["Scene"]["Default"] = proj_config["Scene"]["Default"];
          settings["Scene"]["Default Path"] = proj_config["Scene"]["Default Path"];
        }
      }

      std::ofstream out(settings_path);
      out << std::setw(4) << settings << std::endl;
      out.close();

      log_msg("  Created settings.json with:");
      log_msg("    Project path: " + settings["Project"]["Directory"].get<std::string>());
      log_msg("    Project file: " + settings["Project"]["File Path"].get<std::string>());

      m_progress = 0.4f;

      // ============================================
      // DEBUG: Show temp structure before packing
      // ============================================
      log_msg("=== Temp folder structure before packing ===");
      for (auto& entry : fs::recursive_directory_iterator(m_temp_dir))
      {
        fs::path rel = fs::relative(entry.path(), m_temp_dir);
        std::string type = entry.is_directory() ? "[DIR] " : "[FILE]";
        log_msg("  " + type + " " + rel.string());
      }

      // STEP 4: Run the packer
      log_msg("Packing...");
      m_status = Status::Packing;

      fs::path pak_path = fs::path(m_temp_dir) / m_pak_name;
      auto result = GamePacker::pack(m_temp_dir, pak_path.string());

      if (!result.success)
      {
        m_error_message = "Packing failed!";
        m_status = Status::Failed;
        log_msg("ERROR: Packing failed!");
        return;
      }

      log_msg("Packed " + std::to_string(result.fileCount) + " files (" + std::to_string(result.totalSize) + " bytes)");
      m_progress = 0.7f;

      // STEP 5: Copy .pak to output folder
      log_msg("Copying to output...");
      m_status = Status::Copying;

      if (!fs::exists(m_output_folder))
        fs::create_directories(m_output_folder);

      fs::path final_pak = fs::path(m_output_folder) / m_pak_name;
      fs::copy_file(pak_path, final_pak, fs::copy_options::overwrite_existing);
      log_msg("  .pak -> " + final_pak.string());

      // DEBUG: Show pak file info
      log_msg("  Pak size: " + std::to_string(fs::file_size(final_pak)) + " bytes");

      m_progress = 0.85f;

      // STEP 6: Copy game exe to output folder
      log_msg("Looking for game exe...");

      std::vector<std::string> search_paths = {
              Application::get_exe_directory() + "/" + m_exe_name,
              Application::get_exe_directory() + "/Release/" + m_exe_name,
              Application::get_exe_directory() + "/Debug/" + m_exe_name,
              Application::get_exe_directory() + "/build/" + m_exe_name,
              Application::get_exe_directory() + "/bin/" + m_exe_name,
      };

      bool exe_found = false;
      for (auto& p : search_paths)
      {
        if (fs::exists(p))
        {
          fs::path final_exe = fs::path(m_output_folder) / m_exe_name;
          fs::copy_file(p, final_exe, fs::copy_options::overwrite_existing);
          log_msg("  .exe -> " + final_exe.string());
          exe_found = true;
          break;
        }
      }

      if (!exe_found)
      {
        log_msg("WARNING: " + m_exe_name + " not found.");
        log_msg("  Build your game exe first, then export again.");
        log_msg("  The .pak was still exported successfully.");
      }

      // STEP 7: Clean up
      fs::remove_all(m_temp_dir);
      log_msg("Cleaned up temp.");

      m_progress = 1.0f;
      m_status = Status::Done;
      m_status_message = "Done! Output: " + m_output_folder;
      log_msg("");
      log_msg("=== EXPORT COMPLETE ===");
      log_msg("Folder: " + m_output_folder);

      // Show what was created
      log_msg("Created files:");
      log_msg("  " + final_pak.string());
      if (exe_found)
        log_msg("  " + (fs::path(m_output_folder) / m_exe_name).string());
    }
    catch (const std::exception& e)
    {
      m_error_message = std::string("Export error: ") + e.what();
      m_status = Status::Failed;
      log_msg("ERROR: " + m_error_message);
    }

    if (m_export_thread.joinable())
      m_export_thread.detach();
  }
	
  void GameExporter::log_msg(const std::string& msg)
	{
		std::lock_guard<std::mutex> lock(m_log_mutex);
		m_log.push_back(msg);
	}

}