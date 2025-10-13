#pragma once

#include <Renderer/VertexBuffer.hpp>
#include <Renderer/IndexBuffer.hpp>

namespace aero
{
  class VertexArray
  {
  public:
    virtual ~VertexArray() {};

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual void add_vertex_buffer(const std::shared_ptr<VertexBuffer>& p_vertexbuffer) = 0;
    virtual void set_index_buffer(const std::shared_ptr<IndexBuffer>& p_indexbuffer) = 0;

    virtual std::shared_ptr<IndexBuffer> get_index_buffer() const = 0;

    static VertexArray* create();
  };
}
