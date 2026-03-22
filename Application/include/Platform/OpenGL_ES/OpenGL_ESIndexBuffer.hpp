#pragma once

#ifdef PLATFORM_ANDROID

  #include <Renderer/IndexBuffer.hpp>

namespace ag
{
class OpenGL_ESIndexBuffer : public IndexBuffer
{
public:
  OpenGL_ESIndexBuffer(AG_uint* indices, uint32_t count);
  virtual ~OpenGL_ESIndexBuffer();

  virtual void bind() const override;
  virtual void unbind() const override;

  virtual void set_count(AG_uint count) override;

  virtual AG_uint get_count() const override
  {
    return m_count;
  }

private:
  AG_uint m_ID;
  AG_uint m_count;
};

}  // namespace ag

#endif  // PLATFORM_ANDROID
