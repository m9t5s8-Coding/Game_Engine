#include <Platform/Window/WindowsWindow.hpp>
#include <Log.hpp>
#include <iostream>

#include <Events/KeyEvent.hpp>
#include <Events/MouseEvent.hpp>
#include <Events/WindowEvent.hpp>

namespace aero
{
  static bool s_glfw_initialized = false;
  static bool s_glad_initialized = false;

  static void glfw_error_callback(int error, const char* description)
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

    m_Data.title = props.Title;
    m_Data.width = props.Width;
    m_Data.height = props.Height;

    AERO_CORE_INFO("Creating Window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    if (!s_glfw_initialized)
    {
      int success = glfwInit();
      // AERO_CORE_ASSERT(success, "Could not initialize GLFW!");

      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


      glfwSetErrorCallback(glfw_error_callback);

      s_glfw_initialized = true;
    }

    m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, &m_Data);

    if (!s_glad_initialized)
    {
      int success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

      s_glad_initialized = true;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);

    glViewport(0, 0, m_Data.width, m_Data.height);

    set_vsync(true);

    // Window size event
    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, int width, int height) {
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
      data.width = width;
      data.height = height;

      WindowResizeEvent event(width, height);
      data.event_callback(event);
    });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window) {
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

      WindowCloseEvent event;
      data.event_callback(event);
    });

    glfwSetKeyCallback(m_Window, [](GLFWwindow* window , int key, int scancode, int action, int mode) {
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

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
      }
    });

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mode){
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

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
      }
    });

    glfwSetScrollCallback(m_Window,[](GLFWwindow* window, double offset_X, double offset_Y){
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

      MouseScrolledEvent event((float)offset_X, (float)offset_Y);
      data.event_callback(event);
    });

    glfwSetCursorPosCallback(m_Window,[](GLFWwindow* window, double pos_x, double pos_y){
      WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

      MouseMovedEvent event((float)pos_x, (float)pos_y);
      data.event_callback(event);
    });


  }

  void WindowsWindow::on_update()
  {
    glfwPollEvents();
    glfwSwapBuffers(m_Window);
  }

  bool WindowsWindow::is_open() const
  {
    return m_Window && !glfwWindowShouldClose(m_Window);
  }

  void WindowsWindow::set_vsync(bool enabled)
  {
    if (enabled)
      glfwSwapInterval(1);
    else
      glfwSwapInterval(0);

    m_Data.vsync = enabled;
  }

  bool WindowsWindow::is_vsync() const
  {
    return m_Data.vsync;
  }

  void WindowsWindow::clear_window(const aero::Color &color)
  {
    float r, g, b, a;
    color.NormalizedColor(r, g, b, a);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void WindowsWindow::display_window()
  {
    glfwSwapBuffers(m_Window);
  }

  void WindowsWindow::process_events()
  {
    glfwPollEvents();
  }

  void WindowsWindow::close_window()
  {
    if (m_Window)
    {
      glfwDestroyWindow(m_Window);
    }
  }

}
