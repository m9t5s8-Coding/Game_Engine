#pragma once

#include <Renderer/IndexBuffer.hpp>

namespace aero
{
  class OpenGLIndexBuffer : public IndexBuffer
  {
  public:
    OpenGLIndexBuffer(unsigned int* indices, size_t size);
    virtual ~OpenGLIndexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;
  private:
    unsigned int m_ID;
  };

}
