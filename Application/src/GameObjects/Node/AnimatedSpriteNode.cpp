#include <GameObjects/GameObjects.hpp>
#include <GameObjects/Node/AnimatedSpriteNode.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag {
void AnimatedSpriteNode::create_node(Entity entity) {
  Transform_Component::add_component(entity);
  Texture_Component::add_component(entity);
  Animation_Component::add_component(entity);
}

void AnimatedSpriteNode::delete_node(Entity entity) {
  Script_Component::destroy(entity);
  PhysicsBody_Component::remove_component(entity);
  entity.delete_entity();
}
void AnimatedSpriteNode::clone_node(Entity original, Entity clone) {
  Transform_Component::clone_entity(original, clone);
  Texture_Component::clone_entity(original, clone);
  // Render2D_Component::clone_entity(original, clone);
  Animation_Component::clone_entity(original, clone);

  Script_Component::clone_entity(original, clone);
  Tween_Component::clone_entity(original, clone);
  TextureFlip_Component::clone_entity(original, clone);
  PhysicsBody_Component::clone_entity(original, clone);
}
json AnimatedSpriteNode::save_json(Entity entity) {
  json j;

  NodeHelper::save_component<Transform_Component>(entity, j);
  NodeHelper::save_component<Texture_Component>(entity, j);
  // NodeHelper::save_component<Render2D_Component>(entity, j);
  NodeHelper::save_component<Animation_Component>(entity, j);

  NodeHelper::save_component<Script_Component>(entity, j);
  NodeHelper::save_component<Tween_Component>(entity, j);
  NodeHelper::save_component<TextureFlip_Component>(entity, j);
  NodeHelper::save_component<PhysicsBody_Component>(entity, j);

  return j;
}
void AnimatedSpriteNode::load_json(Entity entity, const json& j) {
  NodeHelper::load_component<Transform_Component>(entity, j);
  NodeHelper::load_component<Texture_Component>(entity, j);
  // NodeHelper::load_component<Render2D_Component>(entity, j);
  NodeHelper::load_component<Animation_Component>(entity, j);

  NodeHelper::load_component<Script_Component>(entity, j);
  NodeHelper::load_component<Tween_Component>(entity, j);
  NodeHelper::load_component<TextureFlip_Component>(entity, j);
  NodeHelper::load_component<PhysicsBody_Component>(entity, j);
}
void AnimatedSpriteNode::update(Entity entity, TimeStamp ts) {
  Animation_Component::update(entity, ts);
  Tween_Component::update(entity, ts);
  PhysicsBody_Component::update_entity(entity);
  Script_Component::update(entity, ts);
}
void AnimatedSpriteNode::draw(Entity entity) {
  if (entity.has_component<Texture_Component>()) {
    auto& texture = entity.get_component<Texture_Component>();
    if (texture.texture)
      Renderer2D::set_texture(entity.get_component<Texture_Component>().texture);
  }
  const auto& transform = Transform_Component::get_world_transform(entity);
  int         entity_id = (int)(entity.get_id());
  Sprite      sprite;

  NodeHelper::set_value(entity, &Animation_Component::rect, sprite.texture_rect);

  // NodeHelper::set_value(entity, &Render2D_Component::size, sprite.size);
  sprite.size = sprite.texture_rect.size;

  NodeHelper::set_value(entity, &TextureFlip_Component::horizontal, sprite.flip_horizontal);
  NodeHelper::set_value(entity, &TextureFlip_Component::vertical, sprite.flip_vertical);

#ifdef AERO_RUNTIME
  NodeHelper::set_value(entity, &UI_Component::mode, sprite.mode);
#endif

  Renderer2D::draw_sprite(sprite, transform, entity_id);
}
}  // namespace ag
