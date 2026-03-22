#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)

  #include <glad/glad.h>

  #include <Platform/OpenGL/OpenGLVertexBuffer.hpp>

namespace ag
{
OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, size_t size)
{
  glGenBuffers(1, &m_ID);
  glBindBuffer(GL_ARRAY_BUFFER, m_ID);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(size_t size)
{
  glGenBuffers(1, &m_ID);
  glBindBuffer(GL_ARRAY_BUFFER, m_ID);
  glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}
OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
  glDeleteBuffers(1, &m_ID);
}

void OpenGLVertexBuffer::bind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

void OpenGLVertexBuffer::set_data(const void* vertices, size_t size)
{
  glBindBuffer(GL_ARRAY_BUFFER, m_ID);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
}

void OpenGLVertexBuffer::unbind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
}  // namespace ag

#endif
