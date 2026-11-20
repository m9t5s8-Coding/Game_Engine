#include "imgui.h"
#include "imgui_internal.h"

#include <Aero.hpp>
#include <icons.h>

#include <Application/EditorLayer.hpp>
#include <Editor/UndoRedo.hpp>
#include <Font/FontImporter.hpp>
#include <Font/FontManager.hpp>
#include <Project/FileDialogs.hpp>
#include <Project/Project.hpp>
#include <UI/UI.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <system_error>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

namespace ag
{
namespace
{
constexpr const char* k_content_payload = "CONTENT_BROWSER_ITEM";
constexpr const char* k_file_payload    = "DND_FILENAME";

enum class CreateKind
{
  Folder,
  Scene,
  Script
};

struct ContentBrowserState
{
  fs::path              project_root;
  fs::path              current;
  fs::path              selected;
  std::vector<fs::path> history;
  size_t                history_pos = 0;
  char                  search[256] = {};
  bool                  list_view   = false;

  bool     rename_active = false;
  fs::path rename_target;
  char     rename_buffer[256] = {};

  bool     delete_active = false;
  fs::path delete_target;

  bool        create_active = false;
  CreateKind  create_kind   = CreateKind::Folder;
  fs::path    create_directory;
  char        create_buffer[256] = {};

  std::string status_message;
};

ContentBrowserState& state()
{
  static ContentBrowserState s;
  return s;
}

std::unordered_map<std::string, AG_ref<Texture2D>>& thumbnail_cache()
{
  static std::unordered_map<std::string, AG_ref<Texture2D>> cache;
  return cache;
}

std::string normalized_string(const fs::path& path)
{
  std::string result = path.string();
  std::replace(result.begin(), result.end(), '\\', '/');
  while (result.size() > 1 && result.back() == '/')
    result.pop_back();
  return result;
}

fs::path normalized(const fs::path& path)
{
  return fs::path(normalized_string(path));
}

std::string lowercase(std::string value)
{
  std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c)
                 { return static_cast<char>(std::tolower(c)); });
  return value;
}

constexpr std::array<ProjectDirectory, 3> k_managed_directories = {
    ProjectDirectory::Assets, ProjectDirectory::Scenes, ProjectDirectory::Scripts};

std::optional<ProjectDirectory> category_of(const fs::path& path)
{
  const std::string value = normalized_string(path);
  if (value.empty())
    return std::nullopt;

  for (ProjectDirectory kind : k_managed_directories)
  {
    if (Project::is_path_in_active_directory(value, kind))
      return kind;
  }
  return std::nullopt;
}

bool is_managed_root(const fs::path& path)
{
  const fs::path value = normalized(path);
  for (ProjectDirectory kind : k_managed_directories)
  {
    if (normalized(Project::get_active_directory_path(kind)) == value)
      return true;
  }
  return false;
}

fs::path managed_root(ProjectDirectory kind)
{
  return normalized(Project::get_active_directory_path(kind));
}

bool is_inside(const fs::path& root, const fs::path& path)
{
  const fs::path relative = normalized(path).lexically_relative(normalized(root));
  if (relative.empty())
    return false;
  for (const auto& part : relative)
  {
    if (part == "..")
      return false;
  }
  return true;
}

void ensure_state()
{
  auto& s = state();
  auto  project = Project::get_active_project();
  if (!project)
  {
    s.project_root.clear();
    s.current.clear();
    s.selected.clear();
    s.history.clear();
    s.history_pos = 0;
    return;
  }

  const fs::path root = normalized(project->get_directory());
  if (root != s.project_root)
  {
    s.project_root = root;
    s.current      = root;
    s.selected.clear();
    s.history.clear();
    s.history.push_back(root);
    s.history_pos = 0;
    s.search[0]   = '\0';
    thumbnail_cache().clear();
  }

  std::error_code ec;
  if (s.current.empty() || !fs::is_directory(s.current, ec) || !is_inside(root, s.current))
  {
    s.current = root;
    s.history.clear();
    s.history.push_back(root);
    s.history_pos = 0;
  }
  if (s.history_pos >= s.history.size())
    s.history_pos = s.history.empty() ? 0 : s.history.size() - 1;
}

void navigate_to(const fs::path& path)
{
  auto& s = state();
  std::error_code ec;
  if (path.empty() || !fs::is_directory(path, ec) || !is_inside(s.project_root, path))
    return;

  const fs::path destination = normalized(path);
  if (s.history_pos + 1 < s.history.size())
    s.history.resize(s.history_pos + 1);
  if (s.history.empty() || s.history.back() != destination)
    s.history.push_back(destination);
  s.history_pos = s.history.size() - 1;
  s.current     = destination;
  s.selected.clear();
}

void navigate_history(bool backward)
{
  auto& s = state();
  if (backward)
  {
    if (s.history_pos == 0)
      return;
    --s.history_pos;
  }
  else
  {
    if (s.history_pos + 1 >= s.history.size())
      return;
    ++s.history_pos;
  }
  s.current = s.history[s.history_pos];
  s.selected.clear();
}

void navigate_up()
{
  auto& s = state();
  if (s.current == s.project_root || s.current.parent_path() == s.current)
    return;
  navigate_to(s.current.parent_path());
}

fs::path target_directory_for(ProjectDirectory kind, const fs::path& hint)
{
  const fs::path root = managed_root(kind);
  if (root.empty())
    return {};

  std::error_code ec;
  fs::create_directories(root, ec);

  if (!hint.empty() && fs::is_directory(hint, ec) && category_of(hint) == kind)
    return normalized(hint);
  return root;
}

void replace_all(std::string& text, const std::string& from, const std::string& to)
{
  if (from.empty())
    return;
  size_t pos = 0;
  while ((pos = text.find(from, pos)) != std::string::npos)
  {
    text.replace(pos, from.size(), to);
    pos += to.size();
  }
}

std::string project_relative(const fs::path& path)
{
  auto kind = category_of(path);
  if (!kind)
    return {};
  return Project::to_active_relative_path(normalized_string(path), *kind);
}

void update_references(const fs::path& old_path, const fs::path& new_path)
{
  const std::string old_relative = project_relative(old_path);
  const std::string new_relative = project_relative(new_path);
  if (old_relative.empty() || new_relative.empty() || old_relative == new_relative)
    return;

  const bool is_directory = fs::is_directory(new_path);
  std::error_code ec;

  for (fs::recursive_directory_iterator it(state().project_root, ec), end; it != end; it.increment(ec))
  {
    if (ec || !it->is_regular_file(ec))
      continue;

    const std::string extension = it->path().extension().string();
    if (extension != ".aeroscene" && extension != ".aeroproj" && extension != ".json")
      continue;

    std::ifstream input(it->path(), std::ios::binary);
    if (!input)
      continue;
    std::ostringstream buffer;
    buffer << input.rdbuf();
    std::string content = buffer.str();
    if (content.find('\0') != std::string::npos)
      continue;

    const std::string original = content;
    if (is_directory)
    {
      const std::string old_without_slash = old_relative.substr(1);
      const std::string new_without_slash = new_relative.substr(1);
      replace_all(content, "\"" + old_relative + "/", "\"" + new_relative + "/");
      replace_all(content,
                  "\"" + old_without_slash + "/",
                  "\"" + new_without_slash + "/");
    }
    else
    {
      replace_all(content, "\"" + old_relative + "\"", "\"" + new_relative + "\"");
      replace_all(content,
                  "\"" + old_relative.substr(1) + "\"",
                  "\"" + new_relative.substr(1) + "\"");
    }

    if (content != original)
    {
      std::ofstream output(it->path(), std::ios::binary | std::ios::trunc);
      if (output)
        output << content;
    }
  }
}

fs::path unique_destination(const fs::path& destination)
{
  if (!fs::exists(destination))
    return destination;

  const fs::path parent = destination.parent_path();
  const std::string stem = destination.stem().string();
  const std::string ext  = destination.extension().string();
  for (int index = 1; index < 10000; ++index)
  {
    fs::path candidate = parent / (stem + "_" + std::to_string(index) + ext);
    if (!fs::exists(candidate))
      return candidate;
  }
  return destination;
}

bool copy_path(const fs::path& source, const fs::path& destination, std::error_code& ec)
{
  fs::create_directories(destination.parent_path(), ec);
  if (ec)
    return false;
  if (fs::is_directory(source, ec))
    fs::copy(source, destination, fs::copy_options::recursive, ec);
  else
    fs::copy_file(source, destination, fs::copy_options::overwrite_existing, ec);
  return !ec;
}

void remove_path(const fs::path& path, std::error_code& ec)
{
  if (fs::is_directory(path, ec))
    fs::remove_all(path, ec);
  else
    fs::remove(path, ec);
}

fs::path next_backup_path()
{
  const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
  return fs::temp_directory_path() / ("aero_asset_undo_" + std::to_string(stamp));
}

void push_asset_command(const std::string& name, std::function<void()> undo, std::function<void()> redo)
{
  UndoRedoManager::get().push(
      std::make_unique<FunctionCommand>(name, std::move(undo), std::move(redo)));
}

bool apply_rename(const fs::path& source, const fs::path& destination)
{
  std::error_code ec;
  fs::rename(source, destination, ec);
  if (ec)
  {
    state().status_message = "Rename failed: " + ec.message();
    AERO_CORE_ERROR("Failed to rename \"{0}\": {1}", source.string(), ec.message());
    return false;
  }

  update_references(source, destination);
  auto& s = state();
  if (s.selected == source)
    s.selected = destination;
  if (s.current == source || is_inside(source, s.current))
  {
    const fs::path relative = source.lexically_relative(s.current);
    s.current               = normalized(destination / relative);
    s.history.push_back(s.current);
    s.history_pos = s.history.size() - 1;
  }
  s.status_message = "Renamed to " + destination.filename().string();
  return true;
}

bool apply_move_exact(const fs::path& source, const fs::path& destination)
{
  std::error_code ec;
  fs::rename(source, destination, ec);
  if (ec)
  {
    if (fs::is_directory(source, ec))
      fs::copy(source, destination, fs::copy_options::recursive, ec);
    else
      fs::copy_file(source, destination, fs::copy_options::overwrite_existing, ec);
    if (!ec)
      remove_path(source, ec);
  }
  if (ec)
  {
    state().status_message = "Move failed: " + ec.message();
    AERO_CORE_ERROR("Failed to move \"{0}\": {1}", source.string(), ec.message());
    return false;
  }

  update_references(source, destination);
  auto& s = state();
  if (s.selected == source)
    s.selected = destination;
  if (s.current == source || is_inside(source, s.current))
  {
    const fs::path relative = source.lexically_relative(s.current);
    s.current               = normalized(destination / relative);
    s.history.push_back(s.current);
    s.history_pos = s.history.size() - 1;
  }
  s.status_message = "Moved " + destination.filename().string();
  return true;
}

void import_file(const fs::path& source, const fs::path& destination_directory)
{
  std::error_code ec;
  if (!fs::exists(source, ec))
    return;

  const std::string extension = lowercase(source.extension().string());
  std::optional<ProjectDirectory> kind;
  if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp" ||
      extension == ".tga" || extension == ".hdr" || extension == ".tif" || extension == ".tiff" ||
      extension == ".webp" || extension == ".gif" || extension == ".psd" || extension == ".svg" ||
      extension == ".mp3" || extension == ".wav" || extension == ".ogg" || extension == ".flac" ||
      extension == ".ttf" || extension == ".otf")
    kind = ProjectDirectory::Assets;
  else if (extension == ".lua")
    kind = ProjectDirectory::Scripts;
  else if (extension == ".aeroscene" || extension == ".scene" || extension == ".prefab")
    kind = ProjectDirectory::Scenes;
  else
    kind = category_of(destination_directory);

  if (!kind)
    kind = ProjectDirectory::Assets;

  const fs::path directory = target_directory_for(*kind, destination_directory);
  if (directory.empty())
    return;

  const fs::path destination = unique_destination(directory / source.filename());
  if (fs::is_directory(source, ec))
    fs::copy(source, destination, fs::copy_options::recursive, ec);
  else
    fs::copy_file(source, destination, ec);

  if (ec)
  {
    AERO_CORE_ERROR("Failed to import \"{0}\": {1}", source.string(), ec.message());
    state().status_message = "Import failed: " + ec.message();
    return;
  }

  if ((extension == ".ttf" || extension == ".otf") &&
      Project::is_path_in_active_directory(destination.string(), ProjectDirectory::Assets))
  {
    FontImportResult result = FontImporter::import_font(destination.string());
    state().status_message  = result.message;
    FontManager::reload();
  }
  else
  {
    state().status_message = "Imported " + destination.filename().string();
  }

  push_asset_command("Import asset",
                     [destination]()
                     {
                       std::error_code undo_ec;
                       remove_path(destination, undo_ec);
                       state().status_message = "Undid import of " + destination.filename().string();
                     },
                     [source, destination]()
                     {
                       std::error_code redo_ec;
                       copy_path(source, destination, redo_ec);
                       state().status_message = "Redid import of " + destination.filename().string();
                     });
}

void move_item(const fs::path& source, const fs::path& destination_directory)
{
  auto& s = state();
  std::error_code ec;

  if (source.empty() || destination_directory.empty() || !fs::exists(source, ec) ||
      !fs::is_directory(destination_directory, ec))
    return;

  const fs::path from = normalized(source);
  const fs::path to   = normalized(destination_directory);
  const auto source_kind      = category_of(from);
  const auto destination_kind = category_of(to);
  if (!source_kind || !destination_kind || *source_kind != *destination_kind)
  {
    s.status_message = "Assets can only be moved inside the same project folder.";
    AERO_CORE_ERROR("Refusing to move \"{0}\" outside its managed project folder.", from.string());
    return;
  }

  const bool source_is_directory = fs::is_directory(from, ec);
  if (from == to || (source_is_directory && is_inside(from, to)))
    return;

  const fs::path destination = unique_destination(to / from.filename());

  if (!apply_move_exact(from, destination))
    return;

  push_asset_command("Move asset",
                     [from, destination]()
                     {
                       apply_move_exact(destination, from);
                       state().status_message = "Undid move of " + destination.filename().string();
                     },
                     [from, destination]()
                     {
                       apply_move_exact(from, destination);
                     });
}

std::vector<std::string> parse_payload(const ImGuiPayload* payload)
{
  std::vector<std::string> values;
  if (!payload || !payload->Data || payload->DataSize == 0)
    return values;

  const char* data = static_cast<const char*>(payload->Data);
  size_t      offset = 0;
  while (offset < payload->DataSize)
  {
    const std::string value(data + offset);
    if (value.empty())
      break;
    values.push_back(value);
    offset += value.size() + 1;
  }
  return values;
}

void open_item(const fs::path& path)
{
  auto& s = state();
  if (path.empty())
    return;

  if (fs::is_directory(path))
  {
    navigate_to(path);
    s.selected = path;
    return;
  }

  s.selected = path;
  const std::string extension = lowercase(path.extension().string());
  if (extension == ".aeroscene")
  {
    std::string scene_path = path.string();
    EditorLayer::get().open_scene(scene_path);
  }
  else if (extension == ".lua")
  {
    UI::open_in_vscode(path.string());
  }
  else
  {
    UI::show_in_explorer(path.string());
  }
}

void do_rename()
{
  auto& s = state();
  std::string name = s.rename_buffer;
  name.erase(0, name.find_first_not_of(" \t\r\n"));
  name.erase(name.find_last_not_of(" \t\r\n") + 1);
  if (name.empty() || name.find('/') != std::string::npos || name.find('\\') != std::string::npos)
  {
    s.status_message = "Enter a valid name (path separators are not allowed).";
    return;
  }

  const fs::path source = s.rename_target;
  if (source.empty() || !fs::exists(source))
    return;

  const fs::path destination = source.parent_path() / name;
  if (destination != source && fs::exists(destination))
  {
    s.status_message = "An item with that name already exists.";
    return;
  }

  if (!apply_rename(source, destination))
    return;

  push_asset_command("Rename asset",
                     [source, destination]()
                     {
                       apply_rename(destination, source);
                       state().status_message = "Undid rename of " + destination.filename().string();
                     },
                     [source, destination]()
                     {
                       apply_rename(source, destination);
                     });
  s.rename_active = false;
}

void write_script_template(const fs::path& path, const std::string& name)
{
  std::ofstream file(path);
  if (!file)
    return;
  file << "--- " << name << ".lua ---\n\n";
  file << "local entity = get_entity()\n\n";
  file << "function on_create()\n  aero_print(\"" << name << " created\")\nend\n\n";
  file << "function on_update(dt)\nend\n\n";
  file << "function on_destroy()\nend\n";
}

void do_create()
{
  auto& s = state();
  std::string name = s.create_buffer;
  name.erase(0, name.find_first_not_of(" \t\r\n"));
  name.erase(name.find_last_not_of(" \t\r\n") + 1);
  if (name.empty() || name.find('/') != std::string::npos || name.find('\\') != std::string::npos)
  {
    s.status_message = "Enter a valid name (path separators are not allowed).";
    return;
  }

  ProjectDirectory kind = ProjectDirectory::Assets;
  std::string      extension;
  switch (s.create_kind)
  {
    case CreateKind::Scene:
      kind      = ProjectDirectory::Scenes;
      extension = ".aeroscene";
      break;
    case CreateKind::Script:
      kind      = ProjectDirectory::Scripts;
      extension = ".lua";
      break;
    default:
      kind = category_of(s.create_directory).value_or(ProjectDirectory::Assets);
      break;
  }

  const fs::path directory = target_directory_for(kind, s.create_directory);
  fs::path       target    = directory / name;
  if (!extension.empty() && target.extension() != extension)
    target.replace_extension(extension);

  std::error_code ec;
  if (fs::exists(target, ec))
  {
    s.status_message = "An item with that name already exists.";
    return;
  }

  switch (s.create_kind)
  {
    case CreateKind::Folder:
      fs::create_directories(target, ec);
      break;
    case CreateKind::Scene:
      EditorLayer::get().create_new_scene(target.string());
      break;
    case CreateKind::Script:
      write_script_template(target, name);
      Scene::save_required();
      break;
  }

  if (ec)
  {
    s.status_message = "Create failed: " + ec.message();
    return;
  }

  s.selected      = target;
  s.status_message = "Created " + target.filename().string();
  s.create_active  = false;

  const CreateKind create_kind = s.create_kind;
  push_asset_command("Create asset",
                     [target]()
                     {
                       std::error_code undo_ec;
                       remove_path(target, undo_ec);
                       state().status_message = "Undid creation of " + target.filename().string();
                     },
                     [target, name, create_kind]()
                     {
                       std::error_code redo_ec;
                       if (create_kind == CreateKind::Folder)
                         fs::create_directories(target, redo_ec);
                       else if (create_kind == CreateKind::Script)
                         write_script_template(target, name);
                       else
                         EditorLayer::get().create_new_scene_internal(target.string());
                       state().status_message = "Redid creation of " + target.filename().string();
                     });
}

void do_delete()
{
  auto& s = state();
  const fs::path target = s.delete_target;
  if (target.empty() || !fs::exists(target))
  {
    s.delete_active = false;
    return;
  }

  std::error_code ec;
  const fs::path  backup = next_backup_path();
  if (!copy_path(target, backup, ec))
  {
    s.status_message = "Delete failed: " + ec.message();
    return;
  }
  remove_path(target, ec);

  if (ec)
  {
    s.status_message = "Delete failed: " + ec.message();
    return;
  }

  if (s.selected == target)
    s.selected.clear();
  if (s.current == target || is_inside(target, s.current))
  {
    const fs::path parent = target.parent_path();
    navigate_to(parent.empty() ? s.project_root : parent);
  }
  s.status_message = "Deleted " + target.filename().string();
  s.delete_active  = false;

  push_asset_command("Delete asset",
                     [target, backup]()
                     {
                       std::error_code undo_ec;
                       copy_path(backup, target, undo_ec);
                       fs::remove_all(backup, undo_ec);
                       state().status_message = "Undid deletion of " + target.filename().string();
                     },
                     [target, backup]()
                     {
                       std::error_code redo_ec;
                       copy_path(target, backup, redo_ec);
                       remove_path(target, redo_ec);
                       state().status_message = "Redid deletion of " + target.filename().string();
                     });
}

void begin_rename(const fs::path& path)
{
  auto& s    = state();
  s.rename_target = path;
  std::strncpy(s.rename_buffer, path.filename().string().c_str(), sizeof(s.rename_buffer) - 1);
  s.rename_buffer[sizeof(s.rename_buffer) - 1] = '\0';
  s.rename_active = true;
}

void begin_create(CreateKind kind, const fs::path& directory)
{
  auto& s        = state();
  s.create_kind  = kind;
  s.create_directory = directory;
  std::strncpy(s.create_buffer,
               kind == CreateKind::Folder ? "New Folder"
               : kind == CreateKind::Scene ? "New Scene"
                                           : "New Script",
               sizeof(s.create_buffer) - 1);
  s.create_buffer[sizeof(s.create_buffer) - 1] = '\0';
  s.create_active = true;
}

void push_asset_selection_colors()
{
  ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.12f, 0.35f, 0.62f, 0.55f));
  ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.16f, 0.43f, 0.74f, 0.70f));
  ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.20f, 0.52f, 0.88f, 0.85f));
}

void pop_asset_selection_colors()
{
  ImGui::PopStyleColor(3);
}

bool toolbar_button(const char* label, const char* tooltip, bool enabled = true)
{
  ImGui::BeginDisabled(!enabled);
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 3));
  const bool pressed = ImGui::Button(label, ImVec2(26, 22));
  ImGui::PopStyleVar();
  if (ImGui::IsItemHovered() && tooltip)
    ImGui::SetTooltip("%s", tooltip);
  ImGui::EndDisabled();
  return pressed;
}

void draw_folder_drop_target(const fs::path& directory)
{
  if (!ImGui::BeginDragDropTarget())
    return;

  if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(k_content_payload))
  {
    for (const std::string& value : parse_payload(payload))
      move_item(normalized(value), directory);
  }
  if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(k_file_payload))
  {
    for (const std::string& value : parse_payload(payload))
      import_file(normalized(value), directory);
  }
  ImGui::EndDragDropTarget();
}

void draw_context_menu(const fs::path& path, bool is_directory)
{
  if (!ImGui::BeginPopupContextItem())
    return;

  auto& s = state();
  if (is_directory)
  {
    const auto kind = category_of(path);
    if (ImGui::MenuItem(ICON_FA_FOLDER_PLUS "  New Folder", nullptr, false, kind.has_value()))
      begin_create(CreateKind::Folder, path);
    if (kind == ProjectDirectory::Scenes &&
        ImGui::MenuItem(ICON_FA_CUBE "  New Scene"))
      begin_create(CreateKind::Scene, path);
    if (kind == ProjectDirectory::Scripts &&
        ImGui::MenuItem(ICON_FA_FILE_CODE "  New Script"))
      begin_create(CreateKind::Script, path);
    if (ImGui::MenuItem(ICON_FA_FILE_IMPORT "  Import Here...", nullptr, false, kind.has_value()))
    {
      const fs::path root = managed_root(*kind);
      std::string selected = FileDialogs::open_file(
          "All Supported Assets\0*.png;*.jpg;*.jpeg;*.bmp;*.tga;*.hdr;*.lua;*.aeroscene;*.wav;*.ogg;*.mp3;*.ttf;*.otf\0All Files (*.*)\0*.*\0",
          root.string());
      if (!selected.empty())
        import_file(normalized(selected), path);
    }
    ImGui::Separator();
  }

  if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN "  Open"))
    open_item(path);
  if (ImGui::MenuItem(ICON_FA_COPY "  Copy Path"))
  {
    std::string value = normalized_string(path);
    ImGui::SetClipboardText(value.c_str());
    s.status_message = "Path copied to clipboard";
  }
  if (ImGui::MenuItem(ICON_FA_UP_RIGHT_FROM_SQUARE "  Show in Explorer"))
    UI::show_in_explorer(path.string());

  if (path != s.project_root && !is_managed_root(path))
  {
    ImGui::Separator();
    if (ImGui::MenuItem(ICON_FA_PEN_TO_SQUARE "  Rename"))
      begin_rename(path);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.42f, 0.42f, 1.0f));
    const bool remove = ImGui::MenuItem(ICON_FA_TRASH "  Delete");
    ImGui::PopStyleColor();
    if (remove)
    {
      s.delete_target = path;
      s.delete_active = true;
    }
  }

  ImGui::EndPopup();
}

std::string ellipsize(const std::string& text, float width)
{
  if (ImGui::CalcTextSize(text.c_str()).x <= width)
    return text;
  std::string result = text;
  while (!result.empty() && ImGui::CalcTextSize((result + "...").c_str()).x > width)
    result.pop_back();
  return result + "...";
}

std::string format_size(uintmax_t bytes)
{
  static const char* units[] = {"B", "KB", "MB", "GB"};
  double             value   = static_cast<double>(bytes);
  size_t             unit    = 0;
  while (value >= 1024.0 && unit + 1 < 4)
  {
    value /= 1024.0;
    ++unit;
  }
  char buffer[48];
  std::snprintf(buffer, sizeof(buffer), "%.1f %s", value, units[unit]);
  return buffer;
}

AG_ref<Texture2D> get_thumbnail_texture(const fs::path& path)
{
  if (!UI::is_image(path))
    return {};
  if (!Project::is_path_in_active_directory(path.string(), ProjectDirectory::Assets))
    return {};

  const std::string key = normalized_string(path);
  auto&             cache = thumbnail_cache();
  auto              found = cache.find(key);
  if (found != cache.end())
    return found->second;

  std::string relative = Project::to_active_relative_path(key, ProjectDirectory::Assets);
  if (relative.empty())
    return {};

  try
  {
    return cache.emplace(key, NodeHelper::load_texture(relative)).first->second;
  }
  catch (const std::exception&)
  {
    return {};
  }
}

ImVec2 thumbnail_size(const AG_ref<Texture2D>& texture, float size)
{
  if (!texture || !texture->get_texture_id())
    return {};

  const float aspect = static_cast<float>(texture->get_size().x) /
                       static_cast<float>(std::max(texture->get_size().y, 1u));
  return aspect >= 1.0f ? ImVec2(size, size / aspect) : ImVec2(size * aspect, size);
}

ImVec2 draw_thumbnail_texture(const AG_ref<Texture2D>& texture, float size)
{
  const ImVec2 image_size = thumbnail_size(texture, size);
  if (image_size.x <= 0.0f || image_size.y <= 0.0f)
    return {};

  ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture->get_texture_id())),
               image_size,
               ImVec2(0, 1),
               ImVec2(1, 0));
  return image_size;
}

void draw_preview_panel()
{
  auto& s = state();
  ImGui::TextUnformatted("PREVIEW");
  ImGui::Separator();

  const ImVec2 avail = ImGui::GetContentRegionAvail();
  const float  preview_height = std::max(160.0f, avail.y - 118.0f);

  ImGui::BeginChild("##PreviewSurface",
                    ImVec2(0, preview_height),
                    true,
                    ImGuiWindowFlags_NoScrollbar);
  const ImVec2 inner = ImGui::GetContentRegionAvail();

  if (s.selected.empty())
  {
    const char* message = "Select an asset to preview it";
    const ImVec2 text_size = ImGui::CalcTextSize(message);
    ImGui::SetCursorPos(ImVec2(std::max(0.0f, (inner.x - text_size.x) * 0.5f),
                               std::max(0.0f, (inner.y - text_size.y) * 0.5f)));
    ImGui::TextDisabled("%s", message);
  }
  else
  {
    std::error_code ec;
    const bool      is_directory = fs::is_directory(s.selected, ec);
    AG_ref<Texture2D> texture = is_directory ? AG_ref<Texture2D>{} : get_thumbnail_texture(s.selected);

    if (texture)
    {
      const float max_size = std::max(32.0f, std::min(inner.x - 16.0f, inner.y - 16.0f));
      const ImVec2 image_size = thumbnail_size(texture, max_size);
      ImGui::SetCursorPos(ImVec2(std::max(0.0f, (inner.x - image_size.x) * 0.5f),
                                 std::max(0.0f, (inner.y - image_size.y) * 0.5f)));
      draw_thumbnail_texture(texture, max_size);
    }
    else
    {
      const char* icon = is_directory ? ICON_FA_FOLDER
                                      : UI::get_file_icon(s.selected.extension().string());
      const ImVec2 icon_size = ImGui::CalcTextSize(icon);
      ImGui::SetCursorPos(ImVec2(std::max(0.0f, (inner.x - icon_size.x) * 0.5f),
                                 std::max(0.0f, (inner.y - icon_size.y) * 0.5f - 20.0f)));
      ImGui::PushStyleColor(ImGuiCol_Text,
                            ImVec4(0.96f, 0.76f, 0.32f, 1.0f));
      ImGui::Text("%s", icon);
      ImGui::PopStyleColor();
    }
  }
  ImGui::EndChild();

  if (s.selected.empty())
    return;

  ImGui::Spacing();
  ImGui::TextUnformatted(s.selected.filename().string().c_str());

  std::error_code ec;
  const bool      is_directory = fs::is_directory(s.selected, ec);
  std::string     type = is_directory ? "Folder" : s.selected.extension().string();
  if (type.empty())
    type = "File";
  if (!type.empty() && type.front() == '.')
    type.erase(type.begin());

  ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
  ImGui::Text("Type: %s", type.c_str());
  if (!is_directory)
    ImGui::Text("Size: %s", format_size(fs::file_size(s.selected, ec)).c_str());
  ImGui::TextWrapped("Path: %s", normalized_string(s.selected).c_str());
  ImGui::PopStyleColor();
}

void draw_asset_item(const fs::path& path, bool is_directory, float cell_width)
{
  auto& s = state();
  const std::string name = path.filename().string();
  ImGui::PushID(normalized_string(path).c_str());

  const float cell_height = 88.0f;
  const ImVec2 start = ImGui::GetCursorScreenPos();
  const bool   selected = s.selected == path;

  push_asset_selection_colors();
  ImGui::Selectable("##asset_item",
                    selected,
                    ImGuiSelectableFlags_AllowDoubleClick,
                    ImVec2(cell_width, cell_height));
  pop_asset_selection_colors();

  // Icon-view interaction: first click selects, second click opens.
  const bool clicked       = ImGui::IsItemClicked();
  const bool double_clicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0);
  if (double_clicked)
  {
    open_item(path);
  }
  else if (clicked)
  {
    s.selected = path;
  }

  if (is_directory)
    draw_folder_drop_target(path);

  if (ImGui::BeginDragDropSource())
  {
    const std::string payload = normalized_string(path);
    ImGui::SetDragDropPayload(k_content_payload, payload.c_str(), payload.size() + 1);
    ImGui::BeginTooltip();
    ImGui::Text("%s  %s", is_directory ? ICON_FA_FOLDER : UI::get_file_icon(path.extension().string()), name.c_str());
    ImGui::TextDisabled("Drag to a folder or an editor field");
    ImGui::EndTooltip();
    ImGui::EndDragDropSource();
  }

  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  const ImVec2 end = ImVec2(start.x + cell_width, start.y + cell_height);
  if (selected)
  {
    draw_list->AddRectFilled(start,
                             end,
                             ImGui::GetColorU32(ImVec4(0.16f, 0.43f, 0.74f, 0.35f)));
  }

  const char* icon = is_directory ? ICON_FA_FOLDER : UI::get_file_icon(path.extension().string());
  ImFont*     font = ImGui::GetFont();
  const float font_size = ImGui::GetFontSize();
  const ImVec2 icon_size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, icon);
  draw_list->AddText(font,
                     font_size * 1.75f,
                     ImVec2(start.x + (cell_width - icon_size.x * 1.75f) * 0.5f, start.y + 10),
                     ImGui::GetColorU32(is_directory ? ImVec4(0.96f, 0.76f, 0.32f, 1.0f)
                                                     : ImVec4(0.65f, 0.78f, 0.92f, 1.0f)),
                     icon);

  const std::string label = ellipsize(name, cell_width - 8.0f);
  const ImVec2      label_size = ImGui::CalcTextSize(label.c_str());
  draw_list->AddText(ImVec2(start.x + (cell_width - label_size.x) * 0.5f, start.y + 56),
                     ImGui::GetColorU32(ImGuiCol_Text),
                     label.c_str());

  if (selected)
  {
    draw_list->AddRect(start,
                       end,
                       ImGui::GetColorU32(ImVec4(0.26f, 0.59f, 0.98f, 0.95f)),
                       0.0f,
                       0,
                       1.5f);
  }

  draw_context_menu(path, is_directory);
  ImGui::PopID();
}

bool entry_matches_filter(const fs::directory_entry& entry, const std::string& filter)
{
  const std::string name = lowercase(entry.path().filename().string());
  return filter.empty() || name.find(filter) != std::string::npos;
}

std::vector<fs::directory_entry> collect_entries()
{
  auto& s = state();
  std::vector<fs::directory_entry> entries;
  std::error_code                  ec;
  const std::string                filter = lowercase(s.search);

  if (!filter.empty())
  {
    // Unity's project search is global, even when the browser is sitting at the
    // project root. Search only inside the managed project folders.
    for (ProjectDirectory kind : k_managed_directories)
    {
      const fs::path root = managed_root(kind);
      if (root.empty())
        continue;
      for (fs::recursive_directory_iterator it(root, ec), end; it != end; it.increment(ec))
      {
        if (ec)
          continue;
        const bool is_directory = it->is_directory(ec);
        if (!is_directory && !UI::is_right_file(it->path()))
          continue;
        if (entry_matches_filter(*it, filter))
          entries.push_back(*it);
      }
    }
  }
  else
  {
    for (fs::directory_iterator it(s.current, ec), end; it != end; it.increment(ec))
    {
      if (ec)
        continue;

      if (s.current == s.project_root)
      {
        if (!it->is_directory(ec) || !is_managed_root(it->path()))
          continue;
      }
      else if (!it->is_directory(ec) && !UI::is_right_file(it->path()))
      {
        continue;
      }

      entries.push_back(*it);
    }
  }

  std::sort(entries.begin(),
            entries.end(),
            [](const auto& a, const auto& b)
            {
              const bool a_dir = a.is_directory();
              const bool b_dir = b.is_directory();
              if (a_dir != b_dir)
                return a_dir;
              return lowercase(a.path().filename().string()) <
                     lowercase(b.path().filename().string());
            });
  return entries;
}

void draw_grid()
{
  auto& s = state();
  const auto entries = collect_entries();
  if (entries.empty())
  {
    ImGui::Spacing();
    ImGui::TextDisabled(s.search[0] ? "No assets match the search." : "This folder is empty.");
    return;
  }

  const float available = ImGui::GetContentRegionAvail().x;
  const float spacing   = 8.0f;
  const int   columns   = std::max(1, static_cast<int>((available + spacing) / 104.0f));
  const float width     = std::max(64.0f, (available - spacing * (columns - 1)) / columns);

  for (size_t i = 0; i < entries.size(); ++i)
  {
    if (i % columns != 0)
      ImGui::SameLine(0, spacing);
    draw_asset_item(entries[i].path(), entries[i].is_directory(), width);
  }
}

void draw_list_view()
{
  auto& s = state();
  const auto entries = collect_entries();
  if (entries.empty())
  {
    ImGui::Spacing();
    ImGui::TextDisabled(s.search[0] ? "No assets match the search." : "This folder is empty.");
    return;
  }

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6.0f, 2.0f));
  for (const auto& entry : entries)
  {
    const fs::path  path = entry.path();
    const bool      is_directory = entry.is_directory();
    const std::string name = path.filename().string();
    const char*     icon = is_directory ? ICON_FA_FOLDER : UI::get_file_icon(path.extension().string());
    const std::string label = std::string(icon) + "  " + name;

    ImGui::PushID(normalized_string(path).c_str());
    const float label_width = std::min(300.0f, std::max(160.0f, ImGui::GetContentRegionAvail().x * 0.45f));
    push_asset_selection_colors();
    ImGui::Selectable(label.c_str(),
                      s.selected == path,
                      ImGuiSelectableFlags_AllowDoubleClick,
                      ImVec2(label_width, 0));
    pop_asset_selection_colors();

    // Single click selects; double click opens.
    if (ImGui::IsItemClicked())
      s.selected = path;
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
      open_item(path);

    if (is_directory)
      draw_folder_drop_target(path);

    if (ImGui::BeginDragDropSource())
    {
      const std::string payload = normalized_string(path);
      ImGui::SetDragDropPayload(k_content_payload, payload.c_str(), payload.size() + 1);
      ImGui::BeginTooltip();
      ImGui::Text("%s  %s", icon, name.c_str());
      ImGui::TextDisabled("Drag to a folder or an editor field");
      ImGui::EndTooltip();
      ImGui::EndDragDropSource();
    }

    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("%s", normalized_string(path).c_str());

    draw_context_menu(path, is_directory);

    if (s.search[0])
    {
      std::string location = normalized_string(path.parent_path());
      const std::string root = normalized_string(s.project_root) + "/";
      if (location.rfind(root, 0) == 0)
        location.erase(0, root.size());
      if (location.empty())
        location = "/";
      ImGui::SameLine(0, 8);
      ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
      ImGui::TextUnformatted(ellipsize(location, ImGui::GetContentRegionAvail().x).c_str());
      ImGui::PopStyleColor();
    }

    ImGui::PopID();
  }
  ImGui::PopStyleVar();
}

void draw_toolbar()
{
  auto& s = state();
  if (toolbar_button(ICON_FA_ARROW_LEFT, "Back", s.history_pos > 0))
    navigate_history(true);
  ImGui::SameLine(0, 3);
  if (toolbar_button(ICON_FA_ARROW_RIGHT, "Forward", s.history_pos + 1 < s.history.size()))
    navigate_history(false);
  ImGui::SameLine(0, 3);
  if (toolbar_button(ICON_FA_ARROW_UP, "Up", s.current != s.project_root))
    navigate_up();
  ImGui::SameLine(0, 3);
  if (toolbar_button(ICON_FA_ARROWS_ROTATE, "Refresh"))
    s.status_message = "Content browser refreshed";
  ImGui::SameLine(0, 3);
  if (toolbar_button(ICON_FA_LIST, s.list_view ? "Icon view" : "List view"))
    s.list_view = !s.list_view;

  ImGui::SameLine(0, 10);
  ImGui::SetNextItemWidth(std::max(90.0f, ImGui::GetContentRegionAvail().x - 210.0f));
  ImGui::InputTextWithHint("##AssetSearch",
                           ICON_FA_MAGNIFYING_GLASS "  Search assets",
                           s.search,
                           sizeof(s.search));

  ImGui::SameLine(0, 6);
  if (ImGui::Button(ICON_FA_PLUS "  New"))
    ImGui::OpenPopup("##NewAssetMenu");
  if (ImGui::BeginPopup("##NewAssetMenu"))
  {
    const auto kind = category_of(s.current);
    if (ImGui::MenuItem(ICON_FA_FOLDER_PLUS "  Folder", nullptr, false, kind.has_value()))
      begin_create(CreateKind::Folder, s.current);
    if (kind == ProjectDirectory::Scenes && ImGui::MenuItem(ICON_FA_CUBE "  Scene"))
      begin_create(CreateKind::Scene, s.current);
    if (kind == ProjectDirectory::Scripts && ImGui::MenuItem(ICON_FA_FILE_CODE "  Script"))
      begin_create(CreateKind::Script, s.current);
    ImGui::EndPopup();
  }
}

void draw_breadcrumbs()
{
  auto& s = state();
  fs::path path = s.project_root;
  size_t   index = 0;

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered]);
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 2));

  if (ImGui::Button(s.project_root.filename().string().c_str()))
    navigate_to(s.project_root);
  draw_folder_drop_target(s.project_root);

  const fs::path relative = s.current.lexically_relative(s.project_root);
  for (const auto& part : relative)
  {
    if (part == ".")
      continue;
    path /= part;
    ImGui::SameLine(0, 2);
    ImGui::TextDisabled("/");
    ImGui::SameLine(0, 2);
    ImGui::PushID(static_cast<int>(index++));
    if (ImGui::Button(part.string().c_str()))
      navigate_to(path);
    draw_folder_drop_target(path);
    ImGui::PopID();
  }

  ImGui::PopStyleVar();
  ImGui::PopStyleColor(2);
}

void draw_folder_tree()
{
  auto& s = state();
  ImGui::BeginChild("ContentFolders", ImVec2(218, 0), true);
  ImGui::TextUnformatted("FOLDERS");
  ImGui::Separator();

  for (ProjectDirectory kind : k_managed_directories)
  {
    const fs::path root = managed_root(kind);
    if (root.empty())
      continue;
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.96f, 0.76f, 0.32f, 1.0f));
    UI::draw_folder_node(root, 0);
    ImGui::PopStyleColor();
  }

  if (!s.status_message.empty())
  {
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextWrapped("%s", s.status_message.c_str());
  }

  ImGui::EndChild();
}

void draw_modals()
{
  auto& s = state();

  if (s.rename_active)
    ImGui::OpenPopup("Rename Asset##ContentBrowser");
  if (ImGui::BeginPopupModal("Rename Asset##ContentBrowser", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    ImGui::TextUnformatted("Enter a new name");
    ImGui::SetNextItemWidth(320);
    if (ImGui::InputText("##RenameAsset", s.rename_buffer, sizeof(s.rename_buffer),
                         ImGuiInputTextFlags_EnterReturnsTrue))
      do_rename();
    if (ImGui::Button("Rename", ImVec2(110, 0)))
      do_rename();
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(110, 0)))
      s.rename_active = false;
    ImGui::EndPopup();
  }

  if (s.delete_active)
    ImGui::OpenPopup("Delete Asset##ContentBrowser");
  if (ImGui::BeginPopupModal("Delete Asset##ContentBrowser", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    ImGui::TextWrapped("Delete \"%s\"? This cannot be undone.",
                       s.delete_target.filename().string().c_str());
    if (ImGui::Button("Delete", ImVec2(110, 0)))
      do_delete();
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(110, 0)))
      s.delete_active = false;
    ImGui::EndPopup();
  }

  if (s.create_active)
    ImGui::OpenPopup("Create Asset##ContentBrowser");
  if (ImGui::BeginPopupModal("Create Asset##ContentBrowser", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    const char* label = s.create_kind == CreateKind::Folder ? "Folder name"
                        : s.create_kind == CreateKind::Scene ? "Scene name"
                                                             : "Script name";
    ImGui::TextUnformatted(label);
    ImGui::SetNextItemWidth(320);
    if (ImGui::InputText("##CreateAsset", s.create_buffer, sizeof(s.create_buffer),
                         ImGuiInputTextFlags_EnterReturnsTrue))
      do_create();
    if (ImGui::Button("Create", ImVec2(110, 0)))
      do_create();
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(110, 0)))
      s.create_active = false;
    ImGui::EndPopup();
  }
}
}  // namespace

void UI::content_browser()
{
  ensure_state();
  auto& s = state();

  ImGui::SetNextWindowSize(ImVec2(760, 420), ImGuiCond_FirstUseEver);
  if (!ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_None))
  {
    ImGui::End();
    return;
  }

  if (s.project_root.empty())
  {
    ImGui::TextDisabled("Open a project to browse its assets.");
    ImGui::End();
    return;
  }

  draw_toolbar();
  draw_breadcrumbs();
  ImGui::Separator();

  ImGui::BeginChild("ContentBody", ImVec2(0, 0));
  draw_folder_tree();
  ImGui::SameLine();

  // Split the file view into the asset grid on the left and a large,
  // Unity-style inspector/preview column on the right.
  const float total_width = ImGui::GetContentRegionAvail().x;
  const float gap         = 6.0f;
  float       preview_width = std::min(420.0f, std::max(220.0f, total_width * 0.42f));
  if (total_width < 620.0f)
    preview_width = std::max(150.0f, total_width * 0.36f);
  float files_width = total_width - preview_width - gap;
  if (files_width < 220.0f)
  {
    preview_width = 0.0f;
    files_width   = total_width;
  }

  ImGui::BeginChild("ContentFiles", ImVec2(files_width, 0), true);

  // Full-area drop surface for files dragged in from the OS. Use a custom
  // target rectangle instead of an InvisibleButton so the grid/list items
  // underneath still receive clicks (single-click selection + bounding box).
  const ImVec2 surface_start = ImGui::GetCursorScreenPos();
  const ImVec2 surface_size  = ImGui::GetContentRegionAvail();
  const ImRect surface_rect(surface_start,
                            ImVec2(surface_start.x + surface_size.x,
                                   surface_start.y + surface_size.y));
  if (ImGui::BeginDragDropTargetCustom(surface_rect, ImGui::GetID("##AssetDropSurface")))
  {
    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(k_file_payload))
    {
      for (const std::string& value : parse_payload(payload))
        import_file(normalized(value), s.current);
    }
    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(k_content_payload))
    {
      for (const std::string& value : parse_payload(payload))
        move_item(normalized(value), s.current);
    }
    ImGui::EndDragDropTarget();
  }

  if (s.list_view)
    draw_list_view();
  else
    draw_grid();
  ImGui::EndChild();

  if (preview_width > 0.0f)
  {
    ImGui::SameLine(0, gap);
    ImGui::BeginChild("ContentPreviewPane", ImVec2(preview_width, 0), true);
    draw_preview_panel();
    ImGui::EndChild();
  }

  ImGui::EndChild();
  draw_modals();
  ImGui::End();
}

void UI::draw_folder_node(const std::filesystem::path& directory, int depth)
{
  auto&       s    = state();
  const fs::path path = normalized(directory);
  if (path.empty() || depth > 24)
    return;

  std::error_code ec;
  if (!fs::is_directory(path, ec))
    return;

  std::vector<fs::path> children;
  for (fs::directory_iterator it(path, ec), end; it != end; it.increment(ec))
  {
    if (it->is_directory(ec))
      children.push_back(it->path());
  }
  std::sort(children.begin(), children.end(), [](const fs::path& a, const fs::path& b)
            { return lowercase(a.filename().string()) < lowercase(b.filename().string()); });

  const bool     is_root   = is_managed_root(path);
  const std::string name   = path.filename().empty() ? path.string() : path.filename().string();
  ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;
  if (s.selected == path || (s.selected.empty() && s.current == path))
    flags |= ImGuiTreeNodeFlags_Selected;
  if (is_root)
    flags |= ImGuiTreeNodeFlags_DefaultOpen;

  ImGui::PushID(normalized_string(path).c_str());
  push_asset_selection_colors();
  const bool open = ImGui::TreeNodeEx((std::string(ICON_FA_FOLDER) + "  " + name).c_str(), flags);
  pop_asset_selection_colors();

  // In the folder pane, a single click selects. Double-clicking the folder
  // name navigates into it; the arrow still expands/collapses the tree.
  if (ImGui::IsItemClicked())
    s.selected = path;
  if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
  {
    navigate_to(path);
    s.selected = path;
  }
  draw_folder_drop_target(path);
  draw_context_menu(path, true);

  if (open)
  {
    for (const fs::path& child : children)
      UI::draw_folder_node(child, depth + 1);
    ImGui::TreePop();
  }
  ImGui::PopID();
}

bool UI::is_right_file(const std::filesystem::path& path)
{
  const std::string ext = lowercase(path.extension().string());

  if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga" ||
      ext == ".hdr" || ext == ".tiff" || ext == ".tif" || ext == ".webp" || ext == ".gif" ||
      ext == ".psd" || ext == ".svg")
    return true;
  if (ext == ".aeroscene" || ext == ".scene" || ext == ".prefab")
    return true;
  if (ext == ".lua")
    return true;
  if (ext == ".mp3" || ext == ".wav" || ext == ".ogg" || ext == ".flac" || ext == ".aiff" ||
      ext == ".wma")
    return true;
  if (ext == ".ttf" || ext == ".otf" || ext == ".woff" || ext == ".woff2")
    return true;
  if (ext == ".glsl" || ext == ".vert" || ext == ".frag" || ext == ".shader")
    return true;
  return false;
}

bool UI::is_image(const std::filesystem::path& path)
{
  const std::string ext = lowercase(path.extension().string());
  return ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga" ||
         ext == ".hdr" || ext == ".tiff" || ext == ".tif" || ext == ".webp" || ext == ".gif" ||
         ext == ".psd" || ext == ".svg";
}

const char* UI::get_file_icon(const std::string& extension)
{
  const std::string ext = lowercase(extension);

  if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".gif" ||
      ext == ".tga" || ext == ".webp" || ext == ".tif" || ext == ".tiff" || ext == ".psd" ||
      ext == ".svg")
    return ICON_FA_IMAGE;
  if (ext == ".cpp" || ext == ".h" || ext == ".hpp" || ext == ".c" || ext == ".cs" ||
      ext == ".java" || ext == ".json" || ext == ".xml" || ext == ".yaml" || ext == ".ini")
    return ICON_FA_FILE_CODE;
  if (ext == ".py" || ext == ".js" || ext == ".lua" || ext == ".sh")
    return ICON_FA_FILE_CODE;
  if (ext == ".txt" || ext == ".md" || ext == ".log")
    return ICON_FA_FILE_LINES;
  if (ext == ".aeroscene" || ext == ".scene" || ext == ".prefab" || ext == ".obj" ||
      ext == ".fbx" || ext == ".gltf" || ext == ".glb")
    return ICON_FA_CUBE;
  if (ext == ".mp3" || ext == ".wav" || ext == ".ogg" || ext == ".flac" || ext == ".aiff" ||
      ext == ".wma")
    return ICON_FA_FILE_AUDIO;
  if (ext == ".mp4" || ext == ".avi" || ext == ".mov")
    return ICON_FA_FILE_VIDEO;
  if (ext == ".glsl" || ext == ".vert" || ext == ".frag" || ext == ".shader")
    return ICON_FA_WAND_MAGIC_SPARKLES;
  if (ext == ".ttf" || ext == ".otf" || ext == ".woff" || ext == ".woff2")
    return ICON_FA_FONT;
  if (ext == ".zip" || ext == ".rar" || ext == ".7z")
    return ICON_FA_FILE_ZIPPER;
  if (ext == ".pdf")
    return ICON_FA_FILE_PDF;
  return ICON_FA_FILE;
}
}  // namespace ag
