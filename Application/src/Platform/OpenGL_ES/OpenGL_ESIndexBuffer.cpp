#ifdef PLATFORM_ANDROID

  #include <GLES3/gl3.h>

  #include <Core/Log.hpp>
  #include <Platform/OpenGL_ES/OpenGL_ESIndexBuffer.hpp>

namespace ag
{
OpenGL_ESIndexBuffer::OpenGL_ESIndexBuffer(AG_uint* indices, uint32_t count)
  : m_count(count)
{
  glGenBuffers(1, &m_ID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);

  // Check for OpenGL errors
  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
  {
    AERO_CORE_ERROR("OpenGL ES IndexBuffer creation error: 0x{:04x}", error);
  }
}

OpenGL_ESIndexBuffer::~OpenGL_ESIndexBuffer()
{
  glDeleteBuffers(1, &m_ID);
}

void OpenGL_ESIndexBuffer::bind() const
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
}

void OpenGL_ESIndexBuffer::unbind() const
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGL_ESIndexBuffer::set_count(AG_uint count)
{
  m_count = count;
}

}  // namespace ag

#endif  // PLATFORM_ANDROID
