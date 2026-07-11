#include <GameObjects/GameObjects.hpp>
#include <GameObjects/Node/TextNode.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag {
void TextNode::create_node(Entity entity) {
  Transform_Component::add_component(entity);
  Text_Component::add_component(entity);
}
void TextNode::delete_node(Entity entity) {
  Script_Component::destroy(entity);
  entity.delete_entity();
}
void TextNode::clone_node(Entity original, Entity clone) {
  Transform_Component::clone_entity(original, clone);
  Text_Component::clone_entity(original, clone);

  FontStyle_Component::clone_entity(original, clone);

  UI_Component::clone_entity(original, clone);
  Script_Component::clone_entity(original, clone);
}
json TextNode::save_json(Entity entity) {
  json j;

  NodeHelper::save_component<Transform_Component>(entity, j);
  NodeHelper::save_component<Text_Component>(entity, j);
  NodeHelper::save_component<FontStyle_Component>(entity, j);
  NodeHelper::save_component<UI_Component>(entity, j);
  NodeHelper::save_component<Script_Component>(entity, j);

  return j;
}
void TextNode::load_json(Entity entity, const json& j) {
  NodeHelper::load_component<Transform_Component>(entity, j);
  NodeHelper::load_component<Text_Component>(entity, j);
  NodeHelper::load_component<FontStyle_Component>(entity, j);
  NodeHelper::load_component<UI_Component>(entity, j);
  NodeHelper::load_component<Script_Component>(entity, j);
}
void TextNode::update(Entity entity, TimeStamp ts) { Script_Component::update(entity, ts); }
void TextNode::draw(Entity entity) {
  int         entity_id = (int)(entity.get_id());
  const auto& transform = Transform_Component::get_world_transform(entity);
  Text        text;

  if (entity.has_component<Text_Component>()) {
    auto& comps = entity.get_component<Text_Component>();

    text.text      = comps.text;
    text.font_size = comps.font_size;
  }

  if (entity.has_component<FontStyle_Component>()) {
    const auto& style = entity.get_component<FontStyle_Component>();

    text.h_allignment = style.h_allignment;
    text.v_allignment = style.v_allignment;
    text.line_height  = style.line_height;
    text.bounds       = style.bounds;
    text.text_color   = style.color;
  }

#ifdef AERO_RUNTIME
  NodeHelper::set_value(entity, &UI_Component::mode, text.mode);
  text.draw_rect = false;
#endif

  Renderer2D::draw_text(text, transform, entity_id);
}
}  // namespace ag
