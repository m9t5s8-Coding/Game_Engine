#pragma once

#include <Core/Core.hpp>
#include <cstddef>
#include <memory>

namespace ag {
class IndexBuffer {
public:
  virtual ~IndexBuffer() {};
  virtual void bind() const   = 0;
  virtual void unbind() const = 0;

  virtual AG_uint get_count() const        = 0;
  virtual void    set_count(AG_uint count) = 0;

  static AG_ref<IndexBuffer> create(AG_uint* indices, AG_uint size);
};
}  // namespace ag
