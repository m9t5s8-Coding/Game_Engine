#include <Editor/Autosave.hpp>

#include <Editor/EditorSerializer.hpp>
#include <Project/Project.hpp>
#include <filesystem>
#include <fstream>
#include <Helper.hpp>

namespace fs = std::filesystem;

namespace ag {
namespace {
std::string sanitize(const std::string& value) {
  std::string result;
  for (char c : value) {
    if (std::isalnum((unsigned char)c) || c == '_' || c == '-' || c == '.')
      result += c;
    else
      result += '_';
  }
  return result.empty() ? "scene" : result;
}
} // namespace

std::string EditorAutosave::directory() {
  auto project = Project::get_active_project();
  if (!project || project->get_directory().empty())
    return {};
  return (fs::path(project->get_directory()) / ".aero_autosave").string();
}

std::string EditorAutosave::path_for(AG_ref<Scene> scene) {
  if (!scene) return {};
  const std::string dir = directory();
  if (dir.empty()) return {};
  return (fs::path(dir) / (sanitize(scene->get_name()) + ".json")).string();
}

bool EditorAutosave::write_scene(AG_ref<Scene> scene, std::string* path_out) {
  if (!scene) return false;
  const std::string path = path_for(scene);
  if (path.empty()) return false;
  try {
    fs::create_directories(fs::path(path).parent_path());
    Helper::makefile_read_only(path, false);
    std::ofstream file(path, std::ios::trunc);
    if (!file.is_open()) return false;
    file << EditorSerializer::snapshot_scene(scene).dump(4);
    file.close();
    Helper::makefile_read_only(path);
    if (path_out) *path_out = path;
    return true;
  } catch (const std::exception& e) {
    AERO_CORE_ERROR("Autosave failed for {0}: {1}", scene->get_name(), e.what());
    return false;
  }
}

void EditorAutosave::remove_scene(const std::string& scene_name) {
  const std::string dir = directory();
  if (dir.empty()) return;
  std::error_code ec;
  fs::remove(fs::path(dir) / (sanitize(scene_name) + ".json"), ec);
}

void EditorAutosave::clear_all() {
  const std::string dir = directory();
  if (dir.empty()) return;
  std::error_code ec;
  fs::remove_all(dir, ec);
}

std::vector<std::string> EditorAutosave::list_files() {
  std::vector<std::string> files;
  const std::string dir = directory();
  if (dir.empty()) return files;
  std::error_code ec;
  for (auto it = fs::directory_iterator(dir, ec); !ec && it != fs::directory_iterator(); it.increment(ec)) {
    if (it->is_regular_file() && it->path().extension() == ".json")
      files.push_back(it->path().string());
  }
  std::sort(files.begin(), files.end());
  return files;
}

std::vector<std::string> EditorAutosave::list_names() {
  std::vector<std::string> names;
  for (const auto& file : list_files()) {
    names.push_back(fs::path(file).stem().string());
  }
  return names;
}

bool EditorAutosave::has_recovery_data() {
  return !list_files().empty();
}

AG_ref<Scene> EditorAutosave::load_recovery(const std::string& file) {
  if (file.empty() || !fs::exists(file)) return {};
  try {
    Helper::makefile_read_only(file, false);
    std::ifstream input(file);
    if (!input.is_open()) return {};
    nlohmann::json j = nlohmann::json::parse(input, nullptr, false);
    input.close();
    Helper::makefile_read_only(file);
    if (j.is_discarded() || !j.contains("Scene")) return {};

    std::string name = j["Scene"].value("Name", std::string("Recovered Scene"));
    std::string path = j["Scene"].value("Path", std::string(""));
    auto scene = Scene::create(name, path);
    if (!EditorSerializer::restore_scene_snapshot(scene, j)) return {};
    scene->set_save_required(true);
    return scene;
  } catch (const std::exception& e) {
    AERO_CORE_ERROR("Failed to load recovery file {0}: {1}", file, e.what());
    return {};
  }
}

} // namespace ag
