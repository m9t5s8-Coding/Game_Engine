#pragma once

#include <Renderer/VertexBuffer.hpp>

namespace aero
{
  class OpenGLVertexBuffer : public VertexBuffer
  {
  public:
    OpenGLVertexBuffer(float *vertices, size_t size);
    virtual ~OpenGLVertexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;

  private:
    unsigned int m_ID;
  };

}
