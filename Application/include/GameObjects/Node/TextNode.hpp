#pragma once

#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag
{
  struct TextNode
  {
    struct TextProp
    {
      Text text;

      static json save(Entity entity)
      {
        json j;
        const auto& props = entity.get_component<TextProp>();

        Helper::save_json(j, "Text", props.text.text);
        Helper::save_json(j, "Text Color", props.text.text_color);
        Helper::save_json(j, "Font Size", props.text.font_size);
        return j;
      }

      static void load(Entity entity, const json& j)
      {
        auto& props = entity.get_component<TextProp>();
        Helper::load_json(j, "Text", props.text.text);
        Helper::load_json(j, "Text Color", props.text.text_color);
        Helper::load_json(j, "Font Size", props.text.font_size);
      }
    };

    static void create_node(Entity entity)
    {
      entity.add_component<Transform>();
      entity.add_component<TextProp>();
    }

    static void delete_node(Entity entity)
    {
      ScriptComponent::destroy(entity);
      entity.delete_entity();
    }

    static void clone_node(Entity original, Entity clone)
    {
      clone.add_component<Transform>(original.get_component<Transform>());
      clone.add_component<TextProp>(original.get_component<TextProp>());

      if (original.has_component<ScriptComponent>())
      {
        clone.add_component<ScriptComponent>(original.get_component<ScriptComponent>());
      }
    }

    static json save_json(Entity entity)
    {
      json j;
      j["TextProps"] = TextProp::save(entity);
      j["Transform"] = Transform::save(entity);

      if (entity.has_component<ScriptComponent>())
      {
        j["ScriptComponent"] = ScriptComponent::save_json(entity);
      }

      return j;
    }

    static void load_json(Entity entity, json j)
    {
      TextProp::load(entity, j["TextProps"]);
      Transform::load(entity, j["Transform"]);

      if (j.contains("ScriptComponent"))
      {
        ScriptComponent::load_json(entity, j["ScriptComponent"]);
      }

    }

    static void show_properties(Entity entity)
    {
      {
        Tag::show_properties(entity);
        {
          auto& props = entity.get_component<TextProp>();
          UI::draw_string("Text", props.text.text);
          UI::draw_color("Fill Color", props.text.text_color);
          UI::draw_value("Font Size", props.text.font_size);
        }
        Transform::show_properties(entity);
      }
    }

    static void update(Entity entity, TimeStamp ts)
    {
      ScriptComponent::update(entity, ts);
    }

    static void draw(Entity entity)
    {
      auto is_visible = entity.get_component<Tag>().is_visible;
      if (!is_visible)
        return;

      auto& transform = entity.get_component<Transform>();
      auto& props = entity.get_component<TextProp>();

      Renderer2D::draw_text(props.text, transform);
    }

  };
}