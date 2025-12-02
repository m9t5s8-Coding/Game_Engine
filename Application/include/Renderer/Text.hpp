#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <fstream>
#include <Core/Core.hpp>
#include <Math/Math.hpp>

namespace ag::TextLoader
{
  struct Glyph 
  {
    float_rect texture_rect;
    float advance;
    float plane_top;
    float plane_bottom;
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

  Font font;

  void loadGlyph(const std::string& jsonPath) 
  {
    std::ifstream file(jsonPath);
    json j;
    file >> j;

    
    
    Helper::load_json(j["atlas"], "height", font.atlas_height);
    Helper::load_json(j["atlas"], "size", font.em_size);

    Helper::load_json(j["metrics"], "ascender", font.ascender);
    Helper::load_json(j["metrics"], "descender", font.descender);
    Helper::load_json(j["metrics"], "lineHeight", font.line_height);


    for (auto& glyphData : j["glyphs"])
    {
      int codepoint;
      Helper::load_json(glyphData, "unicode", codepoint);

      if (codepoint < 0 || codepoint > 127)
        continue;

      char c = static_cast<char>(codepoint);
      Glyph g;

      float left, right, top, bottom;

      Helper::load_json(glyphData["atlasBounds"], "left", left);
      Helper::load_json(glyphData["atlasBounds"], "bottom", bottom);
      Helper::load_json(glyphData["atlasBounds"], "right", right);
      Helper::load_json(glyphData["atlasBounds"], "top", top);
      Helper::load_json(glyphData, "advance", g.advance);

      g.texture_rect.size.x = right - left;
      g.texture_rect.size.y = top - bottom;
      g.texture_rect.position.x = left;
      g.texture_rect.position.y = font.atlas_height - top;


      g.advance = g.advance * font.em_size;

      float planeTop, planeBottom;
      Helper::load_json(glyphData["planeBounds"], "top", planeTop);
      Helper::load_json(glyphData["planeBounds"], "bottom", planeBottom);

      g.plane_top = planeTop * font.em_size;
      g.plane_bottom = planeBottom * font.em_size;


      font.glyphs[c] = g;


      
    }

  }
}