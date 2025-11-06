#pragma once

#include <Renderer/VertexArray.hpp>
#include <memory>
#include <vector>

namespace ag
{
  class OpenGLVertexArray : public VertexArray
  {
    public:
    OpenGLVertexArray();

    virtual ~OpenGLVertexArray() {};

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual void add_vertex_buffer(const AG_ref<VertexBuffer>& p_vertexbuffer, bool instanced = false) override;
    virtual void set_index_buffer(const AG_ref<IndexBuffer>& p_indexbuffer) override;

    virtual AG_ref<IndexBuffer> get_index_buffer() const override { return m_indexbuffer; }
    // virtual AG_ref<VertexBuffer> get_vertex_buffer() const override { return m_vertexbuffer; };

    private:
      std::vector<AG_ref<VertexBuffer>> m_vertexbuffers;
      AG_ref<IndexBuffer> m_indexbuffer;
      unsigned int m_ID;
  };

}

