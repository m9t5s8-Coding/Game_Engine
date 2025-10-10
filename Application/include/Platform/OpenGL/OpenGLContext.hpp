#pragma once
#include <Renderer/GraphicsContext.hpp>

struct GLFWwindow;

namespace aero
{
  class OpenGLContext : public GraphicsContext
  {
  public:
    OpenGLContext(GLFWwindow *window_handle);

    virtual void init() override;
    virtual void swap_buffers() override;

  private:
    GLFWwindow *m_window_handle;
  };
}
