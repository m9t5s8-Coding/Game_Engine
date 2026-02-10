#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)

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

    virtual int read_pixel(AG_uint attachment_index, const vec2i& pos) override;
    virtual void clear_attachment(AG_uint attachment_index, int value) override;

    virtual AG_uint get_colorattachment_id(AG_uint index = 0) const { return m_colorattachment[index]; };

    virtual const FrameBufferSpecification& get_specification() const override { return m_specification; }


  private:
      FrameBufferSpecification m_specification;
      AG_uint m_ID = 0;


      std::vector<FrameBuffer_Texture_Specification> m_color_attachment_specifications;
      FrameBuffer_Texture_Specification m_depth_attachment_specifications = FrameBuffer_Texture_Format::None;

      std::vector<AG_uint> m_colorattachment;
      AG_uint m_depthattachment = 0;

  };

}

#endif
