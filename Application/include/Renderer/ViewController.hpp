#pragma once

#include <Core/Core.hpp>
#include <Core/TimeStamp.hpp>
#include <Events/Event.hpp>
#include <Math/Math.hpp>
#include <Renderer/View.hpp>

namespace ag
{
  class ViewController
  {
  public:
    ViewController();
    ViewController(const vec2u& view_size, const vec2f& view_center);
    ~ViewController();

    void set_view(const vec2u& view_size, const vec2f& view_center);

    void on_update(TimeStamp ts);
    void on_event(Event &e);

    View &get_view() { return m_view; };
    const View &get_view() const { return m_view; };

    void set_viewport_mouse(const vec2f mouse_pos) { m_mouse_in_viewport = mouse_pos; }
    void set_viewport_size(const vec2f& viewport_size) { m_viewport_size = viewport_size; }

    void on_resize(const vec2f &size);

  private:
    bool on_mouse_scroll(MouseScrolledEvent &e);
    bool on_window_resize(WindowResizeEvent &e);
    bool on_mouse_clicked(MouseButtonPressedEvent &e);
    bool on_mouse_released(MouseButtonReleasedEvent &e);
    bool on_mouse_moved(MouseMovedEvent &e);


  private:
    View m_view;

    bool m_middle_pressed = false;
    bool m_view_ismax = false;
    bool m_view_ismin = false;

    vec2f m_last_mouse_pos;
    vec2f m_last_size;

    vec2f m_mouse_in_viewport;
    vec2f m_viewport_size;
  };
}
