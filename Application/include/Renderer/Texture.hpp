#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <Math/Math.hpp>
#include <Core/Core.hpp>

namespace ag
{
  class Texture
  {
  public:
    virtual ~Texture() = default;

    virtual AG_uint get_width() const = 0;
    virtual AG_uint get_height() const = 0;
    virtual vec2u get_size() const = 0;

    virtual void bind(uint32_t p_slot = 0) const = 0;
  };

  class Texture2D : public Texture
  {
  public:
    static std::shared_ptr<Texture2D> create(const std::string& p_path);
  };
}
