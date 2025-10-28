#pragma once

#include <Core/Core.hpp>
#include <Math/Math.hpp>

namespace ag
{
  struct FrameBufferSpecification
  {
    vec2u size;
    AG_uint samples = 1;

    bool swap_chain_target = false;
  };

  class FrameBuffer
  {
  public:
    virtual ~FrameBuffer() {};

    virtual void bind() = 0;
    virtual void unbind() = 0;

    virtual void resize(const vec2u& size) = 0;
    virtual AG_uint get_colorattachment_id() const = 0;
    virtual const FrameBufferSpecification& get_specification() const = 0;
    static AG_ref<FrameBuffer> create(const FrameBufferSpecification &spec);
  };
}
