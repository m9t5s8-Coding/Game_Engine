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
      float cos_r = cos(glm::radians(m_rotation));
      float sin_r = sin(glm::radians(m_rotation));

      glm::mat3 translate(1.0f); translate[2] = glm::vec3(-m_center.x, -m_center.y, 1.0f);
      glm::mat3 rotate(1.0f);
      rotate[0][0] = cos_r; rotate[0][1] = -sin_r;
      rotate[1][0] = sin_r; rotate[1][1] = cos_r;
      glm::mat3 scale = glm::mat3(1.0f); scale[0][0] = 2.0f / m_size.x; scale[1][1] = 2.0f / m_size.y;
      glm::mat3 flip = glm::mat3(1.0f); flip[1][1] = -1.0f;

      return flip * scale * rotate * translate;
    }

  private:
    vec2f m_size;
    vec2f m_center;
    float m_rotation = 0.0f;
  };

}
