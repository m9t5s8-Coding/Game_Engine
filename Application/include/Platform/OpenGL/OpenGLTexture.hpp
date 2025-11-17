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

    virtual AG_uint get_texture_id() const override { return m_ID; }

    virtual void bind(AG_uint p_slot = 0) const override;

  private:
    std::string m_path;
    vec2u m_size;
    AG_uint m_ID;
  };
}
