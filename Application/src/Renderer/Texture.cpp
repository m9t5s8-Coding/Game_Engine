#include <Apch.hpp>
#include <Platform/OpenGL/OpenGLTexture.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Texture.hpp>

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
    }
  }
  AERO_CORE_ASSERT(false, "Unknown RendererAPI");
  return nullptr;
}
}  // namespace ag
