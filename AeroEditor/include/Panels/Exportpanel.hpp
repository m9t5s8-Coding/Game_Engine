#pragma once
#include <Packing/GameExporter.hpp>

namespace ag {
class ExportPanel {
public:
  ExportPanel();

  void open();
  void close();
  void render();

  bool is_open() const { return m_open; }

private:
  void draw_content();

private:
  GameExporter m_exporter;
  bool         m_open = false;

  char m_pak_name[256]      = "game";
  char m_exe_name[256]      = "Sandbox";
  char m_output_folder[512] = "";

  std::string m_temp_output_path;
};
}  // namespace ag
