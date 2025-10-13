#include <Apch.hpp>
#include <Renderer/Texture.hpp>
#include <Renderer/Renderer.hpp>
#include <Platform/OpenGL/OpenGLTexture.hpp>


namespace aero
{
  std::shared_ptr<Texture2D> Texture2D::create(const std::string& p_path)
  {
    switch (Renderer::get_API())
    {
      case RendererAPI::API::None: AERO_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
      case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(p_path);
    }
    AERO_CORE_ASSERT(false, "Unknown RendererAPI");
    return nullptr;
  }
}
