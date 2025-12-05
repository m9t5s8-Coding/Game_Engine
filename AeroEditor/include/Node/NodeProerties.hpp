#pragma once

#include <Scene/Entity.hpp>

namespace ag
{
  class NodeProperties
  {
    public:
    static void animated_sprite_2D(Entity entity);
    static void button_2D(Entity entity);
    static void camera_2D(Entity entity);
    static void character_body_2D(Entity entity);
    static void circle_2D(Entity entity);
    static void rectangle_2D(Entity entity);
    static void scene_2D(Entity entity);
    static void sprite_2D(Entity entity);
    static void text_2D(Entity entity);
    static void texture_button_2D(Entity entity);
    static void tilemap_2D(Entity entity);
  };
}
