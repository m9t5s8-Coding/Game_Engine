#pragma once

#include <Events/KeyCodes.hpp>
#include <Events/MouseCodes.hpp>
#include <Math/Vector2.hpp>

namespace aero
{

  class Keyboard
  {
  public:
    static bool is_key_pressed(const KeyCode key);
    static bool is_key_released(const KeyCode key);
  };

  class Mouse
  {
  public:
    static bool is_mouse_pressed(const MouseCode button);
    static bool is_mouse_released(const MouseCode button);

    static void set_mouse_position(const aero::vector_2f& position);
    static aero::vector_2f get_mouse_position();

  };

}
