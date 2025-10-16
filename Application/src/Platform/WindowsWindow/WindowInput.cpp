#include <Core/Input.hpp>
#include <Apch.hpp>
#include <Core/Application.hpp>
#include <Renderer/View.hpp>

#include <glfw/glfw3.h>

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

  vec2f Mouse::get_mouse_world_position(const View& view)
  {
    vec2f mouse_position = get_mouse_position();
    return get_mouse_world_position(mouse_position, view);
  }

  vec2f Mouse::get_mouse_world_position(const vec2f& mouse_position, const View& view)
  {
    auto &window = Application::get().get_window();
    vec2u window_size = window.get_size();
    vec2f view_size = view.get_size();
    vec2f view_center = view.get_center();

    vec2f ndc;
    ndc.x = (mouse_position.x / window_size.x) * 2.0f - 1.0f;
    ndc.y = (mouse_position.y / window_size.y) * 2.0f - 1.0f;

    vec2f world;
    world.x = ndc.x * (view_size.x / 2.0f) + view_center.x;
    world.y = ndc.y * (view_size.y / 2.0f) + view_center.y;

    return world;
  }
}
