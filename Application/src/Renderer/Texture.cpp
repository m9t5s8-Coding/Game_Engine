#include <Apch.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Texture.hpp>

#if defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS)
  #include <Platform/OpenGL/OpenGLTexture.hpp>
#elif defined(PLATFORM_ANDROID)
  #include <Platform/OpenGL_ES/OpenGL_ESTexture.hpp>
#endif

namespace ag
{
std::shared_ptr<Texture2D> Texture2D::create(const std::string& p_path,
                                             bool               reload,
                                             Filter_Mode        filter_mode,
                                             bool               engine_source)
{
  // if noone is using the texture then the texture will be deleted
  static std::unordered_map<std::string, std::weak_ptr<Texture2D>> s_texture_cache;
  switch (Renderer::get_API())
  {
    case RendererAPI::API::None:
      AERO_CORE_ASSERT(false, "RendererAPI::None is not supported!");
      return nullptr;
    case RendererAPI::API::OpenGL:
    {
#if defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS)
      if (!reload)
      {
        if (auto cached = s_texture_cache[p_path].lock())
        {
          return cached;
        }
      }
      auto texture = std::make_shared<OpenGLTexture2D>(p_path, filter_mode, engine_source);
      s_texture_cache[p_path] = texture;
      return texture;
#else
      return nullptr;
#endif
    }
    case RendererAPI::API::OpenGL_ES:
    {
#ifdef PLATFORM_ANDROID
      if (!reload)
      {
        if (auto cached = s_texture_cache[p_path].lock())
        {
          return cached;
        }
      }
      auto texture = std::make_shared<OpenGL_ESTexture2D>(p_path, filter_mode, engine_source);
      s_texture_cache[p_path] = texture;
      return texture;
#else
      return nullptr;
#endif
    }
    case RendererAPI::API::Metal:
      return nullptr;
  }
  AERO_CORE_ASSERT(false, "Unknown RendererAPI");
  return nullptr;
}
}  // namespace ag
