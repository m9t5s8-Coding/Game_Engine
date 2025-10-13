#include <Apch.hpp>
#include <Renderer/OrthoCameraController.hpp>
#include <Core/Input.hpp>
#include <Core/Core.hpp>

namespace aero
{
  OrthoCameraController::OrthoCameraController(float aspect_radio)
  : m_aspect_ratio(aspect_radio),
   m_camera(-m_aspect_ratio * m_zoom_level, m_aspect_ratio * m_zoom_level, -m_zoom_level, m_zoom_level)
  {

  }



  void OrthoCameraController::on_update(TimeStamp ts)
  {
    float deltatime = ts.get_seconds();

    if (Keyboard::is_key_pressed(Key::W))
      m_camera_pos.y += m_camera_speed * deltatime * m_zoom_level;
    if (Keyboard::is_key_pressed(Key::S))
      m_camera_pos.y -= m_camera_speed * deltatime * m_zoom_level;

    if (Keyboard::is_key_pressed(Key::A))
      m_camera_pos.x -= m_camera_speed * deltatime * m_zoom_level;
    if (Keyboard::is_key_pressed(Key::D))
      m_camera_pos.x += m_camera_speed * deltatime * m_zoom_level;

    m_camera.set_position(m_camera_pos);
  }

  void OrthoCameraController::on_event(Event &e)
  {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(AERO_BIND_EVENT_FN(OrthoCameraController::on_mouse_scroll));
    dispatcher.Dispatch<WindowResizeEvent>(AERO_BIND_EVENT_FN(OrthoCameraController::on_window_resize));
  }

  bool OrthoCameraController::on_mouse_scroll(MouseScrolledEvent &e)
  {
    m_zoom_level -= e.get_offsetY() * 0.3f;
    m_zoom_level = std::max(m_zoom_level, 0.15f);
    m_camera.set_projection(-m_aspect_ratio * m_zoom_level, m_aspect_ratio * m_zoom_level, -m_zoom_level, m_zoom_level);
    return false;
  }

  bool OrthoCameraController::on_window_resize(WindowResizeEvent &e)
  {
    m_aspect_ratio = (float)e.get_width() / (float) e.get_height();
    m_camera.set_projection(-m_aspect_ratio * m_zoom_level, m_aspect_ratio * m_zoom_level, -m_zoom_level, m_zoom_level);
    return false;
  }
}
