#pragma once

#include <cstddef>
namespace aero
{
  class IndexBuffer
  {
  public:
    virtual ~IndexBuffer() {};
    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    static IndexBuffer* create(unsigned int* indices, size_t size);
  };
}
