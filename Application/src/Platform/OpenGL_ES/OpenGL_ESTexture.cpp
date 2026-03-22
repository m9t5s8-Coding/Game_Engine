#ifdef PLATFORM_ANDROID

  #include <android/log.h>
  #include <EGL/egl.h>
  #include <GLES3/gl3.h>

  #include <Core/Log.hpp>
  #include <Platform/OpenGL_ES/OpenGL_ESTexture.hpp>
  #include <Project/Assetmanager.hpp>

  #define STBI_NO_SIMD
  #include <stb_image.h>

namespace ag
{

OpenGL_ESTexture2D::OpenGL_ESTexture2D(const std::string& p_path,
                                       Filter_Mode        filter_mode,
                                       bool               engine_source)
  : m_path(p_path)
{
  int      width = 0, height = 0, channels = 0;
  stbi_uc* pixels = nullptr;

  // Load texture from appropriate source
  if (AssetManager::is_packed(AssetManager::Domain::Engine) && engine_source)
  {
    auto bytes = AssetManager::read_bytes(p_path, AssetManager::Domain::Engine);
    if (bytes.empty())
    {
      AERO_CORE_ERROR("Failed to load texture from engine pak: {}", p_path);
      return;
    }
    pixels = stbi_load_from_memory(bytes.data(),
                                   (int)bytes.size(),
                                   &width,
                                   &height,
                                   &channels,
                                   STBI_rgb_alpha);
  }
  else if (AssetManager::is_packed(AssetManager::Domain::Project))
  {
    auto bytes = AssetManager::read_bytes(p_path, AssetManager::Domain::Project);
    if (bytes.empty())
    {
      AERO_CORE_ERROR("Failed to load texture from project pak: {}", p_path);
      return;
    }
    pixels = stbi_load_from_memory(bytes.data(),
                                   (int)bytes.size(),
                                   &width,
                                   &height,
                                   &channels,
                                   STBI_rgb_alpha);
  }
  else
  {
    pixels = stbi_load(p_path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
  }

  if (!pixels)
  {
    AERO_CORE_ERROR("stb_image failed to load: {}", p_path);
    return;
  }

  m_size = vec2u((uint32_t)width, (uint32_t)height);

  // Check EGL context is current
  EGLContext ctx = eglGetCurrentContext();
  AERO_CORE_INFO("EGL context when loading {}: {}", p_path, (void*)ctx);
  if (ctx == EGL_NO_CONTEXT)
  {
    AERO_CORE_ERROR("No EGL context current! Texture upload deferred needed for: {}", p_path);
    stbi_image_free(pixels);
    return;
  }

  // Clear any pre-existing errors
  while (glGetError() != GL_NO_ERROR)
  {
  }

  glGenTextures(1, &m_ID);
  if (glGetError() != GL_NO_ERROR)
  {
    AERO_CORE_ERROR("glGenTextures failed: {}", p_path);
    return;
  }

  glBindTexture(GL_TEXTURE_2D, m_ID);
  if (glGetError() != GL_NO_ERROR)
  {
    AERO_CORE_ERROR("glBindTexture failed: {}", p_path);
    return;
  }

  switch (filter_mode)
  {
    case AG_NEAREST:
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      break;
    case AG_LINEAR:
    default:
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      break;
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  if (glGetError() != GL_NO_ERROR)
  {
    AERO_CORE_ERROR("glTexImage2D failed: {} ({}x{})", p_path, width, height);
    stbi_image_free(pixels);
    return;
  }

  glGenerateMipmap(GL_TEXTURE_2D);
  if (glGetError() != GL_NO_ERROR)
  {
    AERO_CORE_ERROR("glGenerateMipmap failed: {}", p_path);
  }

  if (filter_mode == AG_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  // Check for OpenGL errors
  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
  {
    AERO_CORE_ERROR("OpenGL ES texture error: 0x{:04x} for texture: {}", error, p_path);

  #ifdef PLATFORM_ANDROID
    __android_log_print(ANDROID_LOG_ERROR,
                        "Texture",
                        "OpenGL ES texture error: 0x%04x for texture: %s",
                        error,
                        p_path.c_str());
  #endif
  }
  else
  {
    AERO_CORE_INFO("Texture loaded successfully: {} ({}x{})", p_path, width, height);
  }

  // Clean up
  stbi_image_free(pixels);
  glBindTexture(GL_TEXTURE_2D, 0);
}

OpenGL_ESTexture2D::~OpenGL_ESTexture2D()
{
  if (m_ID)
  {
    glDeleteTextures(1, &m_ID);
  }
}

void OpenGL_ESTexture2D::bind(uint32_t p_slot) const
{
  glActiveTexture(GL_TEXTURE0 + p_slot);
  glBindTexture(GL_TEXTURE_2D, m_ID);
}

}  // namespace ag

#endif  // PLATFORM_ANDROID
