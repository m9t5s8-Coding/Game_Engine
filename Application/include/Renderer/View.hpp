#pragma once

#include <Math/Math.hpp>
#include <Core/TimeStamp.hpp>

#include <Events/WindowEvent.hpp>
#include <Events/MouseEvent.hpp>

namespace ag
{
  class View
  {
  public:
    View() {}
    View(const vec2f &size, const vec2f &center) : m_size(size), m_center(center) {}

    ~View() {}

    void set_size(const vec2f &size) { m_size = size; }
    vec2f get_size() const { return m_size; }

    void set_center(const vec2f &center) { m_center = center; }
    vec2f get_center() const { return m_center; }

    void zoom(const float zoom_factor) { m_size *= zoom_factor; }
    void move(const vec2f& move_factor) { m_center += move_factor; }
  private:
    vec2f m_size;
    vec2f m_center;
  };

}
