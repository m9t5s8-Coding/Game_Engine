#include "Scripting/ScriptManager.hpp"

#include <Scripting/ScriptBinding/RegisterFunction.hpp>
#include <Scripting/ScriptBinding/ScriptBinding.hpp>
#include <Scripting/ScriptBinding/SignalManager.hpp>

namespace ag
{
void RegisterFunction::client_init()
{
  // Value types first; Entity methods accept Color and vec2f by value.
  ScriptBinding::register_vec2();
  ScriptBinding::register_color();

  // Input and event helpers. On Android these include the Touch table and
  // platform/lifecycle state so scripts can react to mobile input directly.
  ScriptBinding::register_keyboard_polling();
  ScriptBinding::register_mouse_polling();
  ScriptBinding::register_events();
  ScriptBinding::register_platform();
  ScriptBinding::register_log();

  // Entity usertype must exist before component-specific bindings.
  ScriptBinding::register_node();
  ScriptBinding::register_audio_functions();
  ScriptBinding::register_physics();

  // Draw structs and helpers use the Color/vec2/RenderMode types registered above.
  ScriptBinding::register_draw();

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
