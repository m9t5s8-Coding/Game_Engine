#pragma once

#include <Renderer/ViewController.hpp>
#include <Core/Core.hpp>

#include <Renderer/Texture.hpp>

#include <Scene/SceneComponent.hpp>

namespace ag
{
  struct Transform_Component;
  struct Text;
  class Renderer2D
  {
  public:
    static void init();
    static void shut_down();

    static void begin_scene(const View& view, const vec2f& viewport_size);
    static void begin_scene();
    static void end_scene();


    static void flush();

    static void start_batch();
    static void set_texture(const AG_ref<Texture>& texture);

    static void draw_rectangle(const Rectangle& rect, const Transform_Component& transform, int entity_id = -1);
    static void draw_circle(const Circle& circle, const Transform_Component& transform, int entity_id = -1);
    static void draw_sprite(const Sprite& sprite, const Transform_Component& transform, int entity_id = -1);
    static void draw_text(const Text& text, const Transform_Component& transform, int entity_id = -1);

    static void draw_fullscreen_quad(AG_uint id);
  };
}
