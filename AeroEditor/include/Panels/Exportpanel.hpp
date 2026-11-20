#pragma once
#include <Packing/AndroidBuildManager.hpp>
#include <Packing/GameExporter.hpp>

namespace ag {
class ExportPanel {
public:
  ExportPanel();

  void open();
  void open_android();
  void close();
  void render();

  bool is_open() const { return m_open; }

private:
  void draw_content();
  void draw_desktop_content();
  void draw_android_content();

private:
  GameExporter        m_exporter;
  AndroidBuildManager m_android_builder;
  bool                m_open = false;
  int                 m_active_tab = 0;
  bool                m_select_android_tab = false;

  char m_pak_name[256]      = "game";
  char m_exe_name[256]      = "Sandbox";
  char m_output_folder[512] = "";

  char m_android_dir[512] = "";
  bool m_android_offline  = false;
  bool m_android_clean    = false;
  int  m_android_type     = 0;  // 0 = Debug, 1 = Release
};
}  // namespace ag
