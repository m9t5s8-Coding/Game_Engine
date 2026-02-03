#include <Scripting/ScriptBinding/RegisterFunction.hpp>
#include <Scripting/ScriptBinding/ScriptBinding.hpp>

namespace ag
{
  void RegisterFunction::init()
  {
    ScriptBinding::register_vec2();
    ScriptBinding::register_color();

    ScriptBinding::register_keyboard_polling();
    ScriptBinding::register_mouse_polling();
    ScriptBinding::register_events();

    

    ScriptBinding::register_node();
    ScriptBinding::register_audio_functions();
    ScriptBinding::register_physics();
  }
}
