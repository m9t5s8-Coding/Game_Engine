#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX) || defined(PLATFORM_WEB)

#include <Core/Input.hpp>
#include <Apch.hpp>
#include <Core/Application.hpp>
#include <Renderer/View.hpp>

#include <GLFW/glfw3.h>

namespace ag
{
  bool Keyboard::is_key_pressed(const KeyCode key)
  {
    auto *window = static_cast<GLFWwindow *>(Application::get().get_window().get_native_window());
    auto state = glfwGetKey(window, static_cast<int32_t>(key));
    return state == GLFW_PRESS;
  }

  bool Keyboard::is_key_released(const KeyCode key)
  {
    auto *window = static_cast<GLFWwindow *>(Application::get().get_window().get_native_window());
    auto state = glfwGetKey(window, static_cast<int32_t>(key));
    return state == GLFW_RELEASE;
  }

  bool Mouse::is_mouse_pressed(const MouseCode button)
  {
    auto *window = static_cast<GLFWwindow *>(Application::get().get_window().get_native_window());
    auto state = glfwGetMouseButton(window,static_cast<int32_t>(button));
    return state == GLFW_PRESS;
  }

  bool Mouse::is_mouse_released(const MouseCode button)
  {
    auto *window = static_cast<GLFWwindow *>(Application::get().get_window().get_native_window());
    auto state = glfwGetMouseButton(window,static_cast<int32_t>(button));
    return state == GLFW_RELEASE;
  }

  void Mouse::set_mouse_position(const vec2f &position)
  {
    auto *window = static_cast<GLFWwindow *>(Application::get().get_window().get_native_window());

    glfwSetCursorPos(window, position.x, position.y);
  }

  vec2f Mouse::get_mouse_position()
  {
    auto *window = static_cast<GLFWwindow *>(Application::get().get_window().get_native_window());
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    return {static_cast<float>(xpos), static_cast<float>(ypos)};
  }

  vec2f Mouse::get_mouse_screen_position()
  {
    auto* window = static_cast<GLFWwindow*>(Application::get().get_window().get_native_window());

    vec2f mouse_pos = get_mouse_position();
    vec2i window_pos;
    glfwGetWindowPos(window, &window_pos.x, &window_pos.y);

    return window_pos + mouse_pos;
  }

}


#endif
