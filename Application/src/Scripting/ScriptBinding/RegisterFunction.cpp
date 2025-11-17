#include <Scripting/ScriptBinding/RegisterFunction.hpp>
#include <Scripting/ScriptBinding/ScriptBinding.hpp>

namespace ag
{
  void RegisterFunction::init()
  {
    ScriptBinding::register_keyboard_polling();
    ScriptBinding::register_mouse_polling();

    ScriptBinding::register_vec2();
  }
}
