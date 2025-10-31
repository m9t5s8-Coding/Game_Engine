#include <Apch.hpp>
#include <Renderer/ViewController.hpp>
#include <Renderer/View.hpp>
#include <Core/Input.hpp>
#include <Core/Core.hpp>
#include <Core/Application.hpp>
namespace ag
{
  ViewController::ViewController()
  {
    vec2u window_size = Application::get().get_window().get_size();
    m_last_size = vec2f(window_size.x, window_size.y);
  }
  ViewController::ViewController(const vec2u& view_size, const vec2f& view_center)
    : ViewController()
  {
    m_view.set_size(view_size);
    m_view.set_center(view_center);
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
    if (m_view.get_size().x >= 3000)
    {
      float aspect_ratio = m_view.get_size().x / m_view.get_size().y;
      m_view.set_size({ 3000, 3000 / aspect_ratio });
      m_view_ismax = true;
    }
    else if (m_view.get_size().x <= 100)
    {
      float aspect_ratio = m_view.get_size().x / m_view.get_size().y;
      m_view.set_size({ 100, 100 / aspect_ratio });
      m_view_ismin = true;
    }
    else
    {
      m_view_ismax = false;
      m_view_ismin = false;
    }
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
    bool is_zoom_in = false;
    if (e.get_offsetY() > 0)
    {
      is_zoom_in = true;
      scale_factor = 0.9f;
    }
    else
    {
      is_zoom_in = false;
      scale_factor = 1.1f;
    }
    vec2f before_zoom = Math::screen_to_world(m_mouse_in_viewport, m_view.get_float_rect(), m_viewport_size);
    m_view.zoom(scale_factor);

    vec2f after_zoom = Math::screen_to_world(m_mouse_in_viewport, m_view.get_float_rect(), m_viewport_size);

    vec2f offset = before_zoom - after_zoom;
    
    if ((is_zoom_in && !m_view_ismin) || (!is_zoom_in && !m_view_ismax))
    {
      m_view.move(offset);
    }
    return false;
  }

  bool ViewController::on_window_resize(WindowResizeEvent &e)
  {
    vec2f window_size = e.get_size();
    on_resize(window_size);
    return false;
  }
  void ViewController::on_resize(const vec2f& size)
  {
    float scale_x = size.x / m_last_size.x;
    float scale_y = size.y / m_last_size.y;

    vec2f view_size = m_view.get_size();
    view_size.x *= scale_x;
    view_size.y *= scale_y;

    m_view.set_size(view_size);

    m_last_size = size;
  }

  bool ViewController::on_mouse_clicked(MouseButtonPressedEvent &e)
  {
    if(e.get_mouse_button() == Button::ButtonMiddle)
    {
      m_middle_pressed = true;
      m_last_mouse_pos = m_mouse_in_viewport;
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

    vec2f current = m_mouse_in_viewport;
    vec2f start = Math::screen_to_world(m_last_mouse_pos, m_view.get_float_rect(), m_viewport_size);
    vec2f end = Math::screen_to_world(current, m_view.get_float_rect(), m_viewport_size);

    m_view.move(start - end);

    m_last_mouse_pos = current;

    return false;
  }
}
