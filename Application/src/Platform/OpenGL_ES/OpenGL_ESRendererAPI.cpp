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
  const char* version  = (const char*)glGetString(GL_VERSION);
  const char* renderer = (const char*)glGetString(GL_RENDERER);
  AERO_CORE_INFO("OpenGL ES Version: {}", version);
  AERO_CORE_INFO("OpenGL ES Renderer: {}", renderer);
  // Enable blending for transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Enable depth testing for 3D rendering
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Enable culling for performance (optional)
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  // Set default clear color
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  // Check for OpenGL errors
  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
  {
    AERO_CORE_ERROR("OpenGL ES Renderer init error: 0x{:04x}", error);
  }
  else
  {
    AERO_CORE_INFO("OpenGL ES Renderer initialized successfully");
  }

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
  while (glGetError() != GL_NO_ERROR)
  {
  }  // clear stale errors

  glDrawElements(GL_TRIANGLES,
                 p_vertexarray->get_index_buffer()->get_count(),
                 GL_UNSIGNED_INT,
                 nullptr);

  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
    AERO_CORE_ERROR("OpenGL ES draw_indexed error: 0x{:04x}", error);
}

void OpenGL_ESRendererAPI::draw_instanced(const AG_ref<VertexArray>& p_vertexarray,
                                          const AG_uint              instance_count)
{
  if (!p_vertexarray || !p_vertexarray->get_index_buffer())
  {
    AERO_CORE_ERROR("Cannot draw instanced: Invalid vertex array or index buffer");
    return;
  }

  glDrawElementsInstanced(GL_TRIANGLES,
                          p_vertexarray->get_index_buffer()->get_count(),
                          GL_UNSIGNED_INT,
                          nullptr,
                          instance_count);

  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
  {
    AERO_CORE_ERROR("OpenGL ES draw_instanced error: 0x{:04x}", error);
  }
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
