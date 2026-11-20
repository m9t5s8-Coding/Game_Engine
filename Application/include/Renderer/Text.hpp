#pragma once

#include <Core/Core.hpp>
#include <fstream>
#include <glm/glm.hpp>
#include <Helper.hpp>
#include <Math/Math.hpp>
#include <Project/Assetmanager.hpp>
#include <string>
#include <unordered_map>

namespace ag::TextLoader
{
struct Glyph
{
  float_rect texture_rect;
  float      advance      = 0.0f;
  float      plane_top    = 0.0f;
  float      plane_bottom = 0.0f;
  float      plane_left   = 0.0f;
};
struct Font
{
  std::unordered_map<char, Glyph> glyphs;

  float ascender     = 0.0f;
  float descender    = 0.0f;
  float line_height  = 0.0f;
  float em_size      = 0.0f;
  float atlas_height = 0.0f;
};

// Kept for the engine's built-in font and for callers that do not need a
// particular atlas. Renderer2D now carries the resolved font on each command.
inline Font font;

inline Font parse_font(const json& j)
{
  Font result;

  if (!j.is_object())
    return result;

  if (j.contains("atlas"))
  {
    Helper::load_json(j["atlas"], "height", result.atlas_height, 0.0f);
    Helper::load_json(j["atlas"], "size", result.em_size, 0.0f);
  }

  if (j.contains("metrics"))
  {
    Helper::load_json(j["metrics"], "ascender", result.ascender, 0.0f);
    Helper::load_json(j["metrics"], "descender", result.descender, 0.0f);
    Helper::load_json(j["metrics"], "lineHeight", result.line_height, 0.0f);
  }

  if (!j.contains("glyphs") || !j["glyphs"].is_array())
    return result;

  for (const auto& glyphData : j["glyphs"])
  {
    int codepoint = 0;
    Helper::load_json(glyphData, "unicode", codepoint, 0);

    if (codepoint < 0 || codepoint > 127)
      continue;

    Glyph g;
    Helper::load_json(glyphData, "advance", g.advance, 0.0f);
    g.advance *= result.em_size;

    const bool has_bounds = glyphData.contains("atlasBounds") && glyphData.contains("planeBounds");

    if (has_bounds)
    {
      float left = 0.0f, right = 0.0f, top = 0.0f, bottom = 0.0f;
      Helper::load_json(glyphData["atlasBounds"], "left", left, 0.0f);
      Helper::load_json(glyphData["atlasBounds"], "bottom", bottom, 0.0f);
      Helper::load_json(glyphData["atlasBounds"], "right", right, 0.0f);
      Helper::load_json(glyphData["atlasBounds"], "top", top, 0.0f);

      g.texture_rect.size.x     = right - left;
      g.texture_rect.size.y     = top - bottom;
      g.texture_rect.position.x = left;
      g.texture_rect.position.y = result.atlas_height - top;

      float plane_top = 0.0f, plane_bottom = 0.0f, plane_left = 0.0f;
      Helper::load_json(glyphData["planeBounds"], "top", plane_top, 0.0f);
      Helper::load_json(glyphData["planeBounds"], "bottom", plane_bottom, 0.0f);
      Helper::load_json(glyphData["planeBounds"], "left", plane_left, 0.0f);

      g.plane_top    = plane_top * result.em_size;
      g.plane_bottom = plane_bottom * result.em_size;
      g.plane_left   = plane_left * result.em_size;
    }

    result.glyphs[static_cast<char>(codepoint)] = g;
  }

  return result;
}

inline Font load_font(const std::string& jsonPath, AssetManager::Domain domain)
{
  return parse_font(AssetManager::read_json(jsonPath, domain));
}

inline bool load_font_from_file(const std::string& jsonPath, Font& result)
{
  std::ifstream file(jsonPath);
  if (!file.is_open())
    return false;

  json j;
  try
  {
    file >> j;
  }
  catch (const std::exception&)
  {
    return false;
  }

  result = parse_font(j);
  return result.em_size > 0.0f;
}

inline void loadGlyph(const std::string& jsonPath)
{
  font = load_font(jsonPath, AssetManager::Domain::Engine);
}
}  // namespace ag::TextLoader
