#include <Apch.hpp>
#include <Renderer/VertexArray.hpp>
#include <Renderer/Renderer.hpp>
#include <Platform/OpenGL/OpenGLVertexArray.hpp>

namespace aero
{
  VertexArray *VertexArray::create()
  {
    switch (Renderer::get_API())
    {
      case RendererAPI::API::None: AERO_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
      case RendererAPI::API::OpenGL: return new OpenGLVertexArray();
    }
    AERO_CORE_ASSERT(false, "Unknown RendererAPI");
    return nullptr;
  }
}
