#pragma once

#include <Renderer/VertexArray.hpp>
#include <memory>
#include <vector>

namespace aero
{
  class OpenGLVertexArray : public VertexArray
  {
    public:
    OpenGLVertexArray();

    virtual ~OpenGLVertexArray() {};

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual void add_vertex_buffer(const std::shared_ptr<VertexBuffer>& p_vertexbuffer) override;
    virtual void set_index_buffer(const std::shared_ptr<IndexBuffer>& p_indexbuffer) override;

    virtual std::shared_ptr<IndexBuffer> get_index_buffer() const { return m_indexbuffer; }

    private:
      std::vector<std::shared_ptr<VertexBuffer>> m_vertexbuffers;
      std::shared_ptr<IndexBuffer> m_indexbuffer;
      unsigned int m_ID;
  };

}

