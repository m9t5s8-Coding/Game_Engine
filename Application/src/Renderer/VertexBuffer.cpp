#include <Apch.hpp>
#include <Renderer/VertexBuffer.hpp>
#include <glad/glad.h>
#include <Renderer/Renderer.hpp>
#include <Platform/OpenGL/OpenGLVertexBuffer.hpp>

namespace aero
{
  VertexBuffer *VertexBuffer::create(float *vertices, size_t size)
  {
    switch (Renderer::get_API())
    {
    case RendererAPI::None:
    {
      AERO_CORE_ASSERT(false, "RendererAPI::None is not supported!");
      return nullptr;
    }
    case RendererAPI::OpenGl:
    {
      return new OpenGLVertexBuffer(vertices, size);
    }
      AERO_CORE_ASSERT(false, "Unknown RendererAPI");
    }
  }
}
