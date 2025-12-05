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

      vec2f size = { 150,50 };
      std::string button_text = "Button";
      float border_thickness = 1.0f;
      float font_size = 24.0f;
      float corner_radius;

      Color normal_color = Color::White;
      Color hover_color = Color::White;
      Color active_color = Color::White;

      Color normal_text_color = Color::Black;
      Color hover_text_color = Color::Black;
      Color active_text_color = Color::Black;

      Color normal_border_color = Color::Black;
      Color hover_border_color = Color::Black;
      Color active_border_color = Color::Black;

      RenderMode mode = RenderMode::Screen;

      static json save_json(Entity entity)
      {
        json j;
        auto &props = entity.get_component<ButtonProps>();
        Helper::save_json(j, "Size", props.size);
        Helper::save_json(j, "Text", props.button_text);
        Helper::save_json(j, "Font Size", props.font_size);
        Helper::save_json(j, "Corner Radius", props.corner_radius);

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
        Helper::load_json(j, "Font Size", props.font_size);
        Helper::load_json(j, "Corner Radius", props.corner_radius);

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
      if (j.contains("Transform"))
      {
        Transform::load(entity, j["Transform"]);
      }

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
        UI::draw_value("Font Size", props.font_size);
        UI::draw_value("Corner Radius", props.corner_radius);


        UI::draw_title("Normal");
        UI::draw_color("BackGround Color##normal", props.normal_color);
        UI::draw_color("Text Color##normal", props.normal_text_color);
        UI::draw_color("Border Color##normal", props.normal_border_color);

        UI::draw_title("Hover");
        UI::draw_color("BackGround Color##hover", props.hover_color);
        UI::draw_color("Text Color##hover", props.hover_text_color);
        UI::draw_color("Border Color##hover", props.hover_border_color);

        UI::draw_title("Active");
        UI::draw_color("BackGround Color##active", props.active_color);
        UI::draw_color("Text Color##active", props.active_text_color);
        UI::draw_color("Border Color##active", props.active_border_color);

        UI::draw_title("Render Mode");
        if (ImGui::RadioButton("Screen", props.mode == RenderMode::Screen))
        {
          props.mode = RenderMode::Screen;
        }
        if (ImGui::RadioButton("World", props.mode == RenderMode::World))
        {
          props.mode = RenderMode::World;
        }
      }
      Transform::show_properties(entity);
    }

    static void update(Entity entity, TimeStamp ts)
    {
      if (!Engine::is_runtime())
        return;

      auto& props = entity.get_component<ButtonProps>();
      auto& transform = entity.get_component<Transform>();
      float_rect rect;


      rect.position = transform.position - (props.size * transform.scale) / 2;


      rect.size = { props.size * transform.scale };


      auto view_rect = ViewController::get_view_floatrect();
      auto mouse_pos = Mouse::get_mouse_position();
      auto size = Application::get().get_window().get_size();
      mouse_pos = Math::screen_to_world(mouse_pos, view_rect, size);



      if (rect.contains(mouse_pos))
      {
        props.hovered = true;
        props.pressed = false;
        if (Mouse::is_mouse_pressed(Button::ButtonLeft) && props.hovered)
        {
          props.hovered = false;
          props.pressed = true;
        }
      }
      else
      {
        props.hovered = false;
        props.pressed = false;
      }
      ScriptComponent::update(entity, ts);
    }

    static void draw(Entity entity)
    {
      auto &is_visible = entity.get_component<Tag>().is_visible;

      if (!is_visible)
        return;

      auto &props = entity.get_component<ButtonProps>();
      const auto& transform = Transform::get_world_transform(entity);

      Rectangle rect;
      Text text;
      text.text = props.button_text;
      text.font_size = props.font_size;
      rect.size = props.size;
      rect.border_thickness = props.border_thickness;
      rect.corner_radius = props.corner_radius;

      if (Engine::is_runtime())
      {
        rect.mode = props.mode;
        text.mode = props.mode;
      }
      else
      {
        rect.mode = RenderMode::World;
        text.mode = RenderMode::World;
      }
      
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