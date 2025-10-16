#include <Apch.hpp>
#include <Renderer/ViewController.hpp>
#include <Renderer/View.hpp>
#include <Core/Input.hpp>
#include <Core/Core.hpp>
namespace ag
{
  ViewController::ViewController()
  {

  }
  ViewController::ViewController(const vec2u &view_size, const vec2f &view_center)
      : m_view(view_size, view_center)
  {
  }

  ViewController::~ViewController()
  {
  }

  void ViewController::set_view(const vec2u& view_size, const vec2f& view_center)
  {
    m_view.set_size(view_size);
    m_view.set_center(view_center);
  }

  void ViewController::on_update(TimeStamp ts)
  {
    float deltatime = ts.get_seconds();

    vec2f view_center = m_view.get_center();

    if (Keyboard::is_key_pressed(Key::W))
      view_center.y -= m_view_speed * deltatime;
    if (Keyboard::is_key_pressed(Key::S))
      view_center.y += m_view_speed * deltatime;

    if (Keyboard::is_key_pressed(Key::A))
      view_center.x -= m_view_speed * deltatime;
    if (Keyboard::is_key_pressed(Key::D))
      view_center.x += m_view_speed * deltatime;

    m_view.set_center(view_center);
  }

  void ViewController::on_event(Event &e)
  {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(AERO_BIND_EVENT_FN(ViewController::on_mouse_scroll));
    dispatcher.Dispatch<WindowResizeEvent>(AERO_BIND_EVENT_FN(ViewController::on_window_resize));
    dispatcher.Dispatch<MouseButtonPressedEvent>(AERO_BIND_EVENT_FN(ViewController::on_mouse_clicked));
    dispatcher.Dispatch<MouseButtonReleasedEvent>(AERO_BIND_EVENT_FN(ViewController::on_mouse_released));
    dispatcher.Dispatch<MouseMovedEvent>(AERO_BIND_EVENT_FN(ViewController::on_mouse_moved));
  }

  bool ViewController::on_mouse_scroll(MouseScrolledEvent &e)
  {
    float scale_factor = 1.0f;
    if (e.get_offsetY() > 0)
      scale_factor = 0.9f;
    else
      scale_factor = 1.1f;

    m_view.zoom(scale_factor);
    return false;
  }

  bool ViewController::on_window_resize(WindowResizeEvent &e)
  {
    return false;
  }

  bool ViewController::on_mouse_clicked(MouseButtonPressedEvent &e)
  {
    if(e.get_mouse_button() == Button::ButtonMiddle)
    {
      m_middle_pressed = true;
      m_last_mouse_pos = Mouse::get_mouse_position();
    }
    return false;
  }

  bool ViewController::on_mouse_released(MouseButtonReleasedEvent &e)
  {
    if(e.get_mouse_button() == Button::ButtonMiddle)
    {
      m_middle_pressed = false;
    }
    return false;
  }

  bool ViewController::on_mouse_moved(MouseMovedEvent &e)
  {
    if(!m_middle_pressed) return false;

    vec2f current = Mouse::get_mouse_position();
    vec2f start = Mouse::get_mouse_world_position(m_last_mouse_pos, m_view);
    vec2f end = Mouse::get_mouse_world_position(current, m_view);

    m_view.move(start - end);

    m_last_mouse_pos = current;

    return false;
  }
}
