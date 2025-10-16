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

  private:
    bool on_mouse_scroll(MouseScrolledEvent &e);
    bool on_window_resize(WindowResizeEvent &e);
    bool on_mouse_clicked(MouseButtonPressedEvent &e);
    bool on_mouse_released(MouseButtonReleasedEvent &e);
    bool on_mouse_moved(MouseMovedEvent &e);


  private:
    View m_view;

    bool m_middle_pressed = false;
    vec2f m_last_mouse_pos;
    float m_view_speed = 1000.0f;
  };
}
