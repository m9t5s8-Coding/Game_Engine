#pragma once

#include <Core/Core.hpp>
#include <cstddef>
#include <Renderer/BufferLayout.hpp>

namespace ag
{
class VertexBuffer
{
public:
  virtual ~VertexBuffer() {};
  virtual void bind() const   = 0;
  virtual void unbind() const = 0;

  virtual void set_data(const void* vertices, size_t size) = 0;

  virtual void                set_layout(const BufferLayout& layout) = 0;
  virtual const BufferLayout& get_layout() const                     = 0;

  static AG_ref<VertexBuffer> create(float* vertices, size_t size);
  static AG_ref<VertexBuffer> create(size_t size);
};
}  // namespace ag
