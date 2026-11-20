#include <Aero.hpp>
#include <Scripting/ScriptBinding/ScriptBinding.hpp>
#include <Scripting/ScriptBinding/ScriptBindingHelpers.hpp>

namespace ag {

void ScriptBinding::register_keyboard_polling() {
  auto&      lua      = ScriptManager::get_lua();
  sol::table keyboard = lua.create_named_table("Keyboard");

  // Register key codes enum
  lua.new_enum<KeyCode>("Key",
                        {
                            // Printable keys
                            {       "Space",        Key::Space},
                            {  "Apostrophe",   Key::Apostrophe},
                            {       "Comma",        Key::Comma},
                            {       "Minus",        Key::Minus},
                            {      "Period",       Key::Period},
                            {       "Slash",        Key::Slash},

                            // Number keys
                            {          "D0",           Key::D0},
                            {          "D1",           Key::D1},
                            {          "D2",           Key::D2},
                            {          "D3",           Key::D3},
                            {          "D4",           Key::D4},
                            {          "D5",           Key::D5},
                            {          "D6",           Key::D6},
                            {          "D7",           Key::D7},
                            {          "D8",           Key::D8},
                            {          "D9",           Key::D9},

                            // Special characters
                            {   "Semicolon",    Key::Semicolon},
                            {       "Equal",        Key::Equal},

                            // Letter keys
                            {           "A",            Key::A},
                            {           "B",            Key::B},
                            {           "C",            Key::C},
                            {           "D",            Key::D},
                            {           "E",            Key::E},
                            {           "F",            Key::F},
                            {           "G",            Key::G},
                            {           "H",            Key::H},
                            {           "I",            Key::I},
                            {           "J",            Key::J},
                            {           "K",            Key::K},
                            {           "L",            Key::L},
                            {           "M",            Key::M},
                            {           "N",            Key::N},
                            {           "O",            Key::O},
                            {           "P",            Key::P},
                            {           "Q",            Key::Q},
                            {           "R",            Key::R},
                            {           "S",            Key::S},
                            {           "T",            Key::T},
                            {           "U",            Key::U},
                            {           "V",            Key::V},
                            {           "W",            Key::W},
                            {           "X",            Key::X},
                            {           "Y",            Key::Y},
                            {           "Z",            Key::Z},

                            // Brackets and special
                            { "LeftBracket",  Key::LeftBracket},
                            {   "Backslash",    Key::Backslash},
                            {"RightBracket", Key::RightBracket},
                            { "GraveAccent",  Key::GraveAccent},
                            {      "World1",       Key::World1},
                            {      "World2",       Key::World2},

                            // Function keys
                            {      "Escape",       Key::Escape},
                            {       "Enter",        Key::Enter},
                            {         "Tab",          Key::Tab},
                            {   "Backspace",    Key::Backspace},
                            {      "Insert",       Key::Insert},
                            {      "Delete",       Key::Delete},

                            // Arrow keys
                            {       "Right",        Key::Right},
                            {        "Left",         Key::Left},
                            {        "Down",         Key::Down},
                            {          "Up",           Key::Up},

                            // Navigation keys
                            {      "PageUp",       Key::PageUp},
                            {    "PageDown",     Key::PageDown},
                            {        "Home",         Key::Home},
                            {         "End",          Key::End},

                            // Lock keys
                            {    "CapsLock",     Key::CapsLock},
                            {  "ScrollLock",   Key::ScrollLock},
                            {     "NumLock",      Key::NumLock},
                            { "PrintScreen",  Key::PrintScreen},
                            {       "Pause",        Key::Pause},

                            // Function keys F1-F25
                            {          "F1",           Key::F1},
                            {          "F2",           Key::F2},
                            {          "F3",           Key::F3},
                            {          "F4",           Key::F4},
                            {          "F5",           Key::F5},
                            {          "F6",           Key::F6},
                            {          "F7",           Key::F7},
                            {          "F8",           Key::F8},
                            {          "F9",           Key::F9},
                            {         "F10",          Key::F10},
                            {         "F11",          Key::F11},
                            {         "F12",          Key::F12},
                            {         "F13",          Key::F13},
                            {         "F14",          Key::F14},
                            {         "F15",          Key::F15},
                            {         "F16",          Key::F16},
                            {         "F17",          Key::F17},
                            {         "F18",          Key::F18},
                            {         "F19",          Key::F19},
                            {         "F20",          Key::F20},
                            {         "F21",          Key::F21},
                            {         "F22",          Key::F22},
                            {         "F23",          Key::F23},
                            {         "F24",          Key::F24},
                            {         "F25",          Key::F25},

                            // Keypad
                            {         "KP0",          Key::KP0},
                            {         "KP1",          Key::KP1},
                            {         "KP2",          Key::KP2},
                            {         "KP3",          Key::KP3},
                            {         "KP4",          Key::KP4},
                            {         "KP5",          Key::KP5},
                            {         "KP6",          Key::KP6},
                            {         "KP7",          Key::KP7},
                            {         "KP8",          Key::KP8},
                            {         "KP9",          Key::KP9},

                            // Keypad operations
                            {   "KPDecimal",    Key::KPDecimal},
                            {    "KPDivide",     Key::KPDivide},
                            {  "KPMultiply",   Key::KPMultiply},
                            {  "KPSubtract",   Key::KPSubtract},
                            {       "KPAdd",        Key::KPAdd},
                            {     "KPEnter",      Key::KPEnter},
                            {     "KPEqual",      Key::KPEqual},

                            // Modifiers
                            {   "LeftShift",    Key::LeftShift},
                            { "LeftControl",  Key::LeftControl},
                            {     "LeftAlt",      Key::LeftAlt},
                            {   "LeftSuper",    Key::LeftSuper},
                            {  "RightShift",   Key::RightShift},
                            {"RightControl", Key::RightControl},
                            {    "RightAlt",     Key::RightAlt},
                            {  "RightSuper",   Key::RightSuper},
                            {        "Menu",         Key::Menu}
  });

  keyboard.set_function("is_key_down",
                        [](KeyCode key) -> bool { return Keyboard::is_key_pressed(key); });

  keyboard.set_function("is_key_up",
                        [](KeyCode key) -> bool { return Keyboard::is_key_released(key); });
}

void ScriptBinding::register_mouse_polling() {
  auto&      lua   = ScriptManager::get_lua();
  sol::table mouse = lua.create_named_table("Mouse");

  // Register mouse button enum
  lua.new_enum<MouseCode>("Button",
                          {
                              {  "Left", Button::Button0},
                              { "Right", Button::Button1},
                              {"Middle", Button::Button2}
  });

  // Mouse button state functions
  mouse.set_function("is_button_down",
                     [](MouseCode button) -> bool { return Mouse::is_mouse_pressed(button); });

  mouse.set_function("is_button_up",
                     [](MouseCode button) -> bool { return Mouse::is_mouse_released(button); });

  mouse.set_function(
      "get_mouse_position",
      sol::overload([]() -> vec2f { return Mouse::get_mouse_viewport_position(); },

                    [](Entity& entity) -> vec2f {
                      auto type = NodeHelper::get_nodetype(entity);

                      if (type == NodeType::Camera) {
                        const auto& props       = entity.get_component<Camera_Component>();
                        float_rect  window_rect = {props.center - props.size / 2, props.size};
                        vec2f       screen_pos  = Mouse::get_mouse_viewport_position();
                        vec2f       viewport_size = Mouse::get_viewport().framebuffer_size;
                        return Math::screen_to_world(screen_pos, window_rect, viewport_size);
                      }
                      return vec2f(0, 0);
                    }));

  // Raw cursor position in window space, for callers that need the untranslated
  // value (for example to drive OS-level UI).
  mouse.set_function("get_window_mouse_position",
                     []() -> vec2f { return Mouse::get_mouse_position(); });

  mouse.set_function("set_mouse_position",
                     [](const vec2f& position) {
                       Mouse::set_mouse_position(Mouse::viewport_to_window(position));
                     });
}

void ScriptBinding::register_events() {
  auto& lua = ScriptManager::get_lua();

  // Register event types enum
  lua.new_enum<Event_Type>("EventType",
                           {
                               {    "Key_Pressed",           Event_Type::Key_Pressed},
                               {   "Key_Released",          Event_Type::Key_Released},
                               { "Button_Pressed",  Event_Type::Mouse_Button_Pressed},
                               {"Button_Released", Event_Type::Mouse_Button_Released},
                               {    "Mouse_Moved",           Event_Type::Mouse_Moved},
                               { "Mouse_Scrolled",        Event_Type::Mouse_Scrolled}
  });

  // Register base Event type
  lua.new_usertype<Event>("Event", "get_type", &Event::get_event_type);

  // KEYBOARD EVENT FUNCTIONS

  lua.set_function("get_key", [&lua](Event& event) -> sol::object {
    auto type = event.get_event_type();

    switch (type) {
      case Event_Type::Key_Pressed:
        return sol::make_object(lua, static_cast<KeyPressedEvent&>(event).get_key_code());
      case Event_Type::Key_Released:
        return sol::make_object(lua, static_cast<KeyReleasedEvent&>(event).get_key_code());
      case Event_Type::Key_Typed:
        return sol::make_object(lua, static_cast<KeyTypedEvent&>(event).get_key_code());
      default: return sol::nil;
    }
  });

  lua.set_function("is_key_event", [](Event& event) -> bool {
    auto type = event.get_event_type();
    return type == Event_Type::Key_Pressed || type == Event_Type::Key_Released ||
           type == Event_Type::Key_Typed;
  });

  // MOUSE BUTTON EVENT FUNCTIONS

  lua.set_function("get_button", [&lua](Event& event) -> sol::object {
    auto type = event.get_event_type();

    switch (type) {
      case Event_Type::Mouse_Button_Pressed:
        return sol::make_object(lua,
                                static_cast<MouseButtonPressedEvent&>(event).get_mouse_button());
      case Event_Type::Mouse_Button_Released:
        return sol::make_object(lua,
                                static_cast<MouseButtonReleasedEvent&>(event).get_mouse_button());
      default: return sol::nil;
    }
  });

  lua.set_function("is_button_event", [](Event& event) -> bool {
    auto type = event.get_event_type();
    return type == Event_Type::Mouse_Button_Pressed || type == Event_Type::Mouse_Button_Released;
  });

  // MOUSE SCROLL EVENT FUNCTIONS

  lua.set_function("get_scroll", [&lua](Event& event) -> sol::object {
    if (event.get_event_type() == Event_Type::Mouse_Scrolled) {
      auto& e = static_cast<MouseScrolledEvent&>(event);
      return sol::make_object(lua, std::make_pair(e.get_offsetX(), e.get_offsetY()));
    }
    return sol::nil;
  });

  lua.set_function("get_scroll_x", [&lua](Event& event) -> sol::object {
    if (event.get_event_type() == Event_Type::Mouse_Scrolled) {
      return sol::make_object(lua, static_cast<MouseScrolledEvent&>(event).get_offsetX());
    }
    return sol::nil;
  });

  lua.set_function("get_scroll_y", [&lua](Event& event) -> sol::object {
    if (event.get_event_type() == Event_Type::Mouse_Scrolled) {
      return sol::make_object(lua, static_cast<MouseScrolledEvent&>(event).get_offsetY());
    }
    return sol::nil;
  });

  lua.set_function("is_scroll_event", [](Event& event) -> bool {
    return event.get_event_type() == Event_Type::Mouse_Scrolled;
  });
}

}  // namespace ag
