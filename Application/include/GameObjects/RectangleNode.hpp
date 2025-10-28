#pragma once

#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag
{
  struct RectangleNode
  {
    struct RectangleProp
    {
      vec2u size{50, 50};
      Color fill_color = Color::White;

      float border_thickness = 0.0f;
      Color border_color = Color::White;
    };

    static void create_node(Entity entity)
    {
      entity.add_component<Transform>();
      entity.add_component<RectangleProp>();
    }

    static void delete_node(Entity entity)
    {
        entity.delete_entity();
    }

    static void show_properties(Entity entity)
    {
      {
        Tag::show_properties(entity);
        {
          vec2u& size = entity.get_component<RectangleProp>().size;
          UI::draw_vec2("Size", size);
          Color &color = entity.get_component<RectangleProp>().fill_color;
          UI::draw_color("Fill Color", color);
        }
        Transform::show_properties(entity);
      }
    }

    static void draw(Entity entity, TimeStamp ts)
    {
      auto &transform = entity.get_component<Transform>();
      auto &rect = entity.get_component<RectangleProp>();
      Rectangle rectangle;
      rectangle.size = rect.size;
      rectangle.fill_color = rect.fill_color;
      Renderer2D::draw_rectangle(rectangle, transform);
    }

  };
}
