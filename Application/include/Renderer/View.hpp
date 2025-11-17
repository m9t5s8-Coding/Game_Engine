#pragma once

#include <Math/Math.hpp>
#include <Core/TimeStamp.hpp>

#include <Events/WindowEvent.hpp>
#include <Events/MouseEvent.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

    void set_rotation(const float rotation) { m_rotation = rotation; }
    float get_rotation() const { return m_rotation; }

    float_rect get_float_rect() const { return float_rect(m_center - m_size/2, m_size); }

    void zoom(const float zoom_factor) { m_size *= zoom_factor; }
    void move(const vec2f& move_factor) { m_center += move_factor; }

    glm::mat3 get_view_matrix() const
    {
      return Math::get_view_matrix(m_size, m_center, m_rotation);
    }

  private:
    vec2f m_size;
    vec2f m_center;
    float m_rotation = 0.0f;
  };

}
