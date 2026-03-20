#include "Scripting/ScriptManager.hpp"

#include <Scripting/ScriptBinding/RegisterFunction.hpp>
#include <Scripting/ScriptBinding/ScriptBinding.hpp>
#include <Scripting/ScriptBinding/SignalManager.hpp>

namespace ag
{
void RegisterFunction::client_init()
{
  ScriptBinding::register_vec2();
  ScriptBinding::register_color();

  ScriptBinding::register_keyboard_polling();
  ScriptBinding::register_mouse_polling();
  ScriptBinding::register_events();

  ScriptBinding::register_node();
  ScriptBinding::register_audio_functions();
  ScriptBinding::register_physics();

  ScriptBinding::register_scene();

  ScriptBinding::register_network();
  Signal_Manager::bind_signals();
}

void RegisterFunction::server_init()
{
  ScriptBinding::register_vec2();
  ScriptBinding::register_color();
  ScriptBinding::register_network();
  ScriptBinding::register_log();
}
}  // namespace ag
