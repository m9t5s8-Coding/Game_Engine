#pragma once

#include <Aero.hpp>

namespace ag
{
class Sandbox2D : public ag::Layer
{
public:
  Sandbox2D();
  virtual ~Sandbox2D() = default;

  virtual void on_attach() override;
  virtual void on_detach() override;

  virtual void on_update(TimeStamp ts) override;
  virtual void on_event(Event& event) override;

private:
  AG_ref<FrameBuffer>    m_framebuffer;
  AG_ref<ViewController> m_view_controller;
  AG_ref<Scene>          m_scene;

  void load_project_data();

  std::string get_appdata_path();

  bool      m_networking_enabled = false;
  TCPClient m_client;

  Entity m_hover_entity;

  void entity_selection();
  bool on_mouse_pressed(MouseButtonPressedEvent& e);
  bool on_window_resize(WindowResizeEvent& e);
  bool on_window_close(WindowCloseEvent& e);
};
}  // namespace ag
