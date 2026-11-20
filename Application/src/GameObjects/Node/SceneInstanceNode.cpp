#include <GameObjects/GameObjects.hpp>
#include <GameObjects/Node/SceneInstanceNode.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag {
void SceneInstanceNode::create_node(Entity entity) {
  Transform_Component::add_component(entity);
  SceneInstance_Component::add_component(entity);

  auto& instance = entity.get_component<SceneInstance_Component>();
  instance.instance_id = entity.get_component<Tag_Component>().index;
}

void SceneInstanceNode::delete_node(Entity entity) {
  Script_Component::destroy(entity);
  entity.delete_entity();
}

void SceneInstanceNode::clone_node(Entity original, Entity clone) {
  Script_Component::clone_entity(original, clone);
  Transform_Component::clone_entity(original, clone);
  Tween_Component::clone_entity(original, clone);
  SceneInstance_Component::clone_entity(original, clone);

  // The clone owns a fresh set of generated children.
  clone.get_component<SceneInstance_Component>().instance_id =
      clone.get_component<Tag_Component>().index;
}

json SceneInstanceNode::save_json(Entity entity) {
  json j;
  NodeHelper::save_component<Transform_Component>(entity, j);
  NodeHelper::save_component<Script_Component>(entity, j);
  NodeHelper::save_component<Tween_Component>(entity, j);
  NodeHelper::save_component<SceneInstance_Component>(entity, j);
  return j;
}

void SceneInstanceNode::load_json(Entity entity, const json& j) {
  NodeHelper::load_component<Transform_Component>(entity, j);
  NodeHelper::load_component<Script_Component>(entity, j);
  NodeHelper::load_component<Tween_Component>(entity, j);
  NodeHelper::load_component<SceneInstance_Component>(entity, j);

  auto& instance = entity.get_component<SceneInstance_Component>();
  if (instance.instance_id == 0)
    instance.instance_id = entity.get_component<Tag_Component>().index;
}

void SceneInstanceNode::update(Entity entity, TimeStamp ts) {
  Script_Component::update(entity, ts);
  Tween_Component::update(entity, ts);
}

void SceneInstanceNode::draw(Entity entity) {
}
}  // namespace ag
