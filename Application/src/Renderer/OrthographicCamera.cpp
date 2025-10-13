#include <Renderer/OrthographicCamera.hpp>
#include <Apch.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace aero
{
  OrthographicCamera::OrthographicCamera(float p_left, float p_right, float p_bottom, float p_top)
      : m_projection(glm::ortho(p_left, p_right, p_bottom, p_top, -1.0f, 1.0f)), m_view(1.0f)
  {
    m_view_projection = m_projection * m_view;
  }

  void OrthographicCamera::set_projection(float p_left, float p_right, float p_bottom, float p_top)
  {
    m_projection = glm::ortho(p_left, p_right, p_bottom, p_top, -1.0f, 1.0f);
    m_view_projection = m_projection * m_view;
  }

  void OrthographicCamera::recalc_view_matrix()
  {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation), glm::vec3(0, 0, 1));

    m_view = glm::inverse(transform);
    m_view_projection = m_projection * m_view;
  }
}

