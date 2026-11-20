#pragma once

#include <string>

namespace ag
{
struct FontImportResult
{
  bool        success = false;
  std::string json_path;       // Absolute path to the generated JSON atlas.
  std::string png_path;        // Absolute path to the generated PNG atlas.
  std::string project_path;    // Project-relative path stored on Font_Component.
  std::string message;
};

class FontImporter
{
public:
  // Generates an MSDF-compatible atlas (three SDF channels, so median(rgb)
  // in the text shaders reconstructs the same edge) and its JSON metrics.
  // Outputs are written to <project>/<Assets>/fonts/<font name>.{png,json}.
  static FontImportResult import_font(const std::string& source_path);
};
}  // namespace ag
