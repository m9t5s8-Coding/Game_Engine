#pragma once

#include <Core/Core.hpp>
#include <cstdint>
#include <Math/Math.hpp>
#include <memory>
#include <string>

namespace ag
{
enum Filter_Mode
{
  AG_NEAREST = 0,
  AG_LINEAR  = 1
};

class Texture
{
public:
  virtual ~Texture() = default;

  virtual AG_uint get_width() const  = 0;
  virtual AG_uint get_height() const = 0;
  virtual vec2u   get_size() const   = 0;

  virtual AG_uint get_texture_id() const = 0;

  virtual void bind(AG_uint p_slot = 1) const = 0;
};

class Texture2D : public Texture
{
public:
  static std::shared_ptr<Texture2D> create(const std::string& p_path,
                                           bool               reload,
                                           Filter_Mode        filter_mode,
                                           bool               engine_source = false);
};
}  // namespace ag
