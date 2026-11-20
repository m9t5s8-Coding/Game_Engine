
#ifdef PLATFORM_ANDROID

  #include <Apch.hpp>
  #include <Core/Application.hpp>
  #include <Core/Input.hpp>
  #include <Platform/Android/AndroidWindow.hpp>
  #include <Renderer/View.hpp>

namespace ag
{
static AndroidWindow& get_android_window()
{
  return static_cast<AndroidWindow&>(Application::get().get_window());
}
bool Keyboard::is_key_pressed(const KeyCode key)
{
  return get_android_window().is_key_down(key);
}

bool Keyboard::is_key_released(const KeyCode key)
{
  return !get_android_window().is_key_down(key);
}

bool Mouse::is_mouse_pressed(const MouseCode button)
{
  return get_android_window().is_touch_active();
}

bool Mouse::is_mouse_released(const MouseCode button)
{
  return !get_android_window().is_touch_active();
}

void Mouse::set_mouse_position(const vec2f& position)
{
}

vec2f Mouse::get_mouse_position()
{
  return get_android_window().get_touch_position();
}

vec2f Mouse::get_mouse_screen_position()
{
  return get_mouse_position();
}
}  // namespace ag

#endif
