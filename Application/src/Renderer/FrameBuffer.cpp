#include <Apch.hpp>
#include <Renderer/FrameBuffer.hpp>
#include <glad/glad.h>
#include <Renderer/Renderer.hpp>
#include <Platform/OpenGL/OpenGLFrameBuffer.hpp>

namespace ag
{
  AG_ref<FrameBuffer> FrameBuffer::create(const FrameBufferSpecification &spec)
  {
    switch (Renderer::get_API())
    {
      case RendererAPI::API::None: AERO_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
      case RendererAPI::API::OpenGL: return AG_cref<OpenGLFrameBuffer>(spec);
    }
    AERO_CORE_ASSERT(false, "Unknown RendererAPI");
    return nullptr;
  }
}
