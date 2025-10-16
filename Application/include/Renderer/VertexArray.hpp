#pragma once

#include <Renderer/VertexBuffer.hpp>
#include <Renderer/IndexBuffer.hpp>

namespace ag
{
  class VertexArray
  {
  public:
    virtual ~VertexArray() {};

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual void add_vertex_buffer(const AG_ref<VertexBuffer>& p_vertexbuffer) = 0;
    virtual void set_index_buffer(const AG_ref<IndexBuffer>& p_indexbuffer) = 0;

    virtual AG_ref<IndexBuffer> get_index_buffer() const = 0;
    virtual AG_ref<VertexBuffer> get_vertex_buffer() const = 0;

    static AG_ref<VertexArray> create();
  };
}
