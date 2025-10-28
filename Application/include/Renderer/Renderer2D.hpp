#pragma once

#include <Renderer/ViewController.hpp>
#include <Core/Core.hpp>

#include <Renderer/Texture.hpp>

#include <Scene/SceneComponent.hpp>

namespace ag
{

  class Renderer2D
  {
  public:
    static void init();
    static void shut_down();

    static void begin_scene(const View &view);

    static void end_scene();
    static void flush();

    static void start_batch();
    static void set_texture(const AG_ref<Texture> &texture);

    static void draw_rectangle(const Rectangle& rect, const Transform& transform);
    static void draw_circle(const Circle& circle, const Transform& transform);
    static void draw_sprite(const Sprite& sprite, const Transform& transform);

    static void flush_rectangle();
    static void flush_circle();
    static void flush_sprite();
  };
}
