#include <Apch.hpp>
#include <Renderer/VertexArray.hpp>
#include <Renderer/Renderer.hpp>
#include <Platform/OpenGL/OpenGLVertexArray.hpp>

namespace ag
{
  AG_ref<VertexArray> VertexArray::create()
  {
    switch (Renderer::get_API())
    {
      case RendererAPI::API::None: AERO_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
      case RendererAPI::API::OpenGL: return AG_cref<OpenGLVertexArray>();
    }
    AERO_CORE_ASSERT(false, "Unknown RendererAPI");
    return nullptr;
  }
}
