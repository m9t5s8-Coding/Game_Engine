#pragma once

#include <memory>
#include <mutex>
#include <Renderer/Text.hpp>
#include <Renderer/Texture.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace ag
{
class FontManager
{
public:
  struct LoadedFont
  {
    std::string                       path;
    AG_ref<Texture2D>                 texture;
    std::shared_ptr<TextLoader::Font> metrics;
  };

  static constexpr const char* default_font_path() { return "textures/atlas.json"; }

  static void init();
  static void shut_down();
  static void reload();

  static std::shared_ptr<LoadedFont> get(const std::string& path);
  static std::shared_ptr<LoadedFont> get_default();

  // Returns project-relative paths such as "/fonts/MyFont.json". The empty
  // path (the built-in font) is not included; the UI adds that entry itself.
  static std::vector<std::string> list_fonts();
  static std::string              display_name(const std::string& path);

private:
  static std::mutex&                                            mutex();
  static std::unordered_map<std::string, std::shared_ptr<LoadedFont>>& cache();

  static std::shared_ptr<LoadedFont> ensure_default_locked();
  static std::shared_ptr<LoadedFont> load_locked(const std::string& path);

  static std::string resolve_json_path(const std::string& path);
  static std::string derive_png_path(const std::string& json_path);
  static std::string trim_leading_slashes(std::string path);
  static std::string file_name(const std::string& path);
};
}  // namespace ag
