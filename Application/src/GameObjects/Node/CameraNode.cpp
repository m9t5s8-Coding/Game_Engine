#include <GameObjects/Node/CameraNode.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>
#include <GameObjects/GameObjects.hpp>

namespace ag
{
	void CameraNode::create_node(Entity entity)
	{
		Camera_Component::add_component(entity);
	}
	void CameraNode::delete_node(Entity entity)
	{
		Script_Component::destroy(entity);
		entity.delete_entity();
	}
	void CameraNode::clone_node(Entity original, Entity clone)
	{
    Camera_Component::clone_entity(original, clone);
    Script_Component::clone_entity(original, clone);
	}
	json CameraNode::save_json(Entity entity)
	{
    json j;
    NodeHelper::save_component<Camera_Component>(entity, j);
    NodeHelper::save_component<Script_Component>(entity, j);
		NodeHelper::save_component<Window_Component>(entity, j);
    return j;
	}
	void CameraNode::load_json(Entity entity, const json& j)
	{
    NodeHelper::load_component<Camera_Component>(entity, j);
    NodeHelper::load_component<Script_Component>(entity, j);
		NodeHelper::load_component<Window_Component>(entity, j);
	}
	void CameraNode::update(Entity entity, TimeStamp ts)
	{
    Script_Component::update(entity, ts);
	}
	void CameraNode::draw(Entity entity)
	{

	}
}
