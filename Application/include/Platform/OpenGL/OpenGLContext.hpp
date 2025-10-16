#pragma once
#include <Renderer/GraphicsContext.hpp>



struct GLFWwindow;

namespace ag
{
  class OpenGLContext : public GraphicsContext
  {
  public:
    OpenGLContext(GLFWwindow *window_handle);

    void init() override;
    void swap_buffers() override;

  private:
    GLFWwindow *m_window_handle;
  };
}
