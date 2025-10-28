#pragma once

#include <Renderer/FrameBuffer.hpp>

namespace ag
{
  class OpenGLFrameBuffer : public FrameBuffer
  {
  public:
    OpenGLFrameBuffer(const FrameBufferSpecification &spec);

    virtual ~OpenGLFrameBuffer();

    void invalidate();

    virtual void bind() override;
    virtual void unbind() override;

    virtual void resize(const vec2u& size) override;

    virtual AG_uint get_colorattachment_id() const override { return m_colorattachment; };
    virtual const FrameBufferSpecification& get_specification() const override { return m_specification; }
  private:
      FrameBufferSpecification m_specification;
      AG_uint m_colorattachment = 0;
      AG_uint m_depthattachment = 0;
      AG_uint m_ID = 0;
  };

}
