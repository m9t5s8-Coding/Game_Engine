#include <Renderer/RendererAPI.hpp>

namespace ag
{
#ifdef PLATFORM_WINDOWS
  RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

#elif defined(PLATFORM_ANDROID)
  RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL_ES;

#elif defined(PLATFORM_MACOS)
  RendererAPI::API RendererAPI::s_API = RendererAPI::API::Metal;

#elif defined(PLATFORM_LINUX)
  RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
#endif
}
