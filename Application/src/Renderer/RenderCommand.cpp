#include <Renderer/RenderCommand.hpp>
#include <Apch.hpp>
#include <Platform/OpenGL/OpenGLRendererAPI.hpp>

namespace aero
{
  RendererAPI* RenderCommand::s_renderAPI = new OpenGLRendererAPI();
}
