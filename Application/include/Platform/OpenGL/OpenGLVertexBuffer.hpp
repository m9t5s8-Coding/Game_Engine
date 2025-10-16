#pragma once

#include <Renderer/VertexBuffer.hpp>

namespace ag
{
  class OpenGLVertexBuffer : public VertexBuffer
  {
  public:
    OpenGLVertexBuffer(float *vertices, size_t size);
    virtual ~OpenGLVertexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual void set_data(const void *vertices, size_t size) override;

    virtual void set_layout(const BufferLayout &layout) override { m_layout = layout; }
    virtual const BufferLayout &get_layout() const override { return m_layout; };

  private:
    unsigned int m_ID;
    BufferLayout m_layout;
  };

}
