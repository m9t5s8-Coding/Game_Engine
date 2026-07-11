#ifdef PLATFORM_ANDROID

  #include <GLES3/gl3.h>

  #include <Core/Log.hpp>
  #include <Platform/OpenGL_ES/OpenGL_ESRendererAPI.hpp>

namespace ag
{

void OpenGL_ESRendererAPI::set_clear_color(const Color& p_clear_color)
{
  vec4f color;
  p_clear_color.normalize_color(color);
  glClearColor(color.x, color.y, color.z, color.w);
}

void OpenGL_ESRendererAPI::clear()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL_ESRendererAPI::init()
{
  if (m_initialized)
    return;
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDisable(GL_DEPTH_TEST);

  m_initialized = true;
}

void OpenGL_ESRendererAPI::set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
  glViewport(x, y, width, height);
}

void OpenGL_ESRendererAPI::bind(AG_uint id, AG_uint p_slot)
{
  glActiveTexture(GL_TEXTURE0 + p_slot);
  glBindTexture(GL_TEXTURE_2D, id);
}

void OpenGL_ESRendererAPI::draw_indexed(const std::shared_ptr<VertexArray>& p_vertexarray)
{
  glDrawElements(GL_TRIANGLES,
                 p_vertexarray->get_index_buffer()->get_count(),
                 GL_UNSIGNED_INT,
                 nullptr);
}

void OpenGL_ESRendererAPI::draw_instanced(const AG_ref<VertexArray>& p_vertexarray,
                                          const AG_uint              instance_count)
{
  glDrawElementsInstanced(GL_TRIANGLES,
                          p_vertexarray->get_index_buffer()->get_count(),
                          GL_UNSIGNED_INT,
                          nullptr,
                          instance_count);
}

void OpenGL_ESRendererAPI::enable_blend()
{
  glEnable(GL_BLEND);
}

void OpenGL_ESRendererAPI::disable_blend()
{
  glDisable(GL_BLEND);
}

}  // namespace ag

#endif  // PLATFORM_ANDROID
