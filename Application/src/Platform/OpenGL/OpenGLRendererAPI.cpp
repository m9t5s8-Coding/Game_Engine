#include <Platform/OpenGL/OpenGLRendererAPI.hpp>
#include <Apch.hpp>
#include <Core/Application.hpp>

#include <glad/glad.h>

namespace ag
{
  void OpenGLRendererAPI::set_clear_color(const Color &p_clear_color)
  {
    vec4f color;
    p_clear_color.normalize_color(color);
    glClearColor(color.x, color.y, color.z, color.w);
  }
  void OpenGLRendererAPI::clear()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    vec2f window_size = Application::get().get_window().get_size();
    glOrtho(0, window_size.x, window_size.y, 0, -1.0, 5.0);

    // Set modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void OpenGLRendererAPI::init()
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
  }

  void OpenGLRendererAPI::set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
  {
    glViewport(x, y, width, height);
  }

  void OpenGLRendererAPI::draw_indexed(const std::shared_ptr<VertexArray> &p_vertexarray)
  {
    glDrawElements(GL_TRIANGLES, p_vertexarray->get_index_buffer()->get_count(), GL_UNSIGNED_INT, nullptr);
  }

  void OpenGLRendererAPI::draw_instanced(const AG_ref<VertexArray> &p_vertexarray, const AG_uint instance_count)
  {
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, instance_count);
  }
}
