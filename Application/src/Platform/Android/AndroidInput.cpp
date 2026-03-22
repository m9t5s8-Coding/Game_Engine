
#ifdef PLATFORM_ANDROID

  #include <Apch.hpp>
  #include <Core/Application.hpp>
  #include <Core/Input.hpp>
  #include <Renderer/View.hpp>

namespace ag
{
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
  return false;
}

bool Mouse::is_mouse_released(const MouseCode button)
{
  return false;
}

void Mouse::set_mouse_position(const vec2f& position)
{
}

vec2f Mouse::get_mouse_position()
{
  return {0, 0};
}

vec2f Mouse::get_mouse_screen_position()
{
  return {0, 0};
}

vec2f Mouse::get_mouse_world_position(const View& view)
{
  return {0, 0};
}

vec2f Mouse::get_mouse_world_position(const vec2f& mouse_position, const View& view)
{
  return {0, 0};
}

vec2f Mouse::get_mouse_world_position(const vec2f& mouse_position,
                                      const View&  view,
                                      const vec2f& viewport_size)
{
  return {0, 0};
}
}  // namespace ag

#endif
