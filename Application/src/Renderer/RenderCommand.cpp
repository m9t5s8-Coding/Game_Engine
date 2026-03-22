#include <Apch.hpp>
#include <Renderer/RenderCommand.hpp>

#if defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS)
  #include <Platform/OpenGL/OpenGLRendererAPI.hpp>
#elif defined(PLATFORM_ANDROID)
  #include <Platform/OpenGL_ES/OpenGL_ESRendererAPI.hpp>
#endif
namespace ag
{
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
RendererAPI* RenderCommand::s_renderAPI = new OpenGLRendererAPI();
#elif defined(PLATFORM_ANDROID)
RendererAPI* RenderCommand::s_renderAPI = new OpenGL_ESRendererAPI();
#endif
}  // namespace ag
