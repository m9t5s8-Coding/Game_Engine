#include <GameObjects/Node/ButtonNode.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>
#include <GameObjects/GameObjects.hpp>
#include <Core/Input.hpp>

namespace ag
{
  void ButtonNode::create_node(Entity entity)
  {
    ButtonState_Component::add_component(entity);
    Button_Component::add_component(entity);
    Transform_Component::add_component(entity);
    Text_Component::add_component(entity);
  }
  void ButtonNode::delete_node(Entity entity)
  {
    Script_Component::destroy(entity);
    entity.delete_entity();
  }
  void ButtonNode::clone_node(Entity original, Entity clone)
  {
    Script_Component::clone_entity(original, clone);
    Transform_Component::clone_entity(original, clone);
    UI_Component::clone_entity(original, clone);
    ButtonState_Component::clone_entity(original, clone);
    Tween_Component::clone_entity(original, clone);
    Button_Component::clone_entity(original, clone);
    Text_Component::clone_entity(original, clone);
  }
  json ButtonNode::save_json(Entity entity)
  {
    json j;
    NodeHelper::save_component<Script_Component>(entity, j);
    NodeHelper::save_component<Transform_Component>(entity, j);
    NodeHelper::save_component<UI_Component>(entity, j);
    NodeHelper::save_component<ButtonState_Component>(entity, j);
    NodeHelper::save_component<Button_Component>(entity, j);
    NodeHelper::save_component<Text_Component>(entity, j);
    NodeHelper::save_component<Tween_Component>(entity, j);

    return j;
  }
  void ButtonNode::load_json(Entity entity, const json& j)
  {
    NodeHelper::load_component<Script_Component>(entity, j);
    NodeHelper::load_component<Transform_Component>(entity, j);
    NodeHelper::load_component<UI_Component>(entity, j);
    NodeHelper::load_component<ButtonState_Component>(entity, j);
    NodeHelper::load_component<Button_Component>(entity, j);
    NodeHelper::load_component<Text_Component>(entity, j);
    NodeHelper::load_component<Tween_Component>(entity, j);
  }
  void ButtonNode::update(Entity entity, TimeStamp ts)
  {
    Script_Component::update(entity, ts);
    Tween_Component::update(entity, ts);

    if (!Engine::is_runtime() || !entity.has_component<Button_Component>())
      return;


    auto& comps = entity.get_component<Button_Component>();
    if (entity.has_component<ButtonState_Component>())
    {
      auto& state = entity.get_component<ButtonState_Component>().button_state;

      if (state & Button_State::Disabled)
      {
        comps.current_state = Button_Visual_State::Disabled;
        return;
      }

      bool mouse_pressed = Mouse::is_mouse_pressed(Button::ButtonLeft);
      bool mouse_just_released = Mouse::is_mouse_released(Button::ButtonLeft);

      if (state & Button_State::Hovered)
      {
        if (state & Button_State::Pressed)
        {
          state |= Button_State::Active;
        }
        else if (state & Button_State::Active && mouse_pressed)
        {
          state |= Button_State::Active;
          state &= ~Button_State::Pressed;
        }
        else if (state & Button_State::Active && mouse_just_released)
        {
          state &= ~Button_State::Active;
        }
      }
      else
      {
        state &= ~(Button_State::Pressed | Button_State::Active);
      }
      comps.current_state = Button_Visual::get_active_state(state);
    }
    else
      comps.current_state = Button_Visual_State::Normal;

    auto it = comps.overrides.find(comps.current_state);
    if (it != comps.overrides.end())
    {
      comps.base = it->second;
      return;
    }
    comps.base = comps.overrides.find(Button_Visual_State::Normal)->second;
  }
  void ButtonNode::draw(Entity entity)
  {
    if (!Tag_Component::get_visibility(entity))
      return;

    int entity_id = (int)(entity.get_id());

    const auto& transform = Transform_Component::get_world_transform(entity);
    Rectangle rectangle;
    Text text;

    if (entity.has_component<Button_Component>() && entity.has_component<Text_Component>())
    {
      auto& props = entity.get_component<Button_Component>();
      auto& text_comps = entity.get_component<Text_Component>();

      rectangle.size = props.layout.size;
      rectangle.fill_color = props.base.background;
      rectangle.border_color = props.base.border;
      rectangle.border_thickness = props.base.border_thickness;
      rectangle.corner_radius = props.base.corner;

      text.text = text_comps.text;
      text.font_size = text_comps.font_size;
      text.text_color = props.base.text;
      text.h_allignment = props.layout.h_allignment;
      text.v_allignment = props.layout.v_allignment;
      text.bounds = rectangle.size * transform.scale;
      text.padding = vec2f(0.0f, 0.0f);

    }



    if (Engine::is_runtime())
    {
      NodeHelper::set_value(entity, &UI_Component::mode, rectangle.mode);
      NodeHelper::set_value(entity, &UI_Component::mode, text.mode);
      text.draw_rect = false;
    }


    Renderer2D::draw_rectangle(rectangle, transform, entity_id);
    Renderer2D::draw_text(text, transform, entity_id);

    if (entity.has_component<ButtonState_Component>())
    {
      auto& state = entity.get_component<ButtonState_Component>().button_state;
      state &= ~(Button_State::Hovered | Button_State::Pressed);
    }
  }
}
