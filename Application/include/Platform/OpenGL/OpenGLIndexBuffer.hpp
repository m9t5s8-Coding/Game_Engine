#pragma once

#include <Renderer/IndexBuffer.hpp>

namespace ag
{
  class OpenGLIndexBuffer : public IndexBuffer
  {
  public:
    OpenGLIndexBuffer(AG_uint* indices, uint32_t count);
    virtual ~OpenGLIndexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual void set_count(AG_uint count) override;

    virtual AG_uint get_count() const override { return m_count; }
  private:
    unsigned int m_ID;
    AG_uint m_count;
  };

}
