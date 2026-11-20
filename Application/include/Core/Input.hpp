#pragma once

#include <Events/KeyCodes.hpp>
#include <Events/MouseCodes.hpp>
#include <Math/vec2.hpp>

namespace ag {

class Keyboard {
public:
  static bool is_key_pressed(const KeyCode key);
  static bool is_key_released(const KeyCode key);
};

class View;
class Mouse {
public:
  // Describes where the rendered scene lives inside the window. The scene is
  // rendered to its own framebuffer and then composited into a (possibly
  // letterboxed) rectangle inside the window, so window cursor coordinates are
  // not the same as framebuffer pixels once the aspect ratios differ.
  struct Viewport {
    // Window size in cursor coordinates (GLFW/window space).
    vec2f window_size{0.0f, 0.0f};
    // Size of the composited scene rectangle in window coordinates. This is
    // usually the aspect-fit rect, not the full window.
    vec2f content_size{0.0f, 0.0f};
    // Size, in pixels, of the framebuffer the scene was rendered into.
    vec2f framebuffer_size{0.0f, 0.0f};
  };

  static bool is_mouse_pressed(const MouseCode button);
  static bool is_mouse_released(const MouseCode button);

  static void  set_mouse_position(const vec2f& position);
  static vec2f get_mouse_position();
  static vec2f get_mouse_screen_position();

  // Set/clear the scene viewport used to convert cursor coordinates. The
  // Sandbox updates this whenever the window or viewport size changes. When no
  // viewport has been set, the full window is used, which preserves the legacy
  // behaviour for editor/UI code.
  static void             set_viewport(const Viewport& viewport);
  static void             clear_viewport();
  static const Viewport&  get_viewport();

  // Convert a window-space point into scene framebuffer pixel coordinates,
  // removing any letterbox offset and applying the composite scale.
  static vec2f window_to_viewport(const vec2f& window_position);
  static vec2f window_to_viewport(const vec2f&   window_position,
                                  const Viewport& viewport);

  // Inverse of window_to_viewport, useful for placing the cursor from game
  // coordinates.
  static vec2f viewport_to_window(const vec2f& viewport_position);
  static vec2f viewport_to_window(const vec2f&   viewport_position,
                                  const Viewport& viewport);

  // Cursor position in scene framebuffer pixels.
  static vec2f get_mouse_viewport_position();

  static vec2f get_mouse_world_position(const View& view);
  static vec2f get_mouse_world_position(const vec2f& mouse_position, const View& view);
  static vec2f get_mouse_world_position(const vec2f& mouse_position,
                                        const View&  view,
                                        const vec2f& viewport_size);
};

}  // namespace ag
