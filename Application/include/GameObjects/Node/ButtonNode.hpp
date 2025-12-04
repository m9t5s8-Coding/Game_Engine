#pragma once

#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag
{
  struct ButtonNode
  {
    struct ButtonProps
    {
      bool hovered = false;
      bool pressed = false;

      vec2u size;
      std::string button_text;
      float border_thickness;

      Color normal_color;
      Color hover_color;
      Color active_color;

      Color normal_text_color;
      Color hover_text_color;
      Color active_text_color;

      Color normal_border_color;
      Color hover_border_color;
      Color active_border_color;

      RenderMode mode = RenderMode::Screen;

      static json save_json(Entity entity)
      {
        json j;
        auto &props = entity.get_component<ButtonProps>();
        Helper::save_json(j, "Size", props.size);
        Helper::save_json(j, "Text", props.button_text);

        Helper::save_json(j, "Normal Color", props.normal_color);
        Helper::save_json(j, "Hover Color", props.hover_color);
        Helper::save_json(j, "Active Color", props.active_color);

        Helper::save_json(j, "Normal Text Color", props.normal_text_color);
        Helper::save_json(j, "Hover Text Color", props.hover_text_color);
        Helper::save_json(j, "Active Text Color", props.active_text_color);

        Helper::save_json(j, "Normal Border Color", props.normal_border_color);
        Helper::save_json(j, "Hover Border Color", props.hover_border_color);
        Helper::save_json(j, "Active Border Color", props.active_border_color);

        Helper::save_json(j, "Border Thickness", props.border_thickness);

        Helper::save_json(j, "Mode", static_cast<int>(props.mode));

        return j;
      }

      static void load_json(Entity entity, const json &j)
      {
        auto &props = entity.get_component<ButtonProps>();
        Helper::load_json(j, "Size", props.size);
        Helper::load_json(j, "Text", props.button_text);

        Helper::load_json(j, "Normal Color", props.normal_color);
        Helper::load_json(j, "Hover Color", props.hover_color);
        Helper::load_json(j, "Active Color", props.active_color);

        Helper::load_json(j, "Normal Text Color", props.normal_text_color);
        Helper::load_json(j, "Hover Text Color", props.hover_text_color);
        Helper::load_json(j, "Active Text Color", props.active_text_color);

        Helper::load_json(j, "Normal Border Color", props.normal_border_color);
        Helper::load_json(j, "Hover Border Color", props.hover_border_color);
        Helper::load_json(j, "Active Border Color", props.active_border_color);

        Helper::load_json(j, "Border Thickness", props.border_thickness);

        Helper::load_json(j, "Mode", props.mode);
      }
    };

    static void create_node(Entity entity)
    {
      entity.add_component<ButtonProps>();
      entity.add_component<Transform>();
    }

    static void delete_node(Entity entity)
    {
    }

    static void clone_node(Entity original, Entity duplicate)
    {
      duplicate.add_component<ButtonProps>(original.get_component<ButtonProps>());
      duplicate.add_component<Transform>(original.get_component<Transform>());
    }

    static json save_json(Entity entity)
    {
      json j;
      j["ButtonProps"] = ButtonProps::save_json(entity);
      j["Transform"] = Transform::save(entity);

      if (entity.has_component<ScriptComponent>())
      {
        j["ScriptComponent"] = ScriptComponent::save_json(entity);
      }
      return j;
    }

    static void load_json(Entity entity, const json &j)
    {
      ButtonProps::load_json(entity, j["ButtonProps"]);
      Transform::load(entity, j["Transform"]);

      if (j.contains("ScriptComponent"))
      {
        ScriptComponent::load_json(entity, j["ScriptComponent"]);
      }
    }

    static void show_properties(Entity entity)
    {
      Tag::show_properties(entity);

      auto &props = entity.get_component<ButtonProps>();
      {
        UI::draw_vec2("Size", props.size);
        UI::draw_string("Text", props.button_text);
        UI::draw_value("Border Thickness", props.border_thickness);

        UI::draw_title("Normal");
        UI::draw_color("BackGround Color", props.normal_color);
        UI::draw_color("Text Color", props.normal_text_color);
        UI::draw_color("Border Color", props.normal_border_color);

        UI::draw_title("Hover");
        UI::draw_color("BackGround Color", props.hover_color);
        UI::draw_color("Text Color", props.hover_text_color);
        UI::draw_color("Border Color", props.hover_border_color);

        UI::draw_title("Active");
        UI::draw_color("BackGround Color", props.active_color);
        UI::draw_color("Text Color", props.active_text_color);
        UI::draw_color("Border Color", props.active_border_color);

        UI::draw_title("Render Mode");
        if (ImGui::RadioButton("Screen", props.mode == RenderMode::Screen))
        {
          props.mode == RenderMode::Screen;
        }
        if (ImGui::RadioButton("World", props.mode == RenderMode::World))
        {
          props.mode == RenderMode::World;
        }
      }
      Transform::show_properties(entity);
    }

    static void update(Entity entity, TimeStamp ts)
    {
      ScriptComponent::update(entity, ts);
    }

    static void draw(Entity entity)
    {
      auto &is_visible = entity.get_component<Tag>().is_visible;

      if (!is_visible)
        return;

      auto &props = entity.get_component<ButtonProps>();
      auto& transform = entity.get_component<Transform>();

      Rectangle rect;
      Text text;
      text.text = props.button_text;
      rect.size = props.size;
      rect.mode = props.mode;
      if(props.hovered)
      {
        rect.fill_color = props.hover_color;
        rect.border_color = props.hover_border_color;
        text.text_color = props.hover_text_color;
      }
      else if(props.pressed)
      {
        rect.fill_color = props.active_color;
        rect.border_color = props.active_border_color;
        text.text_color = props.active_text_color;
      }
      else
      {
        rect.fill_color = props.normal_color;
        rect.border_color = props.normal_border_color;
        text.text_color = props.normal_text_color;
      }
      Renderer2D::draw_rectangle(rect, transform);
      Renderer2D::draw_text(text, transform);
    }
  };

}