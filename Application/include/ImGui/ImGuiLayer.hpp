#pragma once

#include <Layers/Layer.hpp>

struct ImGuiStyle;
struct ImFont;
struct ImFontConfig;
typedef unsigned short ImWchar;
namespace ag
{

class ImGuiLayer : public Layer
{
public:
  ImGuiLayer();
  ~ImGuiLayer();

  virtual void on_attach() override;
  virtual void on_detach() override;
  virtual void on_event(Event& event) override;

  void begin();
  void end();

  void set_engine_theme();

  void on_imgui_render() override;

  void block_events(bool block)
  {
    m_block_events = block;
  }

  ImFont*
  load_font(const std::string& path, float size, ImFontConfig* config, const ImWchar* ranges);

  static void set_cyberpunk_theme();
  static void set_warm_amber_theme();
  static void set_ocean_blue_theme();
  static void set_blood_red_theme();
  static void set_monochrome_slate_theme();
  static void set_light_azure_theme();
  static void set_sunset_orange_theme();
  static void set_forest_green_theme();
  static void set_purple_nebula_theme();
  static void set_crimson_red_theme();
  static void set_cyan_teal_theme();
  static void set_soft_pink_theme();
  static void set_golden_amber_theme();
  static void set_light_lavender_theme();
  static void set_midnight_blue_theme();
  static void apply_default_style_settings(ImGuiStyle& style);
  static void set_unity_dark_theme();
  static void set_unity_light_theme();

private:
  float m_time         = 0.0f;
  bool  m_block_events = true;
};
}  // namespace ag
