#pragma once

#include <cstddef>
#include <Renderer/BufferLayout.hpp>

namespace aero
{
  class VertexBuffer
  {
  public:
    virtual ~VertexBuffer() {};
    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual void set_layout(const BufferLayout &layout) = 0;
    virtual const BufferLayout& get_layout() const = 0;

    static VertexBuffer *create(float *vertices, size_t size);
  };
}
