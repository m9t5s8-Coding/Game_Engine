#include <Apch.hpp>

#include <Core/Application.hpp>
#include <Core/Input.hpp>
#include <Renderer/View.hpp>

namespace ag {
namespace {
Mouse::Viewport s_viewport;
}  // namespace

namespace {
Mouse::Viewport
effective_viewport()
{
  Mouse::Viewport viewport = s_viewport;

  if (viewport.window_size.x <= 0.0f || viewport.window_size.y <= 0.0f)
  {
    vec2u window_size = Application::get().get_window().get_size();
    viewport.window_size = vec2f(window_size);
  }

  if (viewport.content_size.x <= 0.0f || viewport.content_size.y <= 0.0f)
    viewport.content_size = viewport.window_size;

  if (viewport.framebuffer_size.x <= 0.0f || viewport.framebuffer_size.y <= 0.0f)
    viewport.framebuffer_size = viewport.window_size;

  return viewport;
}
}  // namespace

namespace {
Mouse::Viewport
resolve_viewport(const Mouse::Viewport& viewport)
{
  Mouse::Viewport resolved = viewport;

  if (resolved.window_size.x <= 0.0f || resolved.window_size.y <= 0.0f)
    resolved.window_size = effective_viewport().window_size;

  if (resolved.content_size.x <= 0.0f || resolved.content_size.y <= 0.0f)
    resolved.content_size = resolved.window_size;

  if (resolved.framebuffer_size.x <= 0.0f || resolved.framebuffer_size.y <= 0.0f)
    resolved.framebuffer_size = resolved.window_size;

  return resolved;
}
}  // namespace

void
Mouse::set_viewport(const Viewport& viewport)
{
  s_viewport = viewport;
}

void
Mouse::clear_viewport()
{
  s_viewport = Viewport{};
}

const Mouse::Viewport&
Mouse::get_viewport()
{
  s_viewport = resolve_viewport(s_viewport);
  return s_viewport;
}

vec2f
Mouse::window_to_viewport(const vec2f& window_position, const Viewport& viewport)
{
  const Viewport resolved = resolve_viewport(viewport);

  // The scene is composited centred in the window at content_size, so remove
  // the letterbox offset and scale the remainder into framebuffer pixels.
  const vec2f offset = (resolved.window_size - resolved.content_size) * 0.5f;
  const vec2f local  = window_position - offset;

  return {local.x / resolved.content_size.x * resolved.framebuffer_size.x,
          local.y / resolved.content_size.y * resolved.framebuffer_size.y};
}

vec2f
Mouse::window_to_viewport(const vec2f& window_position)
{
  return window_to_viewport(window_position, get_viewport());
}

vec2f
Mouse::viewport_to_window(const vec2f& viewport_position, const Viewport& viewport)
{
  const Viewport resolved = resolve_viewport(viewport);

  const vec2f local = {viewport_position.x / resolved.framebuffer_size.x * resolved.content_size.x,
                       viewport_position.y / resolved.framebuffer_size.y * resolved.content_size.y};

  return local + (resolved.window_size - resolved.content_size) * 0.5f;
}

vec2f
Mouse::viewport_to_window(const vec2f& viewport_position)
{
  return viewport_to_window(viewport_position, get_viewport());
}

vec2f
Mouse::get_mouse_viewport_position()
{
  return window_to_viewport(get_mouse_position());
}

vec2f
Mouse::get_mouse_world_position(const View& view)
{
  return get_mouse_world_position(get_mouse_position(), view);
}

vec2f
Mouse::get_mouse_world_position(const vec2f& mouse_position, const View& view)
{
  const Viewport viewport = effective_viewport();
  const vec2f    position = window_to_viewport(mouse_position, viewport);
  return get_mouse_world_position(position, view, viewport.framebuffer_size);
}

vec2f
Mouse::get_mouse_world_position(const vec2f& mouse_position,
                                const View&  view,
                                const vec2f& viewport_size)
{
  vec2f view_size   = view.get_size();
  vec2f view_center = view.get_center();

  vec2f ndc;
  ndc.x = (mouse_position.x / viewport_size.x) * 2.0f - 1.0f;
  ndc.y = (mouse_position.y / viewport_size.y) * 2.0f - 1.0f;

  vec2f world;
  world.x = ndc.x * (view_size.x / 2.0f) + view_center.x;
  world.y = ndc.y * (view_size.y / 2.0f) + view_center.y;

  return world;
}
}  // namespace ag
