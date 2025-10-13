#pragma once

#include <memory>
#include <cstddef>

namespace aero
{
  class IndexBuffer
  {
  public:
    virtual ~IndexBuffer() {};
    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual uint32_t get_count() const = 0;

    static IndexBuffer* create(unsigned int* indices, size_t size);
  };
}
