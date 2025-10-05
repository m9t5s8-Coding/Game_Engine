#pragma once

#include <Vector2.hpp>

namespace aero
{
  enum class Button
  {
    Left = 0,
    Right,
    Middle,
    Button1,
    Button2,
    Button3,
    Button4,
    Button5,
    Button6,
    Button7,
    Button8,
    Unknown = -1
  };
  class RenderWindow;
  class Mouse
  {
  public:
    static bool is_button_pressed(Button button);
    static bool is_button_released(Button button);
    static void set_mouse_position(const aero::vector_2i &position);
    static void set_mouse_position(const aero::vector_2i &position, const RenderWindow &window);
    static aero::vector_2f get_mouse_position();
    static aero::vector_2f get_mouse_position(const RenderWindow &window);
  };
}
