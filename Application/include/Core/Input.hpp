#pragma once

#include <Events/KeyCodes.hpp>
#include <Events/MouseCodes.hpp>
#include <Math/vec2.hpp>

namespace ag
{

  class Keyboard
  {
  public:
    static bool is_key_pressed(const KeyCode key);
    static bool is_key_released(const KeyCode key);
  };

  class View;
  class Mouse
  {
  public:
    static bool is_mouse_pressed(const MouseCode button);
    static bool is_mouse_released(const MouseCode button);

    static void set_mouse_position(const vec2f& position);
    static vec2f get_mouse_position();
    static vec2f get_mouse_screen_position();
    static vec2f get_mouse_world_position(const View& view);
    static vec2f get_mouse_world_position(const vec2f& mouse_position, const View& view);
    static vec2f get_mouse_world_position(const vec2f& mouse_position, const View& view, const vec2f& viewport_size);

  };

}
