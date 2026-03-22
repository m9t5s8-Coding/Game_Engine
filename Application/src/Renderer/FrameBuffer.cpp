#include <Apch.hpp>
#include <Renderer/FrameBuffer.hpp>
#include <Renderer/Renderer.hpp>

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
  #include <Platform/OpenGL/OpenGLFrameBuffer.hpp>
#elif defined(PLATFORM_ANDROID)
  #include <Platform/OpenGL_ES/OpenGL_ESFrameBuffer.hpp>
#endif

namespace ag
{
AG_ref<FrameBuffer> FrameBuffer::create(const FrameBufferSpecification& spec)
{
  switch (Renderer::get_API())
  {
    case RendererAPI::API::None:
      AERO_CORE_ASSERT(false, "RendererAPI::None is not supported!");
      return nullptr;
    case RendererAPI::API::OpenGL:
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
      return AG_cref<OpenGLFrameBuffer>(spec);
#else
      return nullptr;
#endif
    case RendererAPI::API::OpenGL_ES:
#ifdef PLATFORM_ANDROID
      return AG_cref<OpenGL_ESFrameBuffer>(spec);
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
