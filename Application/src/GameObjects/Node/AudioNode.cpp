#include <GameObjects/GameObjects.hpp>
#include <GameObjects/Node/AudioNode.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag
{
void AudioNode::create_node(Entity entity)
{
  Audio_Component::add_component(entity);
}
void AudioNode::delete_node(Entity entity)
{
  Script_Component::destroy(entity);
  Audio_Component::delete_entity(entity);
  entity.delete_entity();
}
void AudioNode::clone_node(Entity original, Entity clone)
{
  Script_Component::clone_entity(original, clone);
  Audio_Component::clone_entity(original, clone);
}
json AudioNode::save_json(Entity entity)
{
  json j;

  NodeHelper::save_component<Audio_Component>(entity, j);
  NodeHelper::save_component<Script_Component>(entity, j);
  return j;
}
void AudioNode::load_json(Entity entity, const json& j)
{
  NodeHelper::load_component<Audio_Component>(entity, j);
  NodeHelper::load_component<Audio_Component>(entity, j);
}
void AudioNode::update(Entity entity, TimeStamp ts)
{
}
void AudioNode::draw(Entity entity)
{
}
}  // namespace ag
