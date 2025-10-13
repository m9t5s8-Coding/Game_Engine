#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace aero
{
  class Texture
  {
  public:
    virtual ~Texture() = default;

    virtual uint32_t get_width() const = 0;
    virtual uint32_t get_height() const = 0;

    virtual void bind(uint32_t p_slot = 0) const = 0;
  };

  class Texture2D : public Texture
  {
  public:
    static std::shared_ptr<Texture2D> create(const std::string& p_path);
  };
}
