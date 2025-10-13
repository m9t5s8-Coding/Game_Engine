#pragma once

#include <glm/glm.hpp>

namespace aero
{
  class OrthographicCamera
  {
  public:
    OrthographicCamera(float p_left, float p_right, float p_bottom, float p_top);
    void set_projection(float p_left, float p_right, float p_bottom, float p_top);

    void set_position(const glm::vec3& p_position) { m_position = p_position; recalc_view_matrix(); }
    const glm::vec3& get_position() const { return m_position; }

    void set_rotation(float p_rotation) { m_rotation = p_rotation; recalc_view_matrix(); }
    float get_rotation() const { return m_rotation; }

    const glm::mat4& get_projection_mat() const { return m_projection; }
    const glm::mat4& get_view_mat() const { return m_view; }
    const glm::mat4& get_view_projection_mat() const { return m_view_projection; }

  private:
    void recalc_view_matrix();
  private:
    glm::mat4 m_projection;
    glm::mat4 m_view;
    glm::mat4 m_view_projection;

    glm::vec3 m_position = { 0.0f, 0.0f, 0.0f };
    float m_rotation = 0.0f;
  };

}
