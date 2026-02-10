#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)

#include <Platform/Window/WindowsWindow.hpp>
#include <iostream>

#include <Events/KeyEvent.hpp>
#include <Events/MouseEvent.hpp>
#include <Events/WindowEvent.hpp>
#include <Events/TextInput.hpp>

#include <Platform/OpenGL/OpenGLContext.hpp>

namespace ag
{
  static bool s_glfw_initialized = false;

  static void glfw_error_callback(int error, const char *description)
  {
    // AERO_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
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
    m_win_data.size = props.Size;

    AERO_CORE_INFO("Creating Window {0} ({1}, {2})", props.Title, props.Size.x, props.Size.y);

    if (!s_glfw_initialized)
    {
      int success = glfwInit();
      AERO_CORE_ASSERT(success, "Could not initialize GLFW!");

      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef PLATFORM_LINUX
      glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
#endif

      glfwSetErrorCallback(glfw_error_callback);
      s_glfw_initialized = true;
    }

    m_Window = glfwCreateWindow(static_cast<int>(props.Size.x), static_cast<int>(props.Size.y),
                                props.Title.c_str(), nullptr, nullptr);

    AERO_CORE_ASSERT(m_Window, "Failed to create GLFW window!");

    m_context = new OpenGLContext(m_Window);
    m_context->init();

    glfwSetWindowUserPointer(m_Window, &m_win_data);

    int fb_width, fb_height;
    glfwGetFramebufferSize(m_Window, &fb_width, &fb_height);

    if (fb_width > 0 && fb_height > 0)
    {
      glViewport(0, 0, fb_width, fb_height);
    }
    else
    {
      glViewport(0, 0, props.Size.x, props.Size.y);
    }

#ifdef PLATFORM_LINUX
    glfwShowWindow(m_Window);
#endif

    set_vsync(true);

    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, const int width, const int height)
    {
      WindowData& data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
      data.size.x = width;
      data.size.y = height;

      WindowResizeEvent event(data.size);
      data.event_callback(event);
    });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window)
    {
      WindowData& data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

      WindowCloseEvent event;
      data.event_callback(event);
    });

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
      }
    });

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
      }
    });

    glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double offset_X, double offset_Y)
    {
      WindowData& data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

      MouseScrolledEvent event(static_cast<float>(offset_X), static_cast<float>(offset_Y));
      data.event_callback(event);
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double pos_x, double pos_y)
    {
      WindowData& data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));

      MouseMovedEvent event(static_cast<float>(pos_x), static_cast<float>(pos_y));
      data.event_callback(event);
    });

    glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int codepoint)
    {
      WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

      TextInputEvent event(static_cast<char>(codepoint));
      data.event_callback(event);
    });
  }

  void WindowsWindow::set_vsync(bool enabled)
  {
    if (enabled)
      glfwSwapInterval(1);
    else
      glfwSwapInterval(0);

    m_win_data.vsync = enabled;
  }

  void WindowsWindow::set_full_screen()
  {
    GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *p_mode = glfwGetVideoMode(primary_monitor);

    if (!m_win_data.is_fullscreen)
    {
#ifndef PLATFORM_LINUX
      glfwGetWindowPos(m_Window, &m_win_data.old_pos.x, &m_win_data.old_pos.y);
#endif
      glfwGetWindowSize(m_Window, &m_win_data.old_size.x, &m_win_data.old_size.y);

      glfwSetWindowAttrib(m_Window, GLFW_DECORATED, GLFW_FALSE);
      glfwSetWindowMonitor(m_Window, nullptr, 0, 0, p_mode->width, p_mode->height, 0);

      m_win_data.is_fullscreen = true;
    }
    else
    {
      glfwSetWindowAttrib(m_Window, GLFW_DECORATED, GLFW_TRUE);
#ifdef PLATFORM_LINUX
      glfwSetWindowMonitor(m_Window, nullptr, 0, 0,
                          m_win_data.old_size.x, m_win_data.old_size.y, 0);
#else
      glfwSetWindowMonitor(m_Window, nullptr, m_win_data.old_pos.x, m_win_data.old_pos.y,
                          m_win_data.old_size.x, m_win_data.old_size.y, 0);
#endif
      m_win_data.is_fullscreen = false;
    }
  }

  void WindowsWindow::set_size(const vec2u& size)
  {
    m_win_data.size = size;
    glfwSetWindowSize(m_Window, size.x, size.y);
  }

  void WindowsWindow::set_position(const vec2i& position)
  {
#ifdef PLATFORM_LINUX
    return;
#else
    glfwSetWindowPos(m_Window, position.x, position.y);
#endif
  }

  vec2u WindowsWindow::get_position() const
  {
    vec2i window_pos = {0, 0};

#ifdef PLATFORM_LINUX
    return window_pos;
#else
    glfwGetWindowPos(m_Window, &window_pos.x, &window_pos.y);
    return window_pos;
#endif
  }

  void WindowsWindow::center_window()
  {
#ifdef PLATFORM_LINUX
    m_win_data.is_center_window = false;
    return;
#else
    GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* p_mode = glfwGetVideoMode(primary_monitor);

    vec2i monitor_size = { p_mode->width, p_mode->height };
    vec2i position = monitor_size / 2 - m_win_data.size / 2;

    set_position(position);
    m_win_data.is_center_window = true;
#endif
  }

  void WindowsWindow::show_decoration(const bool show)
  {
    glfwSetWindowAttrib(m_Window, GLFW_DECORATED, show);
  }

  bool WindowsWindow::is_vsync() const
  {
    return m_win_data.vsync;
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
      m_Window = nullptr;
    }
  }

  std::string WindowsWindow::get_clipboard_string() const
  {
    const char* clipboard_text = glfwGetClipboardString(m_Window);
    if (clipboard_text)
    {
      return std::string(clipboard_text);
    }
    return std::string("");
  }
}

#endif
