
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
  return false;
}

bool Keyboard::is_key_released(const KeyCode key)
{
  return false;
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

vec2f Mouse::get_mouse_world_position(const View& view)
{
  vec2f mouse_pos = get_mouse_position();
  return get_mouse_world_position(mouse_pos, view);
}

vec2f Mouse::get_mouse_world_position(const vec2f& mouse_position, const View& view)
{
  auto& window      = Application::get().get_window();
  vec2u window_size = window.get_size();
  return get_mouse_world_position(mouse_position, view, window_size);
}

vec2f Mouse::get_mouse_world_position(const vec2f& mouse_position,
                                      const View&  view,
                                      const vec2f& viewport_size)
{
  vec2f view_size   = view.get_size();
  vec2f view_center = view.get_center();

  vec2f ndc;
  ndc.x = (mouse_position.x / viewport_size.x) * 2.0f - 1.0f;
  ndc.y = (mouse_position.y / viewport_size.y) * 2.0f - 1.0f;

  vec2f world;
  world.x = ndc.x * (view_size.x / 2.0f) + view_center.x;
  world.y = ndc.y * (view_size.y / 2.0f) + view_center.y;
  return world;
}
}  // namespace ag

#endif
