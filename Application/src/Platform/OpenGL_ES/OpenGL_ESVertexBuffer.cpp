#ifdef PLATFORM_ANDROID

  #include <GLES3/gl3.h>

  #include <Core/Log.hpp>
  #include <Platform/OpenGL_ES/OpenGL_ESVertexBuffer.hpp>

namespace ag
{

OpenGL_ESVertexBuffer::OpenGL_ESVertexBuffer(float* vertices, size_t size)
{
  glGenBuffers(1, &m_ID);
  glBindBuffer(GL_ARRAY_BUFFER, m_ID);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

  m_buffer_size = size;
  GLenum error  = glGetError();
  if (error != GL_NO_ERROR)
  {
    AERO_CORE_ERROR("OpenGL ES VertexBuffer creation error: 0x{:04x}", error);
  }
}

OpenGL_ESVertexBuffer::OpenGL_ESVertexBuffer(size_t size)
{
  glGenBuffers(1, &m_ID);
  glBindBuffer(GL_ARRAY_BUFFER, m_ID);
  glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);  // ← DYNAMIC_DRAW
  m_buffer_size = size;
  GLenum error  = glGetError();
  if (error != GL_NO_ERROR)
    AERO_CORE_ERROR("OpenGL ES VertexBuffer dynamic creation error: 0x{:04x}", error);
}

OpenGL_ESVertexBuffer::~OpenGL_ESVertexBuffer()
{
  glDeleteBuffers(1, &m_ID);
}

void OpenGL_ESVertexBuffer::bind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

void OpenGL_ESVertexBuffer::unbind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGL_ESVertexBuffer::set_data(const void* vertices, size_t size)
{
  glBindBuffer(GL_ARRAY_BUFFER, m_ID);

  glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);

  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
  {
    AERO_CORE_ERROR("OpenGL ES VertexBuffer set_data error: 0x{:04x}, size: {}, bufferSize: {}",
                    error,
                    size,
                    m_buffer_size);
  }
}
}  // namespace ag

#endif  // PLATFORM_ANDROID
