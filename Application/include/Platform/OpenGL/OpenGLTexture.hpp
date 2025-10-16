#pragma once

#include <Renderer/Texture.hpp>

namespace ag
{
  class OpenGLTexture2D : public Texture2D
  {
  public:
    OpenGLTexture2D(const std::string& p_path);
    virtual ~OpenGLTexture2D();

    virtual AG_uint get_width() const override { return m_size.x; };
    virtual AG_uint get_height() const override { return m_size.y; };
    virtual vec2u get_size() const override { return m_size; }

    virtual void bind(uint32_t p_slot = 0) const override;

  private:
    std::string m_path;
    vec2u m_size;
    uint32_t m_ID;
  };
}
