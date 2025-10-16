#pragma once

#include <memory>
#include <cstddef>
#include <Core/Core.hpp>

namespace ag
{
  class IndexBuffer
  {
  public:
    virtual ~IndexBuffer() {};
    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual uint32_t get_count() const = 0;
    virtual void set_count(uint32_t count) = 0;

    static AG_ref<IndexBuffer> create(unsigned int* indices, uint32_t size);
  };
}
