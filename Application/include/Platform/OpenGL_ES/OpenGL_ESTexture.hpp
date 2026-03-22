#pragma once

#ifdef PLATFORM_ANDROID

  #include <GLES3/gl3.h>

  #include <Renderer/Texture.hpp>
  #include <string>

namespace ag
{
class OpenGL_ESTexture2D : public Texture2D
{
public:
  OpenGL_ESTexture2D(const std::string& p_path, Filter_Mode filter_mode, bool engine_source);
  virtual ~OpenGL_ESTexture2D();

  virtual AG_uint get_width() const override
  {
    return m_size.x;
  }
  virtual AG_uint get_height() const override
  {
    return m_size.y;
  }
  virtual vec2u get_size() const override
  {
    return m_size;
  }
  virtual AG_uint get_texture_id() const override
  {
    return m_ID;
  }
  virtual void bind(AG_uint p_slot = 1) const override;

private:
  std::string m_path;
  vec2u       m_size;
  GLuint      m_ID;
};

}  // namespace ag

#endif  // PLATFORM_ANDROID
