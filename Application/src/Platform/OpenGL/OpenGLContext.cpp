#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX) || defined(PLATFORM_WEB)

#include <Platform/OpenGL/OpenGLContext.hpp>
#include <Apch.hpp>
#ifdef PLATFORM_WEB
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

namespace ag
{
 OpenGLContext::OpenGLContext(GLFWwindow *window_handle)
     : m_window_handle(window_handle)
 {
 }

 void OpenGLContext::init()
 {
  glfwMakeContextCurrent(m_window_handle);
#ifdef PLATFORM_WEB
  // WebGL 2 / OpenGL ES 3 functions are provided directly by Emscripten.
#else
  int glad_status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  AERO_CORE_ASSERT(glad_status, "Couldnot Initialize GLAD!");
#endif

  AERO_CORE_INFO("OpenGL Version: {}", reinterpret_cast<const char *>(glGetString(GL_VERSION)));
  AERO_CORE_INFO("Renderer: {}", reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
  AERO_CORE_INFO("Vendor: {}", reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
  AERO_CORE_INFO("GLSL Version: {}", reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
 }

 void OpenGLContext::swap_buffers()
 {
  glfwSwapBuffers(m_window_handle);
 }
}

#endif
