#include <Renderer/RenderCommand.hpp>
#include <Apch.hpp>
#include <Platform/OpenGL/OpenGLRendererAPI.hpp>

namespace ag
{
  RendererAPI* RenderCommand::s_renderAPI = new OpenGLRendererAPI();
}
