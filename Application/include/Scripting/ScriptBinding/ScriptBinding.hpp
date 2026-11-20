#pragma once

namespace ag
{
// Registration is split by domain into the .cpp files next to the public
// facade. RegisterFunction remains the single place that controls ordering.
class ScriptBinding
{
public:
  // Input / events (InputBindings.cpp)
  static void register_mouse_polling();
  static void register_keyboard_polling();
  static void register_events();

  // Value types (MathBindings.cpp)
  static void register_color();
  static void register_vec2();

  // Entity API (EntityBindings.cpp)
  static void register_node();

  // Component helpers (AudioBindings.cpp, PhysicsBindings.cpp)
  static void register_audio_functions();
  static void register_physics();

  // Drawing API (DrawBindings.cpp)
  static void register_draw();

  // World and networking (SceneBindings.cpp, NetworkBindings.cpp, LogBindings.cpp)
  static void register_network();
  static void register_log();
  static void register_scene();

  // Platform and touch (PlatformBindings.cpp)
  static void register_platform();
};
}  // namespace ag
