#pragma once

#ifdef PLATFORM_ANDROID

  #include <GLES3/gl3.h>

  #include <Renderer/FrameBuffer.hpp>
  #include <vector>

namespace ag
{
class OpenGL_ESFrameBuffer : public FrameBuffer
{
public:
  OpenGL_ESFrameBuffer(const FrameBufferSpecification& spec);
  virtual ~OpenGL_ESFrameBuffer();

  void invalidate();

  virtual void    bind() override;
  virtual void    unbind() override;
  virtual void    resize(const vec2u& size) override;
  virtual int     read_pixel(AG_uint attachment_index, const vec2i& pos) override;
  virtual void    clear_attachment(AG_uint attachment_index, int value) override;
  virtual AG_uint get_colorattachment_id(AG_uint index = 0) const override
  {
    return index < m_colorattachment.size() ? m_colorattachment[index] : 0;
  }
  virtual const FrameBufferSpecification& get_specification() const override
  {
    return m_specification;
  }

private:
  FrameBufferSpecification m_specification;
  GLuint                   m_ID = 0;

  void                                           cleanup();
  std::vector<FrameBuffer_Texture_Specification> m_color_attachment_specifications;
  FrameBuffer_Texture_Specification              m_depth_attachment_specifications =
      FrameBuffer_Texture_Format::None;

  std::vector<GLuint> m_colorattachment;
  GLuint              m_depthattachment = 0;
};

}  // namespace ag

#endif  // PLATFORM_ANDROID
