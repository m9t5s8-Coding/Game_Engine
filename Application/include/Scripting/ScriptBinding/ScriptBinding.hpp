#pragma once


namespace ag
{
  class ScriptBinding
  {
  public:

    // Mouse and Keyboard Register
    static void register_mouse_polling();
    static void register_keyboard_polling();

    static void register_events();

    // Register Colors
    static void register_color();

    // Register vec2f
    static void register_vec2();

    static void register_node();
  };
}
