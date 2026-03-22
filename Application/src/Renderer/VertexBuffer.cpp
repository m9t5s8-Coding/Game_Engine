
#include <Apch.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/VertexBuffer.hpp>

#if defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS)
  #include <Platform/OpenGL/OpenGLVertexBuffer.hpp>
#elif defined(PLATFORM_ANDROID)
  #include <Platform/OpenGL_ES/OpenGL_ESVertexBuffer.hpp>
#endif
namespace ag
{
AG_ref<VertexBuffer> VertexBuffer::create(float* vertices, size_t size)
{
  switch (Renderer::get_API())
  {
    case RendererAPI::API::None:
      AERO_CORE_ASSERT(false, "RendererAPI::None is not supported!");
      return nullptr;
    case RendererAPI::API::OpenGL:
#if defined(PLATFORM_LINUX) || defined(PLATFORM_WIDNOW)
      return AG_cref<OpenGLVertexBuffer>(vertices, size);
#else
      return nullptr;
#endif
    case RendererAPI::API::OpenGL_ES:
#ifdef PLATFORM_ANDROID
      return std::make_shared<OpenGL_ESVertexBuffer>(vertices, size);
#else
      return nullptr;
#endif
    case RendererAPI::API::Metal:
      return nullptr;
  }
  AERO_CORE_ASSERT(false, "Unknown RendererAPI");
  return nullptr;
}

AG_ref<VertexBuffer> VertexBuffer::create(size_t size)  // dynamic, no initial data
{
  switch (Renderer::get_API())
  {
    case RendererAPI::API::None:
      AERO_CORE_ASSERT(false, "RendererAPI::None is not supported!");
      return nullptr;
    case RendererAPI::API::OpenGL:
#if defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS)
      return AG_cref<OpenGLVertexBuffer>(size);  // make sure desktop also has this constructor
#else
      return nullptr;
#endif
    case RendererAPI::API::OpenGL_ES:
#ifdef PLATFORM_ANDROID
      return std::make_shared<OpenGL_ESVertexBuffer>(size);  // routes to GL_DYNAMIC_DRAW ctor
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
