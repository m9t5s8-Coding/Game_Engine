#pragma once

#include <Renderer/VertexBuffer.hpp>

namespace aero
{
  class VertexArray
  {
  public:
    VertexArray();
    ~VertexArray();
    virtual void bind() = 0;
    virtual void unbind() = 0;
  };
}
