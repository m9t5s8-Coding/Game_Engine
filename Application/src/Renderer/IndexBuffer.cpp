#include <Apch.hpp>
#include <Renderer/IndexBuffer.hpp>
#include <glad/glad.h>
#include <Renderer/Renderer.hpp>
#include <Platform/OpenGL/OpenGLIndexBuffer.hpp>

namespace ag
{
  AG_ref<IndexBuffer> IndexBuffer::create(unsigned int *indices, uint32_t count)
  {
    switch (Renderer::get_API())
    {
      case RendererAPI::API::None: AERO_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
      case RendererAPI::API::OpenGL: return AG_cref<OpenGLIndexBuffer>(indices, count);
    }
    AERO_CORE_ASSERT(false, "Unknown RendererAPI");
    return nullptr;
  }
}
