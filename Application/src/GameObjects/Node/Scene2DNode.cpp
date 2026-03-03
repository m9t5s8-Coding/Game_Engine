#include <GameObjects/GameObjects.hpp>
#include <GameObjects/Node/Scene2DNode.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag
{
void Scene2DNode::create_node(Entity entity)
{
  Transform_Component::add_component(entity);
}

void Scene2DNode::delete_node(Entity entity)
{
  Script_Component::destroy(entity);
  entity.delete_entity();
}

void Scene2DNode::clone_node(Entity original, Entity clone)
{
  Script_Component::clone_entity(original, clone);
  Transform_Component::clone_entity(original, clone);
  Tween_Component::clone_entity(original, clone);
}

json Scene2DNode::save_json(Entity entity)
{
  json j;

  NodeHelper::save_component<Transform_Component>(entity, j);
  NodeHelper::save_component<Script_Component>(entity, j);
  NodeHelper::save_component<Tween_Component>(entity, j);
  return j;
}

void Scene2DNode::load_json(Entity entity, const json& j)
{
  NodeHelper::load_component<Transform_Component>(entity, j);
  NodeHelper::load_component<Script_Component>(entity, j);
  NodeHelper::load_component<Tween_Component>(entity, j);
}

void Scene2DNode::update(Entity entity, TimeStamp ts)
{
  Script_Component::update(entity, ts);
  Tween_Component::update(entity, ts);
}

void Scene2DNode::draw(Entity entity)
{
}

}  // namespace ag
