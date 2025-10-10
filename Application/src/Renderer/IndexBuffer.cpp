#include <Apch.hpp>
#include <Renderer/IndexBuffer.hpp>
#include <glad/glad.h>
#include <Renderer/Renderer.hpp>
#include <Platform/OpenGL/OpenGLIndexBuffer.hpp>

namespace aero
{
  IndexBuffer *IndexBuffer::create(unsigned int *indices, size_t size)
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
      return new OpenGLIndexBuffer(indices, size);
    }
    AERO_CORE_ASSERT(false, "Unknown RendererAPI");
    }
  }
}
