#include <Apch.hpp>
#include <Renderer/Texture.hpp>
#include <Renderer/Renderer.hpp>
#include <Platform/OpenGL/OpenGLTexture.hpp>


namespace ag
{
  std::shared_ptr<Texture2D> Texture2D::create(const std::string& p_path)
  {
    static std::unordered_map<std::string, std::weak_ptr<Texture2D>> s_texture_cache;
    switch (Renderer::get_API())
    {
      case RendererAPI::API::None: AERO_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
      case RendererAPI::API::OpenGL:
      {
        if (auto cached = s_texture_cache[p_path].lock())
        {
          return cached;
        }
        auto texture = std::make_shared<OpenGLTexture2D>(p_path);
        s_texture_cache[p_path] = texture;
        return texture;
      }
    }
    AERO_CORE_ASSERT(false, "Unknown RendererAPI");
    return nullptr;
  }
}
