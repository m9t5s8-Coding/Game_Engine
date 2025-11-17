#include <Scripting/ScriptBinding/ScriptBinding.hpp>
#include <Aero.hpp>


namespace ag
{
  void ScriptBinding::register_keyboard_polling()
  {
    auto& lua = ScriptManager::get_lua();

    sol::table keyboard = lua.create_named_table("Keyboard");

    lua.new_enum<ag::KeyCode>("Key",
      {
          {"Space", ag::Key::Space},
          {"Apostrophe", ag::Key::Apostrophe},
          {"Comma", ag::Key::Comma},
          {"Minus", ag::Key::Minus},
          {"Period", ag::Key::Period},
          {"Slash", ag::Key::Slash},

          {"D0", ag::Key::D0}, {"D1", ag::Key::D1}, {"D2", ag::Key::D2}, {"D3", ag::Key::D3},
          {"D4", ag::Key::D4}, {"D5", ag::Key::D5}, {"D6", ag::Key::D6}, {"D7", ag::Key::D7},
          {"D8", ag::Key::D8}, {"D9", ag::Key::D9},

          {"Semicolon", ag::Key::Semicolon},
          {"Equal", ag::Key::Equal},

          {"A", ag::Key::A}, {"B", ag::Key::B}, {"C", ag::Key::C}, {"D", ag::Key::D},
          {"E", ag::Key::E}, {"F", ag::Key::F}, {"G", ag::Key::G}, {"H", ag::Key::H},
          {"I", ag::Key::I}, {"J", ag::Key::J}, {"K", ag::Key::K}, {"L", ag::Key::L},
          {"M", ag::Key::M}, {"N", ag::Key::N}, {"O", ag::Key::O}, {"P", ag::Key::P},
          {"Q", ag::Key::Q}, {"R", ag::Key::R}, {"S", ag::Key::S}, {"T", ag::Key::T},
          {"U", ag::Key::U}, {"V", ag::Key::V}, {"W", ag::Key::W}, {"X", ag::Key::X},
          {"Y", ag::Key::Y}, {"Z", ag::Key::Z},

          {"LeftBracket", ag::Key::LeftBracket},
          {"Backslash", ag::Key::Backslash},
          {"RightBracket", ag::Key::RightBracket},
          {"GraveAccent", ag::Key::GraveAccent},

          {"World1", ag::Key::World1},
          {"World2", ag::Key::World2},

          // Function keys
          {"Escape", ag::Key::Escape}, {"Enter", ag::Key::Enter}, {"Tab", ag::Key::Tab},
          {"Backspace", ag::Key::Backspace}, {"Insert", ag::Key::Insert}, {"Delete", ag::Key::Delete},

          {"Right", ag::Key::Right}, {"Left", ag::Key::Left}, {"Down", ag::Key::Down}, {"Up", ag::Key::Up},

          {"PageUp", ag::Key::PageUp}, {"PageDown", ag::Key::PageDown}, {"Home", ag::Key::Home}, {"End", ag::Key::End},

          {"CapsLock", ag::Key::CapsLock}, {"ScrollLock", ag::Key::ScrollLock}, {"NumLock", ag::Key::NumLock},
          {"PrintScreen", ag::Key::PrintScreen}, {"Pause", ag::Key::Pause},

          {"F1", ag::Key::F1}, {"F2", ag::Key::F2}, {"F3", ag::Key::F3}, {"F4", ag::Key::F4},
          {"F5", ag::Key::F5}, {"F6", ag::Key::F6}, {"F7", ag::Key::F7}, {"F8", ag::Key::F8},
          {"F9", ag::Key::F9}, {"F10", ag::Key::F10}, {"F11", ag::Key::F11}, {"F12", ag::Key::F12},
          {"F13", ag::Key::F13}, {"F14", ag::Key::F14}, {"F15", ag::Key::F15}, {"F16", ag::Key::F16},
          {"F17", ag::Key::F17}, {"F18", ag::Key::F18}, {"F19", ag::Key::F19}, {"F20", ag::Key::F20},
          {"F21", ag::Key::F21}, {"F22", ag::Key::F22}, {"F23", ag::Key::F23}, {"F24", ag::Key::F24},
          {"F25", ag::Key::F25},

          // Keypad
          {"KP0", ag::Key::KP0}, {"KP1", ag::Key::KP1}, {"KP2", ag::Key::KP2}, {"KP3", ag::Key::KP3},
          {"KP4", ag::Key::KP4}, {"KP5", ag::Key::KP5}, {"KP6", ag::Key::KP6}, {"KP7", ag::Key::KP7},
          {"KP8", ag::Key::KP8}, {"KP9", ag::Key::KP9},

          {"KPDecimal", ag::Key::KPDecimal}, {"KPDivide", ag::Key::KPDivide}, {"KPMultiply", ag::Key::KPMultiply},
          {"KPSubtract", ag::Key::KPSubtract}, {"KPAdd", ag::Key::KPAdd}, {"KPEnter", ag::Key::KPEnter},
          {"KPEqual", ag::Key::KPEqual},

          // Modifiers
          {"LeftShift", ag::Key::LeftShift}, {"LeftControl", ag::Key::LeftControl}, {"LeftAlt", ag::Key::LeftAlt}, {"LeftSuper", ag::Key::LeftSuper},
          {"RightShift", ag::Key::RightShift}, {"RightControl", ag::Key::RightControl}, {"RightAlt", ag::Key::RightAlt}, {"RightSuper", ag::Key::RightSuper},
          {"Menu", ag::Key::Menu}
      });

    keyboard.set_function("is_key_pressed", [](ag::KeyCode key) -> bool {
      return ag::Keyboard::is_key_pressed(key);
      });

    keyboard.set_function("is_key_released", [](ag::KeyCode key) -> bool {
      return ag::Keyboard::is_key_released(key);
      });
  }

  void ScriptBinding::register_mouse_polling()
  {
    auto& lua = ScriptManager::get_lua();

    sol::table mouse = lua.create_named_table("Mouse");

    lua.new_enum<ag::KeyCode>("Button",
      {
          {"Left", ag::Button::ButtonLeft},
          {"Right", ag::Button::ButtonRight},
          {"Middle", ag::Button::ButtonRight}
      });

    mouse.set_function("is_mouse_pressed", [](ag::MouseCode button) -> bool {
      return ag::Mouse::is_mouse_pressed(button);
      });

    mouse.set_function("is_mouse_released", [](ag::MouseCode button) -> bool {
      return ag::Mouse::is_mouse_released(button);
      });

    mouse.set_function("get_mouse_position", []() -> ag::vec2f {
      return ag::Mouse::get_mouse_position();
      });

    mouse.set_function("set_mouse_position", [](ag::vec2f position) {
      ag::Mouse::set_mouse_position(position);
      });

  }

  void ScriptBinding::register_color()
  {
    auto& lua = ScriptManager::get_lua();


    
  }

  void ScriptBinding::register_vec2()
  {
    auto& lua = ScriptManager::get_lua();

    lua.new_usertype<ag::vec2f>("vec2f",
      sol::constructors<ag::vec2f(), ag::vec2f(float, float)>(),
      "x", &ag::vec2f::x,
      "y", &ag::vec2f::y,
      "__add", sol::resolve<ag::vec2f(const ag::vec2f&) const>(&ag::vec2f::operator+),
      "__sub", sol::resolve<ag::vec2f(const ag::vec2f&) const>(&ag::vec2f::operator-),
      "__mul", sol::resolve<ag::vec2f(const ag::vec2f&) const>(&ag::vec2f::operator*),
      "__div", sol::resolve<ag::vec2f(const ag::vec2f&) const>(&ag::vec2f::operator/),
      "__eq", sol::resolve<bool(const ag::vec2f&) const>(&ag::vec2f::operator==),
      "__tostring", [](const ag::vec2f& v) { return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")"; }
    );

    lua.new_usertype<ag::vec2i>("vec2i",
      sol::constructors<ag::vec2i(), ag::vec2i(int, int)>(),
      "x", &ag::vec2i::x,
      "y", &ag::vec2i::y,
      "__add", sol::resolve<ag::vec2i(const ag::vec2i&) const>(&ag::vec2i::operator+),
      "__sub", sol::resolve<ag::vec2i(const ag::vec2i&) const>(&ag::vec2i::operator-),
      "__mul", sol::resolve<ag::vec2i(const ag::vec2i&) const>(&ag::vec2i::operator*),
      "__div", sol::resolve<ag::vec2i(const ag::vec2i&) const>(&ag::vec2i::operator/),
      "__eq", sol::resolve<bool(const ag::vec2i&) const>(&ag::vec2i::operator==),
      "__tostring", [](const ag::vec2i& v) { return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")"; }
    );
  }


}