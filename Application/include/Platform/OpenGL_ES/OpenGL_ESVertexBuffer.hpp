#pragma once

#ifdef PLATFORM_ANDROID

  #include <GLES3/gl3.h>

  #include <Renderer/VertexBuffer.hpp>

namespace ag
{
class OpenGL_ESVertexBuffer : public VertexBuffer
{
public:
  OpenGL_ESVertexBuffer(float* vertices, size_t size);
  OpenGL_ESVertexBuffer(size_t);
  virtual ~OpenGL_ESVertexBuffer();

  virtual void bind() const override;
  virtual void unbind() const override;

  virtual void set_data(const void* vertices, size_t size) override;

  virtual void set_layout(const BufferLayout& layout) override
  {
    m_layout = layout;
  }
  virtual const BufferLayout& get_layout() const override
  {
    return m_layout;
  }

private:
  GLuint       m_ID;
  BufferLayout m_layout;
  size_t       m_buffer_size;
};

}  // namespace ag

#endif  // PLATFORM_ANDROID
