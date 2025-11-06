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
      vec2u size{100, 100};
      Color fill_color = Color::White;

      float border_thickness = 0.0f;
      Color border_color = Color::White;

      static json save(Entity entity)
      {
        json j;
        const auto& props = entity.get_component<RectangleProp>();

        j["Size"] = props.size.save();
        j["Fill Color"] = props.fill_color.save();
        j["Border Color"] = props.border_color.save();
        j["Border Thickness"] = props.border_thickness;

        return j;
      }

      static void load(Entity entity, const json& j)
      {
        auto& props = entity.get_component<RectangleProp>();
        props.size.load(j["Size"]);
        props.fill_color.load(j["Fill Color"]);
        props.border_color.load(j["Border Color"]);
        props.border_thickness = j["Border Thickness"].get<float>();
      }
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

    static void clone_node(Entity original, Entity clone)
    {
      clone.add_component<Transform>(original.get_component<Transform>());
      clone.add_component<RectangleProp>(original.get_component<RectangleProp>());
    }

    static json save_json(Entity entity)
    {
      json j;
      j["RectangleProps"] = RectangleProp::save(entity);
      j["Transform"] = Transform::save(entity);

      return j;
    }

    static void load_json(Entity entity, json j)
    {
      RectangleProp::load(entity, j["RectangleProps"]);
      Transform::load(entity, j["Transform"]);
    }

    static void show_properties(Entity entity)
    {
      {
        Tag::show_properties(entity);
        {
          auto& props = entity.get_component<RectangleProp>();
          UI::draw_vec2("Size", props.size);
          UI::draw_value("Border Thickness", props.border_thickness);
          UI::draw_color("Fill Color", props.fill_color);
          UI::draw_color("Border Color", props.border_color);


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
      rectangle.border_color = rect.border_color;
      rectangle.border_thickness = rect.border_thickness;
      Renderer2D::draw_rectangle(rectangle, transform);
    }

  };
}
