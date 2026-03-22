#pragma once

#ifdef PLATFORM_ANDROID

  #include <GLES3/gl3.h>

  #include <memory>
  #include <Renderer/VertexArray.hpp>
  #include <vector>

namespace ag
{
class OpenGL_ESVertexArray : public VertexArray
{
public:
  OpenGL_ESVertexArray();
  virtual ~OpenGL_ESVertexArray();

  virtual void bind() const override;
  virtual void unbind() const override;

  virtual void add_vertex_buffer(const AG_ref<VertexBuffer>& p_vertexbuffer,
                                 bool                        instanced = false) override;
  virtual void set_index_buffer(const AG_ref<IndexBuffer>& p_indexbuffer) override;

  virtual AG_ref<IndexBuffer> get_index_buffer() const override
  {
    return m_indexbuffer;
  }

private:
  std::vector<AG_ref<VertexBuffer>> m_vertexbuffers;
  AG_ref<IndexBuffer>               m_indexbuffer;
  GLuint                            m_ID;
};

}  // namespace ag

#endif  // PLATFORM_ANDROID
