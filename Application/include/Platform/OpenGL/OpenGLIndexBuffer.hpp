#pragma once

#include <Renderer/IndexBuffer.hpp>

namespace ag
{
  class OpenGLIndexBuffer : public IndexBuffer
  {
  public:
    OpenGLIndexBuffer(unsigned int* indices, uint32_t count);
    virtual ~OpenGLIndexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual void set_count(uint32_t count) override;

    virtual uint32_t get_count() const override { return m_count; }
  private:
    unsigned int m_ID;
    uint32_t m_count;
  };

}
