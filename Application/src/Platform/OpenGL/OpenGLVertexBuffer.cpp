#include <Platform/OpenGL/OpenGLVertexBuffer.hpp>
#include <glad/glad.h>

namespace aero
{
  OpenGLVertexBuffer::OpenGLVertexBuffer(float *vertices, size_t size)
  {
    glGenBuffers(1, &m_ID);
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
  }

  OpenGLVertexBuffer::~OpenGLVertexBuffer()
  {
    glDeleteBuffers(1, &m_ID);
  }

  void OpenGLVertexBuffer::bind() const
  {
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
  }

  void OpenGLVertexBuffer::unbind() const
  {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}
