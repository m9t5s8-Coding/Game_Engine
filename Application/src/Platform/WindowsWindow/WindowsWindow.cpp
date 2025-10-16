#include <Platform/Window/WindowsWindow.hpp>
#include <iostream>

#include <Events/KeyEvent.hpp>
#include <Events/MouseEvent.hpp>
#include <Events/WindowEvent.hpp>

#include <Platform/OpenGL/OpenGLContext.hpp>

namespace ag
{
  static bool s_glfw_initialized = false;

  static void glfw_error_callback(int error, const char *description)
  {
    AERO_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
  }

  Window *Window::create(const WindowProps &props)
  {
    return new WindowsWindow(props);
  }

  WindowsWindow::WindowsWindow(const WindowProps &props)
  {
    init(props);
  }

  WindowsWindow::~WindowsWindow()
  {
    close_window();
  }

  void WindowsWindow::init(const WindowProps &props)
  {

    m_win_data.title = props.Title;
    m_win_data.size= props.Size;

    AERO_CORE_INFO("Creating Window {0} ({1}, {2})", props.Title, props.Size.x, props.Size.y);

    if (!s_glfw_initialized)
    {
      int success = glfwInit();
      AERO_CORE_ASSERT(success, "Could not initialize GLFW!");

      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      glfwSetErrorCallback(glfw_error_callback);
      s_glfw_initialized = true;
    }

    m_Window = glfwCreateWindow(static_cast<int>(props.Size.x), static_cast<int>(props.Size.y), props.Title.c_str(), nullptr, nullptr);

    m_context = new OpenGLContext(m_Window);
    m_context->init();

    glfwSetWindowUserPointer(m_Window, &m_win_data);
    glViewport(0, 0, props.Size.x, props.Size.y);

    set_vsync(true);

    // Window size event
    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, const int width, const int height)
    {
      WindowData& data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
      data.size.x = width;
      data.size.y = height;

      WindowResizeEvent event(data.size);
      data.event_callback(event); });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window)
    {
      WindowData& data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

      WindowCloseEvent event;
      data.event_callback(event); });

    glfwSetKeyCallback(m_Window, [](GLFWwindow *window, const int key, const int scancode, const int action, const int mode)
    {
      WindowData& data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

      switch(action)
      {
        case GLFW_PRESS:
        {
          KeyPressedEvent event(key, 0);
          data.event_callback(event);
          break;
        }
        case GLFW_RELEASE:
        {
          KeyReleasedEvent event(key);
          data.event_callback(event);
          break;
        }
        case GLFW_REPEAT:
        {
          KeyPressedEvent event(key, 1);
          data.event_callback(event);
          break;
        }
          default:
            break;
      } });

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mode)
    {
      WindowData& data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

      switch(action)
      {
        case GLFW_PRESS:
        {
          MouseButtonPressedEvent event(button);
          data.event_callback(event);
          break;
        }
        case GLFW_RELEASE:
        {
          MouseButtonReleasedEvent event(button);
          data.event_callback(event);
          break;
        }
          default:
            break;
      } });

    glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double offset_X, double offset_Y)
    {
      WindowData& data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

      MouseScrolledEvent event(static_cast<float>(offset_X), static_cast<float>(offset_Y));
      data.event_callback(event); });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double pos_x, double pos_y)
    {
      WindowData& data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

      MouseMovedEvent event(static_cast<float>(pos_x), static_cast<float>(pos_y));
      data.event_callback(event); });
  }

  void WindowsWindow::set_vsync(bool enabled)
  {
    if (enabled)
      glfwSwapInterval(1);
    else
      glfwSwapInterval(0);

      m_win_data.vsync = enabled;
  }

  bool WindowsWindow::is_vsync() const
  {
    return m_win_data.vsync;
  }

  void WindowsWindow::clear_window(const ag::Color &color)
  {
    float r, g, b, a;
    color.NormalizedColor(r, g, b, a);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void WindowsWindow::on_update()
  {
    glfwPollEvents();
    m_context->swap_buffers();
  }

  void WindowsWindow::close_window()
  {
    if (m_Window)
    {
      glfwDestroyWindow(m_Window);
    }
  }

}
