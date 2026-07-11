#include <Core/Input.hpp>
#include <GameObjects/GameObjects.hpp>
#include <GameObjects/Node/TextureButtonNode.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag {
void TextureButtonNode::create_node(Entity entity) {
  ButtonState_Component::add_component(entity);
  Textured_Button_Component::add_component(entity);
  Transform_Component::add_component(entity);
  Texture_Component::add_component(entity);
#ifdef AERO_EDITOR
  TextureRect_Component::add_component(entity);
#endif
}
void TextureButtonNode::delete_node(Entity entity) {
  Script_Component::destroy(entity);
  entity.delete_entity();
}
void TextureButtonNode::clone_node(Entity original, Entity clone) {
  Script_Component::clone_entity(original, clone);
  Transform_Component::clone_entity(original, clone);
  UI_Component::clone_entity(original, clone);
  ButtonState_Component::clone_entity(original, clone);
  Textured_Button_Component::clone_entity(original, clone);
  Texture_Component::clone_entity(original, clone);
  TextureRect_Component::clone_entity(original, clone);
  Tween_Component::clone_entity(original, clone);
}

json TextureButtonNode::save_json(Entity entity) {
  json j;
  NodeHelper::save_component<Script_Component>(entity, j);
  NodeHelper::save_component<Transform_Component>(entity, j);
  NodeHelper::save_component<UI_Component>(entity, j);
  NodeHelper::save_component<Texture_Component>(entity, j);
  NodeHelper::save_component<ButtonState_Component>(entity, j);
  NodeHelper::save_component<Textured_Button_Component>(entity, j);
  NodeHelper::save_component<Tween_Component>(entity, j);

  return j;
}

void TextureButtonNode::load_json(Entity entity, const json& j) {
  NodeHelper::load_component<Script_Component>(entity, j);
  NodeHelper::load_component<Transform_Component>(entity, j);
  NodeHelper::load_component<UI_Component>(entity, j);
  NodeHelper::load_component<Texture_Component>(entity, j);
  NodeHelper::load_component<ButtonState_Component>(entity, j);
  NodeHelper::load_component<Textured_Button_Component>(entity, j);
  NodeHelper::load_component<Tween_Component>(entity, j);
}

void TextureButtonNode::update(Entity entity, TimeStamp ts) {
  Script_Component::update(entity, ts);
  Tween_Component::update(entity, ts);

#ifdef AERO_EDITOR
  return;
#endif

  if (!entity.has_component<Textured_Button_Component>())
    return;

  auto& comps = entity.get_component<Textured_Button_Component>();
  if (entity.has_component<ButtonState_Component>()) {
    auto& state = entity.get_component<ButtonState_Component>().button_state;

    if (state & Button_State::Disabled) {
      comps.current_state = Button_Visual_State::Disabled;
      return;
    }

    bool mouse_pressed       = Mouse::is_mouse_pressed(Button::ButtonLeft);
    bool mouse_just_released = Mouse::is_mouse_released(Button::ButtonLeft);

    if (state & Button_State::Hovered) {
      if (mouse_pressed) {
        state |= Button_State::Pressed;
      } else if (state & Button_State::Pressed) {
        state |= Button_State::Active;
        state &= ~Button_State::Pressed;
      }
    } else {
      state &= ~Button_State::Pressed;
    }
    comps.current_state = Button_Visual::get_active_state(state);
  } else
    comps.current_state = Button_Visual_State::Normal;

  auto it = comps.overrides.find(comps.current_state);
  if (it != comps.overrides.end()) {
    comps.base_rect = it->second;
    return;
  }
  comps.base_rect = comps.overrides.find(Button_Visual_State::Normal)->second;
}

void TextureButtonNode::draw(Entity entity) {
  if (!entity.has_component<Texture_Component>() ||
      !entity.get_component<Texture_Component>().texture)
    return;

  Renderer2D::set_texture(entity.get_component<Texture_Component>().texture);

  int entity_id = (int)(entity.get_id());

  const auto& transform = Transform_Component::get_world_transform(entity);
  Sprite      sprite;

  if (!entity.has_component<Textured_Button_Component>())
    return;

  auto& props = entity.get_component<Textured_Button_Component>();

  sprite.size         = props.base_rect.size;
  sprite.texture_rect = props.base_rect;

  if (entity.has_component<TextureFlip_Component>()) {
    auto& flip             = entity.get_component<TextureFlip_Component>();
    sprite.flip_horizontal = flip.horizontal;
    sprite.flip_vertical   = flip.vertical;
  }
#ifdef AERO_RUNTIME
  NodeHelper::set_value(entity, &UI_Component::mode, sprite.mode);
#endif

  Renderer2D::draw_sprite(sprite, transform, entity_id);

  if (entity.has_component<ButtonState_Component>()) {
    auto& state = entity.get_component<ButtonState_Component>().button_state;
    state &= ~(Button_State::Hovered | Button_State::Active);
  }
}
}  // namespace ag
