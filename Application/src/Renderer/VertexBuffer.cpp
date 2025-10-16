#include <Apch.hpp>
#include <Renderer/VertexBuffer.hpp>
#include <glad/glad.h>
#include <Renderer/Renderer.hpp>
#include <Platform/OpenGL/OpenGLVertexBuffer.hpp>

namespace ag
{
  AG_ref<VertexBuffer> VertexBuffer::create(float *vertices, size_t size)
  {
    switch (Renderer::get_API())
    {
      case RendererAPI::API::None: AERO_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
      case RendererAPI::API::OpenGL: return AG_cref<OpenGLVertexBuffer>(vertices, size);
    }
    AERO_CORE_ASSERT(false, "Unknown RendererAPI");
    return nullptr;
  }
}
