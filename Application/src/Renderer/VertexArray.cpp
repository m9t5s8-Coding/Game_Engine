#include <Apch.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/VertexArray.hpp>

#if defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS)
  #include <Platform/OpenGL/OpenGLVertexArray.hpp>
#elif defined(PLATFORM_ANDROID)
  #include <Platform/OpenGL_ES/OpenGL_ESVertexArray.hpp>
#endif

namespace ag
{
AG_ref<VertexArray> VertexArray::create()
{
  switch (Renderer::get_API())
  {
    case RendererAPI::API::None:
      AERO_CORE_ASSERT(false, "RendererAPI::None is not supported!");
      return nullptr;
    case RendererAPI::API::OpenGL:
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
      return AG_cref<OpenGLVertexArray>();
#else
      return nullptr;
#endif
    case RendererAPI::API::OpenGL_ES:
#ifdef PLATFORM_ANDROID
      return AG_cref<OpenGL_ESVertexArray>();
#else
      return nullptr;
#endif
    case RendererAPI::API::Metal:
      return nullptr;
  }
  AERO_CORE_ASSERT(false, "Unknown RendererAPI");
  return nullptr;
}
}  // namespace ag
