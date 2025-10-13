#pragma once

#include <Platform/OpenGL/OpenGLIndexBuffer.hpp>
#include <glad/glad.h>

namespace aero
{
  OpenGLIndexBuffer::OpenGLIndexBuffer(unsigned int *indices, uint32_t count)
  :m_count(count)
  {
    glGenBuffers(1, &m_ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
  }
  OpenGLIndexBuffer::~OpenGLIndexBuffer()
  {
    glDeleteBuffers(1, &m_ID);
  }

  void OpenGLIndexBuffer::bind() const
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
  }
  void OpenGLIndexBuffer::unbind() const
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

}
