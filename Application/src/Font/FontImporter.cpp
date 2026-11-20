#define STB_TRUETYPE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_truetype.h>
#include <stb_image_write.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <Font/FontImporter.hpp>
#include <Helper.hpp>
#include <Project/Project.hpp>
#include <vector>

namespace fs = std::filesystem;

namespace ag
{
namespace
{
constexpr int   k_font_size = 48;
constexpr int   k_padding   = 8;
// SDF glyph bitmaps already include their distance padding. Keep a full
// padding-sized gutter between packed glyphs so neighbours cannot overwrite
// each other's edge falloff.
constexpr int   k_margin    = k_padding + 1;
constexpr int   k_first_cp  = 32;
constexpr int   k_last_cp   = 126;
constexpr float k_on_edge   = 128.0f;

struct ImportedGlyph
{
  int                 codepoint = 0;
  float               advance   = 0.0f;
  float               bearing_x = 0.0f;
  bool                has_bounds = false;
  int                 width  = 0;
  int                 height = 0;
  int                 xoff   = 0;
  int                 yoff   = 0;
  int                 atlas_x = 0;
  int                 atlas_y = 0;
  std::vector<uint8_t> sdf;
};

std::string trim_leading_slashes(std::string path)
{
  Helper::normalize_path(path);
  while (!path.empty() && path.front() == '/')
    path.erase(path.begin());
  return path;
}

std::string sanitize_name(std::string name)
{
  std::string result;
  result.reserve(name.size());
  for (unsigned char c : name)
  {
    if (std::isalnum(c) || c == '_' || c == '-')
      result.push_back(static_cast<char>(c));
    else if (!result.empty() && result.back() != '_')
      result.push_back('_');
  }
  while (!result.empty() && result.back() == '_')
    result.pop_back();
  return result.empty() ? "ImportedFont" : result;
}

bool pack_glyphs(std::vector<ImportedGlyph>& glyphs, int atlas_size)
{
  int x          = k_margin;
  int y          = k_margin;
  int row_height = 0;

  for (auto& glyph : glyphs)
  {
    if (!glyph.has_bounds)
      continue;

    if (x + glyph.width + k_margin > atlas_size)
    {
      x          = k_margin;
      y += row_height + k_margin;
      row_height = 0;
    }

    if (y + glyph.height + k_margin > atlas_size)
      return false;

    glyph.atlas_x = x;
    glyph.atlas_y = y;
    x += glyph.width + k_margin;
    row_height = std::max(row_height, glyph.height);
  }

  return true;
}
}  // namespace

FontImportResult FontImporter::import_font(const std::string& source_path)
{
  FontImportResult result;

  std::error_code ec;
  if (!fs::exists(source_path, ec) || ec)
  {
    result.message = "Font file does not exist.";
    return result;
  }

  auto project = Project::get_active_project();
  if (!project)
  {
    result.message = "Open a project before importing a font.";
    return result;
  }

  std::ifstream font_file(source_path, std::ios::binary | std::ios::ate);
  if (!font_file.is_open())
  {
    result.message = "Failed to open the font file.";
    return result;
  }

  const std::streampos font_size = font_file.tellg();
  font_file.seekg(0);
  std::vector<uint8_t> font_data(static_cast<size_t>(font_size));
  if (!font_data.empty())
    font_file.read(reinterpret_cast<char*>(font_data.data()), font_size);
  font_file.close();

  const int font_offset = stbtt_GetFontOffsetForIndex(font_data.data(), 0);
  if (font_offset < 0)
  {
    result.message = "The selected file is not a supported TrueType/OpenType font.";
    return result;
  }

  stbtt_fontinfo info;
  if (stbtt_InitFont(&info, font_data.data(), font_offset) == 0)
  {
    result.message = "Failed to parse the selected font.";
    return result;
  }

  const float scale = stbtt_ScaleForPixelHeight(&info, static_cast<float>(k_font_size));
  const float pixel_dist_scale = k_on_edge / static_cast<float>(k_padding);

  int ascent = 0, descent = 0, line_gap = 0;
  stbtt_GetFontVMetrics(&info, &ascent, &descent, &line_gap);

  std::vector<ImportedGlyph> glyphs;
  glyphs.reserve(k_last_cp - k_first_cp + 1);

  for (int cp = k_first_cp; cp <= k_last_cp; ++cp)
  {
    ImportedGlyph glyph;
    glyph.codepoint = cp;

    int  advance = 0, bearing = 0;
    stbtt_GetCodepointHMetrics(&info, cp, &advance, &bearing);
    glyph.advance   = static_cast<float>(advance) * scale / static_cast<float>(k_font_size);
    glyph.bearing_x = static_cast<float>(bearing) * scale / static_cast<float>(k_font_size);

    int w = 0, h = 0, xoff = 0, yoff = 0;
    uint8_t* sdf = stbtt_GetCodepointSDF(&info,
                                         scale,
                                         cp,
                                         k_padding,
                                         static_cast<unsigned char>(k_on_edge),
                                         pixel_dist_scale,
                                         &w,
                                         &h,
                                         &xoff,
                                         &yoff);
    if (sdf)
    {
      glyph.has_bounds = true;
      glyph.width      = w;
      glyph.height     = h;
      glyph.xoff       = xoff;
      glyph.yoff       = yoff;
      glyph.sdf.assign(sdf, sdf + static_cast<size_t>(w) * static_cast<size_t>(h));
      stbtt_FreeSDF(sdf, nullptr);
    }

    glyphs.push_back(std::move(glyph));
  }

  int atlas_size = 0;
  for (int candidate : {512, 1024, 2048})
  {
    if (pack_glyphs(glyphs, candidate))
    {
      atlas_size = candidate;
      break;
    }
  }

  if (atlas_size == 0)
  {
    result.message = "The font is too large to fit in a font atlas.";
    return result;
  }

  std::vector<uint8_t> atlas(static_cast<size_t>(atlas_size) * atlas_size * 3, 0);
  for (const auto& glyph : glyphs)
  {
    if (!glyph.has_bounds)
      continue;

    for (int y = 0; y < glyph.height; ++y)
    {
      for (int x = 0; x < glyph.width; ++x)
      {
        const size_t src = static_cast<size_t>(y) * glyph.width + x;
        const size_t dst = (static_cast<size_t>(glyph.atlas_y + y) * atlas_size +
                            glyph.atlas_x + x) *
                           3;
        const uint8_t value = glyph.sdf[src];
        atlas[dst] = value;
        atlas[dst + 1] = value;
        atlas[dst + 2] = value;
      }
    }
  }

  const std::string assets    = trim_leading_slashes(project->get_assets_directory());
  const fs::path    out_dir   = fs::path(project->get_directory()) /
                              (assets.empty() ? "Assets" : assets) / "fonts";
  std::error_code   dir_error;
  fs::create_directories(out_dir, dir_error);
  if (dir_error)
  {
    result.message = "Failed to create the project fonts folder: " + dir_error.message();
    return result;
  }

  const std::string stem = sanitize_name(fs::path(source_path).stem().string());
  const fs::path    png_path  = out_dir / (stem + ".png");
  const fs::path    json_path = out_dir / (stem + ".json");
  const fs::path    font_path = out_dir / (stem + fs::path(source_path).extension().string());

  if (stbi_write_png(png_path.string().c_str(),
                     atlas_size,
                     atlas_size,
                     3,
                     atlas.data(),
                     atlas_size * 3) == 0)
  {
    result.message = "Failed to write the generated font atlas PNG.";
    return result;
  }

  json atlas_json;
  atlas_json["atlas"] = {{"type", "msdf"},
                         {"distanceRange", k_padding * 2},
                         {"distanceRangeMiddle", k_on_edge / 255.0f},
                         {"size", k_font_size},
                         {"width", atlas_size},
                         {"height", atlas_size},
                         {"yOrigin", "bottom"}};

  const float em_scale = scale / static_cast<float>(k_font_size);
  atlas_json["metrics"] = {{"emSize", 1.0f},
                           {"lineHeight", static_cast<float>(ascent - descent + line_gap) * em_scale},
                           {"ascender", static_cast<float>(ascent) * em_scale},
                           {"descender", static_cast<float>(descent) * em_scale},
                           {"underlineY", -0.125f},
                           {"underlineThickness", 0.05f}};

  atlas_json["glyphs"] = json::array();
  for (const auto& glyph : glyphs)
  {
    json entry;
    entry["unicode"] = glyph.codepoint;
    entry["advance"] = glyph.advance;
    entry["bearing"] = {{"x", glyph.bearing_x}, {"y", 0.0f}};

    if (glyph.has_bounds)
    {
      const float left   = glyph.atlas_x + 0.5f;
      const float right  = glyph.atlas_x + glyph.width - 0.5f;
      const float top    = atlas_size - (glyph.atlas_y + 0.5f);
      const float bottom = atlas_size - (glyph.atlas_y + glyph.height - 0.5f);

      entry["atlasBounds"] = {{"left", left},
                              {"bottom", bottom},
                              {"right", right},
                              {"top", top}};

      entry["planeBounds"] = {
          {"left", (glyph.xoff + 0.5f) / k_font_size},
          {"bottom", -((glyph.yoff + glyph.height) - 0.5f) / k_font_size},
          {"right", (glyph.xoff + glyph.width - 0.5f) / k_font_size},
          {"top", -(glyph.yoff + 0.5f) / k_font_size}};
    }

    atlas_json["glyphs"].push_back(std::move(entry));
  }

  std::ofstream json_file(json_path, std::ios::binary);
  if (!json_file.is_open())
  {
    result.message = "Failed to write the generated font atlas JSON.";
    return result;
  }
  json_file << atlas_json.dump(2);
  json_file.close();

  fs::copy_file(source_path, font_path, fs::copy_options::overwrite_existing, dir_error);

  result.success      = true;
  result.json_path    = json_path.string();
  result.png_path     = png_path.string();
  result.project_path = "/fonts/" + stem + ".json";
  result.message      = "Imported " + stem + ".";
  return result;
}
}  // namespace ag
