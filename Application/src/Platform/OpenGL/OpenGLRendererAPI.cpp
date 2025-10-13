#include <Platform/OpenGL/OpenGLRendererAPI.hpp>
#include <Apch.hpp>

#include <glad/glad.h>

namespace aero
{
  void OpenGLRendererAPI::set_clear_color(const Color &p_clear_color)
  {
    float r, g, b, a;
    p_clear_color.NormalizedColor(r, g, b, a);
    glClearColor(r, g, b, a);
  }
  void OpenGLRendererAPI::clear()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void OpenGLRendererAPI::init()
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
  }

  void OpenGLRendererAPI::set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
  {
    glViewport(x, y, width, height);
  }

  void OpenGLRendererAPI::draw_indexed(const std::shared_ptr<VertexArray> &p_vertexarray)
  {
    glDrawElements(GL_TRIANGLES, p_vertexarray->get_index_buffer()->get_count(), GL_UNSIGNED_INT, nullptr);
  }
}
