#pragma once

#include <Renderer/OrthographicCamera.hpp>
#include <Core/TimeStamp.hpp>

#include <Events/WindowEvent.hpp>
#include <Events/MouseEvent.hpp>


namespace aero
{
  class OrthoCameraController
  {
  public:
    OrthoCameraController(float aspect_radio);

    void on_update(TimeStamp ts);
    void on_event(Event& e);

    OrthographicCamera& get_camera() { return m_camera; }
    const OrthographicCamera& get_camera() const { return m_camera; }

  private:
    bool on_mouse_scroll(MouseScrolledEvent& e);
    bool on_window_resize(WindowResizeEvent& e);
  private:
    float m_aspect_ratio;
    float m_zoom_level = 1.0f;
    OrthographicCamera m_camera;

    glm::vec3 m_camera_pos = { 0.0f, 0.0f, 0.0f};
    float m_camera_speed = 1.2f;
  };
}
