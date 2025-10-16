#pragma once

#include <cstddef>
#include <Renderer/BufferLayout.hpp>
#include <Core/Core.hpp>

namespace ag
{
  class VertexBuffer
  {
  public:
    virtual ~VertexBuffer() {};
    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual void set_data(const void *vertices, size_t size) = 0;

    virtual void set_layout(const BufferLayout &layout) = 0;
    virtual const BufferLayout& get_layout() const = 0;

    static AG_ref<VertexBuffer> create(float *vertices, size_t size);
  };
}
