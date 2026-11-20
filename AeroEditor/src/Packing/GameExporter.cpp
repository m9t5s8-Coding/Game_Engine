#include <Core/Application.hpp>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <Helper.hpp>
#include <iomanip>
#include <Packing/GameExporter.hpp>
#include <Project/FileDialogs.hpp>
#include <Project/GamePacker.hpp>
#include <Project/Project.hpp>

namespace ag
{
namespace
{
namespace fs = std::filesystem;

nlohmann::json build_settings_json(const Project& project)
{
  json settings;

  Helper::save_json(settings["Project"], "Name", project.get_name(), std::string(""));
  Helper::save_json(settings["Project"], "Directory", project.get_name(), std::string(""));
  Helper::save_json(settings["Project"],
                    "File Path",
                    project.get_name() + "/" + project.get_name() + ".aeroproj",
                    std::string(""));
  Helper::save_json(settings["Project"],
                    "Assets",
                    project.get_assets_directory(),
                    std::string(""));
  Helper::save_json(settings["Project"],
                    "Scene",
                    project.get_scene_directory(),
                    std::string(""));
  Helper::save_json(settings["Project"],
                    "Scripts",
                    project.get_scripts_directory(),
                    std::string(""));

  Helper::save_json(settings["Network"], "Enabled", project.get_server_config().enabled, false);
  if (project.get_server_config().enabled)
  {
    Helper::save_json(settings["Network"],
                      "Port",
                      project.get_server_config().port,
                      (uint16_t)7777);
    Helper::save_json(settings["Network"],
                      "MaxClients",
                      project.get_server_config().max_clients,
                      5);
    Helper::save_json(settings["Network"],
                      "TickRate",
                      project.get_server_config().tick_rate,
                      20);
    Helper::save_json(settings["Network"],
                      "ConnectionTimeout",
                      project.get_server_config().connection_timeout,
                      10);
    Helper::save_json(settings["Network"],
                      "Reconnect",
                      project.get_server_config().auto_reconnect,
                      true);
    Helper::save_json(settings["Network"],
                      "ReconnectTries",
                      project.get_server_config().max_reconnection_tries,
                      3);
    Helper::save_json(settings["Network"],
                      "ServerIP",
                      project.get_server_config().server_IP,
                      std::string("127.0.0.1"));
    settings["Network"]["scripts"] = project.get_server_config().scripts;
  }

  settings["Global"]["scripts"] = project.get_global_scripts().global_scripts;

  // The runtime reads the default scene from the project file inside the pak.
  const std::string project_file = project.get_project_file_directory();
  std::error_code   ec;
  if (!project_file.empty() && fs::exists(project_file, ec))
  {
    std::ifstream input(project_file);
    if (input)
    {
      json project_config = json::parse(input, nullptr, false);
      if (!project_config.is_discarded() && project_config.contains("Scene"))
      {
        settings["Scene"]["Default"] =
            project_config["Scene"].value("Default", std::string(""));
        settings["Scene"]["Default Path"] =
            project_config["Scene"].value("Default Path", std::string(""));
      }
    }
  }

  return settings;
}

struct TempDirCleanup {
  fs::path path;
  ~TempDirCleanup()
  {
    std::error_code ec;
    if (!path.empty())
      fs::remove_all(path, ec);
  }
};
}  // namespace

GameExporter::GameExporter() {
  const char* userprofile = std::getenv("USERPROFILE");
  if (userprofile)
    m_config.output_folder = std::string(userprofile) + "/Desktop";
  else
    m_config.output_folder = ".";
}

GameExporter::~GameExporter()
{
  // Never leave a joinable future behind; wait for any in-flight export before
  // the exporter (and its config buffers) are destroyed.
  if (m_export_future.valid())
  {
    m_export_future.wait();
    m_export_future.get();
  }
}

GameExporter::Config GameExporter::snapshot_config() const
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  return m_config;
}

void GameExporter::refresh_project_info()
{
  auto project = Project::get_active_project();
  if (!project)
    return;

  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_config.project_dir  = project->get_directory();
  m_config.project_name = project->get_name();
}

std::vector<std::string> GameExporter::get_log() const
{
  std::lock_guard<std::mutex> lock(m_log_mutex);
  return m_log;
}

std::string GameExporter::get_project_name() const
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  return m_config.project_name;
}

std::string GameExporter::get_project_dir() const
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  return m_config.project_dir;
}

std::string GameExporter::get_output_folder() const
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  return m_config.output_folder;
}

std::string GameExporter::get_pak_name() const
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  return m_config.pak_name;
}

std::string GameExporter::get_exe_name() const
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  return m_config.exe_name;
}

void GameExporter::set_project_name(const std::string& name)
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_config.project_name = name;
}

void GameExporter::set_project_dir(const std::string& dir)
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_config.project_dir = dir;
}

void GameExporter::set_output_folder(const std::string& folder)
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_config.output_folder = folder;
}

void GameExporter::set_pak_name(const std::string& name)
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_config.pak_name = name;
}

void GameExporter::set_exe_name(const std::string& name)
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_config.exe_name = name;
}

void GameExporter::clear_status()
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_status         = Status::Idle;
  m_progress       = 0.0f;
  m_error_message.clear();
  m_status_message.clear();
}

std::string GameExporter::get_error_message() const
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  return m_error_message;
}

GameExporter::Status GameExporter::get_status() const
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  return m_status;
}

float GameExporter::get_progress() const
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  return m_progress;
}

std::string GameExporter::get_status_message() const
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  return m_status_message;
}

void GameExporter::set_status(Status status)
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_status = status;
}

void GameExporter::set_progress(float progress)
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_progress = progress;
}

void GameExporter::set_error(const std::string& message)
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_error_message = message;
}

void GameExporter::set_result_message(const std::string& message)
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_status_message = message;
}

void GameExporter::finish_if_complete()
{
  if (!m_running || !m_export_future.valid())
    return;
  if (m_export_future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
    return;

  // Propagate exceptions from the worker instead of leaving a broken promise.
  try
  {
    m_export_future.get();
  }
  catch (const std::exception& e)
  {
    set_error(std::string("Export error: ") + e.what());
    set_status(Status::Failed);
    log_msg("ERROR: " + std::string(e.what()));
  }
  m_running = false;
}

bool GameExporter::is_busy()
{
  finish_if_complete();
  Status status = get_status();
  return status != Status::Idle && status != Status::Done && status != Status::Failed;
}

void GameExporter::start_export()
{
  if (is_busy())
    return;

  // Reap a completed export before starting another one.
  if (m_export_future.valid())
  {
    m_export_future.wait();
    m_export_future.get();
  }

  Config config = snapshot_config();
  if (config.project_dir.empty())
  {
    set_error("No project loaded!");
    set_status(Status::Failed);
    return;
  }

  auto project = Project::get_active_project();
  if (!project)
  {
    set_error("No project loaded!");
    set_status(Status::Failed);
    return;
  }

  config.settings = build_settings_json(*project);

  {
    std::lock_guard<std::mutex> lock(m_log_mutex);
    m_log.clear();
  }
  set_error("");
  set_result_message("");
  set_progress(0.0f);
  set_status(Status::Preparing);
  m_running = true;

  m_export_future = std::async(std::launch::async, [this, config]() { export_thread_func(config); });
}

void GameExporter::browse_output_folder()
{
  std::string path = ag::FileDialogs::select_folder("Select Output Folder");
  if (!path.empty())
    set_output_folder(path);
}

void GameExporter::export_thread_func(Config config)
{
  try
  {
    log_msg("Starting export...");
    set_status(Status::Preparing);

    // Unique temp directory avoids collisions between editor instances.
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    m_temp_dir       = (fs::temp_directory_path() /
                  ("aero_export_tmp_" + std::to_string(stamp)))
                     .string();
    TempDirCleanup cleanup{m_temp_dir};

    fs::create_directories(m_temp_dir);

    log_msg("=== Project files ===");
    uint32_t source_count = 0;
    for (fs::recursive_directory_iterator it(config.project_dir), end; it != end; ++it)
    {
      if (it->is_regular_file())
      {
        log_msg("  " + fs::relative(it->path(), config.project_dir).string());
        ++source_count;
      }
    }
    log_msg("Total files: " + std::to_string(source_count));
    log_msg("  -> Temp: " + m_temp_dir);
    set_progress(0.1f);

    log_msg("=== Copying project files ===");
    const fs::path project_temp_dir = fs::path(m_temp_dir) / config.project_name;
    fs::create_directories(project_temp_dir);

    uint32_t file_count = 0;
    for (fs::recursive_directory_iterator it(config.project_dir), end; it != end; ++it)
    {
      if (!it->is_regular_file())
        continue;

      const fs::path rel = fs::relative(it->path(), config.project_dir);
      fs::path       dest = project_temp_dir / rel;
      fs::create_directories(dest.parent_path());

      std::error_code ec;
      fs::copy_file(it->path(), dest, fs::copy_options::overwrite_existing, ec);
      if (ec)
      {
        log_msg("WARNING: Could not copy " + rel.string() + ": " + ec.message());
        continue;
      }

      std::string rel_str = rel.string();
      std::replace(rel_str.begin(), rel_str.end(), '\\', '/');
      log_msg("  + " + rel_str);
      ++file_count;
    }
    log_msg("Copied " + std::to_string(file_count) + " files.");
    set_progress(0.3f);

    log_msg("=== Creating settings.json ===");
    const fs::path settings_path = fs::path(m_temp_dir) / "settings.json";
    std::ofstream  out(settings_path);
    if (!out)
      throw std::runtime_error("Failed to create settings.json");
    out << std::setw(4) << config.settings << std::endl;
    out.close();
    log_msg("    Project path: " +
            config.settings["Project"].value("Directory", std::string("")));
    log_msg("    Project file: " +
            config.settings["Project"].value("File Path", std::string("")));
    set_progress(0.4f);

    log_msg("=== Packing ===");
    set_status(Status::Packing);
    const fs::path pak_path = fs::path(m_temp_dir) / config.pak_name;
    auto           result   = GamePacker::pack(m_temp_dir, pak_path.string());
    if (!result.success)
    {
      const std::string message =
          result.error.empty() ? "Packing failed!" : "Packing failed: " + result.error;
      set_error(message);
      set_status(Status::Failed);
      log_msg("ERROR: " + message);
      return;
    }
    log_msg("Packed " + std::to_string(result.fileCount) + " files (" +
            std::to_string(result.totalSize) + " bytes)");
    set_progress(0.7f);

    log_msg("=== Copying to output ===");
    set_status(Status::Copying);
    fs::create_directories(config.output_folder);

    const fs::path final_pak = fs::path(config.output_folder) / config.pak_name;
    fs::copy_file(pak_path, final_pak, fs::copy_options::overwrite_existing);
    log_msg("  .pak -> " + final_pak.string());
    log_msg("  Pak size: " + std::to_string(fs::file_size(final_pak)) + " bytes");
    set_progress(0.85f);

    log_msg("=== Locating runtime ===");
    const std::string& exe_dir = Application::get_exe_directory();
    const std::vector<std::string> search_paths = {
        exe_dir + "/" + config.exe_name,
        exe_dir + "/Release/" + config.exe_name,
        exe_dir + "/Debug/" + config.exe_name,
        exe_dir + "/build/full/bin/" + config.exe_name,
        exe_dir + "/build/Release/bin/" + config.exe_name,
        exe_dir + "/build/Debug/bin/" + config.exe_name,
        exe_dir + "/bin/" + config.exe_name,
        config.project_dir + "/" + config.exe_name,
    };

    bool exe_found = false;
    for (const auto& candidate : search_paths)
    {
      std::error_code ec;
      if (!fs::exists(candidate, ec))
        continue;

      const fs::path exe_path = fs::path(candidate);
      const fs::path out_dir  = fs::path(config.output_folder);
      const fs::path final_exe = out_dir / config.exe_name;
      fs::copy_file(exe_path, final_exe, fs::copy_options::overwrite_existing, ec);
      if (ec)
      {
        log_msg("WARNING: Could not copy runtime " + exe_path.string() + ": " + ec.message());
        break;
      }
      log_msg("  exe -> " + final_exe.string());

      // Ship engine data paks next to the runtime. Never overwrite the game pak
      // that was just exported.
      for (fs::directory_iterator it(exe_path.parent_path(), ec), end; !ec && it != end; it.increment(ec))
      {
        if (!it->is_regular_file() || it->path().extension() != ".pak")
          continue;
        if (it->path().filename() == fs::path(config.pak_name).filename())
          continue;
        const fs::path dest = out_dir / it->path().filename();
        fs::copy_file(it->path(), dest, fs::copy_options::overwrite_existing, ec);
        log_msg("  pak -> " + dest.string());
      }

      exe_found = true;
      break;
    }

    if (!exe_found)
    {
      log_msg("WARNING: " + config.exe_name + " was not found.");
      log_msg("  Build the Sandbox/runtime target, then export again.");
      log_msg("  The .pak was still exported successfully.");
    }

    set_progress(1.0f);
    set_status(Status::Done);
    set_result_message("Done! Output: " + config.output_folder);
    log_msg("");
    log_msg("=== EXPORT COMPLETE ===");
    log_msg("Folder: " + config.output_folder);
    log_msg("Created files:");
    log_msg("  " + final_pak.string());
    if (exe_found)
      log_msg("  " + (fs::path(config.output_folder) / config.exe_name).string());
    log_msg("  Engine data paks copied next to the executable when available.");
  }
  catch (const std::exception& e)
  {
    const std::string message = std::string("Export error: ") + e.what();
    set_error(message);
    set_status(Status::Failed);
    log_msg("ERROR: " + message);
  }
}

void GameExporter::log_msg(const std::string& msg)
{
  std::lock_guard<std::mutex> lock(m_log_mutex);
  m_log.push_back(msg);
  if (m_log.size() > 2000)
    m_log.erase(m_log.begin(), m_log.begin() + (m_log.size() - 2000));
}

}  // namespace ag
