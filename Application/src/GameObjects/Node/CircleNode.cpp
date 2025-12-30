#include <GameObjects/Node/CircleNode.hpp>
#include <Renderer/Renderer2D.hpp>

namespace ag
{

	void CircleNode::create_node(Entity entity)
	{
		entity.add_component<Transform_Component>();
		entity.add_component<Render2D_Component>();
	}

	void CircleNode::delete_node(Entity entity)
	{
		Script_Component::destroy(entity);
		entity.delete_entity();
	}

	void CircleNode::clone_node(Entity original, Entity clone)
	{
		Script_Component::clone_entity(original, clone);
		Transform_Component::clone_entity(original, clone);
		Render2D_Component::clone_entity(original, clone);
		Border_Component::clone_entity(original, clone);
		UI_Component::clone_entity(original, clone);
	}

	json CircleNode::save_json(Entity entity)
	{
		json j;

		NodeHelper::save_component<Transform_Component>(entity, j);
		NodeHelper::save_component<Render2D_Component>(entity, j);
		NodeHelper::save_component<Script_Component>(entity, j);
		NodeHelper::save_component<Border_Component>(entity, j);
		NodeHelper::save_component<UI_Component>(entity, j);

		return j;
	}

	void CircleNode::load_json(Entity entity, const json& j)
	{

		NodeHelper::load_component<Transform_Component>(entity, j);
		NodeHelper::load_component<Render2D_Component>(entity, j);
		NodeHelper::load_component<Script_Component>(entity, j);
		NodeHelper::load_component<Border_Component>(entity, j);
		NodeHelper::load_component<UI_Component>(entity, j);
	}

	void CircleNode::update(Entity entity, TimeStamp ts)
	{
		Script_Component::update(entity, ts);
	}

	void CircleNode::draw(Entity entity)
	{
		if (!entity.get_component<Tag_Component>().visible)
			return;

		auto transform = Transform_Component::get_world_transform(entity);
		Circle circle;

		NodeHelper::set_value(entity, &Render2D_Component::size, circle.size);
		NodeHelper::set_value(entity, &Render2D_Component::color, circle.fill_color);

		NodeHelper::set_value(entity, &Border_Component::thickness, circle.border_thickness);
		NodeHelper::set_value(entity, &Border_Component::color, circle.border_color);

		if(Engine::is_runtime())
			NodeHelper::set_value(entity, &UI_Component::mode, circle.mode);

		Renderer2D::draw_circle(circle, transform);
	}

}
