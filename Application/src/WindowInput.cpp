#include <Input.hpp>
#include <Apch.hpp>
#include <Application.hpp>

#include <glfw/glfw3.h>

namespace aero
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

  void Mouse::set_mouse_position(const vector_2f &position)
  {
    auto *window = static_cast<GLFWwindow *>(Application::get().get_window().get_native_window());

    glfwSetCursorPos(window, position.x, position.y);
  }

  vector_2f Mouse::get_mouse_position()
  {
    auto *window = static_cast<GLFWwindow *>(Application::get().get_window().get_native_window());
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    return vector_2f((float)xpos, (float)ypos);
  }
}
