#include <Font/FontManager.hpp>

#include <algorithm>
#include <filesystem>
#include <Helper.hpp>
#include <Project/Project.hpp>
#include <set>

namespace fs = std::filesystem;

namespace ag
{
namespace
{
bool is_engine_font_path(const std::string& path)
{
  return path.empty() || path == FontManager::default_font_path() ||
         path.rfind("textures/", 0) == 0;
}
}  // namespace

std::mutex& FontManager::mutex()
{
  static std::mutex s_mutex;
  return s_mutex;
}

std::unordered_map<std::string, std::shared_ptr<FontManager::LoadedFont>>& FontManager::cache()
{
  static std::unordered_map<std::string, std::shared_ptr<LoadedFont>> s_cache;
  return s_cache;
}

void FontManager::init()
{
  std::lock_guard<std::mutex> lk(mutex());
  ensure_default_locked();
}

void FontManager::shut_down()
{
  std::lock_guard<std::mutex> lk(mutex());
  cache().clear();
}

void FontManager::reload()
{
  shut_down();
  init();
}

std::shared_ptr<FontManager::LoadedFont> FontManager::ensure_default_locked()
{
  auto& fonts = cache();
  if (auto it = fonts.find(""); it != fonts.end())
    return it->second;

  auto loaded             = std::make_shared<LoadedFont>();
  loaded->path            = "";
  loaded->metrics         = std::make_shared<TextLoader::Font>(
      TextLoader::load_font(default_font_path(), AssetManager::Domain::Engine));
  loaded->texture         = Texture2D::create("textures/atlas.png",
                                              true,
                                              Filter_Mode::AG_LINEAR_NOMIPMAP,
                                              true);

  // Keep the legacy global in sync for any code that still reads it directly.
  TextLoader::font = *loaded->metrics;
  fonts.emplace("", loaded);
  return loaded;
}

std::shared_ptr<FontManager::LoadedFont> FontManager::get_default()
{
  std::lock_guard<std::mutex> lk(mutex());
  return ensure_default_locked();
}

std::shared_ptr<FontManager::LoadedFont> FontManager::get(const std::string& path)
{
  std::string key = path;
  Helper::normalize_path(key);

  std::lock_guard<std::mutex> lk(mutex());
  auto                        def = ensure_default_locked();

  auto it = cache().find(key);
  if (it != cache().end())
    return it->second;

  if (key.empty())
    return def;

  auto loaded = load_locked(key);
  if (!loaded)
  {
    AERO_CORE_WARN("[FontManager] Falling back to the default font for '{0}'", key);
    return def;
  }

  cache().emplace(key, loaded);
  return loaded;
}

std::shared_ptr<FontManager::LoadedFont> FontManager::load_locked(const std::string& path)
{
  if (is_engine_font_path(path))
  {
    auto loaded     = std::make_shared<LoadedFont>();
    loaded->path    = path;
    loaded->metrics = std::make_shared<TextLoader::Font>(
        TextLoader::load_font(default_font_path(), AssetManager::Domain::Engine));
    loaded->texture = Texture2D::create("textures/atlas.png",
                                        true,
                                        Filter_Mode::AG_LINEAR_NOMIPMAP,
                                        true);
    return loaded;
  }

  const std::string resolved = resolve_json_path(path);
  std::error_code   ec;
  const bool        on_disk = !resolved.empty() && fs::exists(resolved, ec) && !ec;

  auto loaded = std::make_shared<LoadedFont>();
  loaded->path = path;

  if (on_disk)
  {
    TextLoader::Font metrics;
    if (!TextLoader::load_font_from_file(resolved, metrics))
      return nullptr;

    loaded->metrics = std::make_shared<TextLoader::Font>(std::move(metrics));
    loaded->texture = Texture2D::create(derive_png_path(resolved),
                                        true,
                                        Filter_Mode::AG_LINEAR_NOMIPMAP,
                                        false);
    return loaded;
  }

  // Packed project fallback: AssetManager resolves paths relative to the
  // mounted pak, which never wants a leading slash.
  const std::string relative = trim_leading_slashes(path);
  auto                 j     = AssetManager::read_json(relative, AssetManager::Domain::Project);
  if (!j.is_object())
    return nullptr;

  loaded->metrics = std::make_shared<TextLoader::Font>(TextLoader::parse_font(j));
  loaded->texture = Texture2D::create(derive_png_path(relative),
                                      true,
                                      Filter_Mode::AG_LINEAR_NOMIPMAP,
                                      false);
  return loaded;
}

std::string FontManager::resolve_json_path(const std::string& path)
{
  if (path.empty() || is_engine_font_path(path))
    return default_font_path();

  std::error_code ec;
  fs::path        direct(path);
  if (fs::exists(direct, ec) && !ec)
    return fs::absolute(direct, ec).string();

  auto project = Project::get_active_project();
  if (!project)
    return path;

  const std::string relative = trim_leading_slashes(path);
  const std::string assets   = trim_leading_slashes(project->get_assets_directory());
  fs::path          base(project->get_directory());

  fs::path candidate;
  if (!assets.empty() && relative.rfind(assets + "/", 0) == 0)
    candidate = base / relative;
  else
    candidate = base / (assets.empty() ? "Assets" : assets) / relative;

  if (fs::exists(candidate, ec) && !ec)
    return fs::absolute(candidate, ec).string();

  return relative;
}

std::string FontManager::derive_png_path(const std::string& json_path)
{
  fs::path path(json_path);
  path.replace_extension(".png");
  return path.string();
}

std::string FontManager::trim_leading_slashes(std::string path)
{
  Helper::normalize_path(path);
  while (!path.empty() && path.front() == '/')
    path.erase(path.begin());
  return path;
}

std::string FontManager::file_name(const std::string& path)
{
  const size_t slash = path.find_last_of("/\\");
  return slash == std::string::npos ? path : path.substr(slash + 1);
}

std::vector<std::string> FontManager::list_fonts()
{
  std::set<std::string> found;

  auto project = Project::get_active_project();
  if (project)
  {
    const std::string assets = trim_leading_slashes(project->get_assets_directory());
    fs::path          dir = fs::path(project->get_directory()) /
                      (assets.empty() ? "Assets" : assets) / "fonts";

    std::error_code ec;
    if (fs::is_directory(dir, ec) && !ec)
    {
      for (const auto& entry : fs::directory_iterator(dir, ec))
      {
        if (entry.is_regular_file(ec) && entry.path().extension() == ".json")
          found.insert("/fonts/" + entry.path().filename().string());
      }
    }
  }

  if (AssetManager::is_packed(AssetManager::Domain::Project))
  {
    for (const auto& file : AssetManager::list_files(AssetManager::Domain::Project))
    {
      const std::string normalized = trim_leading_slashes(file);
      if (normalized.find("fonts/") == std::string::npos)
        continue;
      if (normalized.size() < 5 || normalized.substr(normalized.size() - 5) != ".json")
        continue;
      found.insert("/fonts/" + file_name(normalized));
    }
  }

  return std::vector<std::string>(found.begin(), found.end());
}

std::string FontManager::display_name(const std::string& path)
{
  if (path.empty())
    return "Default";

  std::string name = file_name(path);
  const size_t dot = name.find_last_of('.');
  if (dot != std::string::npos)
    name.erase(dot);
  return name;
}
}  // namespace ag
