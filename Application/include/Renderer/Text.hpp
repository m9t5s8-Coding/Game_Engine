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
  float      advance;
  float      plane_top;
  float      plane_bottom;
  float      plane_left;
};
struct Font
{
  std::unordered_map<char, Glyph> glyphs;

  float ascender;
  float descender;
  float line_height;
  float em_size;
  float atlas_height;
};

inline Font font;

inline void loadGlyph(const std::string& jsonPath)
{
  json j = AssetManager::read_json(jsonPath, AssetManager::Domain::Engine);

  Helper::load_json(j["atlas"], "height", font.atlas_height, 0.0f);
  Helper::load_json(j["atlas"], "size", font.em_size, 0.0f);

  Helper::load_json(j["metrics"], "ascender", font.ascender, 0.0f);
  Helper::load_json(j["metrics"], "descender", font.descender, 0.0f);
  Helper::load_json(j["metrics"], "lineHeight", font.line_height, 0.0f);

  for (auto& glyphData : j["glyphs"])
  {
    int codepoint;
    Helper::load_json(glyphData, "unicode", codepoint, 0);

    if (codepoint < 0 || codepoint > 127)
      continue;

    char  c = static_cast<char>(codepoint);
    Glyph g;

    float left, right, top, bottom;

    Helper::load_json(glyphData["atlasBounds"], "left", left, 0.0f);
    Helper::load_json(glyphData["atlasBounds"], "bottom", bottom, 0.0f);
    Helper::load_json(glyphData["atlasBounds"], "right", right, 0.0f);
    Helper::load_json(glyphData["atlasBounds"], "top", top, 0.0f);
    Helper::load_json(glyphData, "advance", g.advance, 0.0f);

    g.texture_rect.size.x     = right - left;
    g.texture_rect.size.y     = top - bottom;
    g.texture_rect.position.x = left;
    g.texture_rect.position.y = font.atlas_height - top;

    g.advance = g.advance * font.em_size;

    float plane_top, plane_bottom, plane_left;
    Helper::load_json(glyphData["planeBounds"], "top", plane_top, 0.0f);
    Helper::load_json(glyphData["planeBounds"], "bottom", plane_bottom, 0.0f);
    Helper::load_json(glyphData["planeBounds"], "left", plane_left, 0.0f);

    g.plane_top    = plane_top * font.em_size;
    g.plane_bottom = plane_bottom * font.em_size;
    g.plane_left   = plane_left * font.em_size;

    font.glyphs[c] = g;
  }
}
}  // namespace ag::TextLoader
