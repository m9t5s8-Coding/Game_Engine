#include <GameObjects/Node/RectangleNode.hpp>
#include <Renderer/Renderer2D.hpp>

namespace ag
{
	// Rectangle Node Properties



	// Rectangle Node
	void RectangleNode::create_node(Entity entity)
	{
		Transform_Component::add_component(entity);
		Render2D_Component::add_component(entity);
	}

	void RectangleNode::delete_node(Entity entity)
	{
		PhysicsBody_Component::delete_entity(entity);
		Script_Component::destroy(entity);
		entity.delete_entity();
	}

	void RectangleNode::clone_node(Entity original, Entity clone)
	{
		Script_Component::clone_entity(original, clone);
		Transform_Component::clone_entity(original, clone);
		Render2D_Component::clone_entity(original, clone);
		Border_Component::clone_entity(original, clone);
		Corner_Component::clone_entity(original, clone);
		UI_Component::clone_entity(original, clone);
		PhysicsBody_Component::clone_entity(original, clone);
	}

	json RectangleNode::save_json(Entity entity)
	{
		json j;


		NodeHelper::save_component<Render2D_Component>(entity, j);
		NodeHelper::save_component<Transform_Component>(entity, j);
		NodeHelper::save_component<Script_Component>(entity, j);
		NodeHelper::save_component<Border_Component>(entity, j);
		NodeHelper::save_component<Corner_Component>(entity, j);
		NodeHelper::save_component<UI_Component>(entity, j);
		NodeHelper::save_component<PhysicsBody_Component>(entity, j);

		return j;
	}

	void RectangleNode::load_json(Entity entity, const json& j)
	{
		NodeHelper::load_component<Transform_Component>(entity, j);
		NodeHelper::load_component<Render2D_Component>(entity, j);
		NodeHelper::load_component<Script_Component>(entity, j);
		NodeHelper::load_component<Border_Component>(entity, j);
		NodeHelper::load_component<Corner_Component>(entity, j);
		NodeHelper::load_component<UI_Component>(entity, j);
		NodeHelper::load_component<PhysicsBody_Component>(entity, j);
	}

	void RectangleNode::update(Entity entity, TimeStamp ts)
	{
		PhysicsBody_Component::update_entity(entity);
		Script_Component::update(entity, ts);
	}

	void RectangleNode::draw(Entity entity)
	{
		if (!entity.get_component<Tag_Component>().visible)
			return;

		const auto& transform = Transform_Component::get_world_transform(entity);
		Rectangle rectangle;

		NodeHelper::set_value(entity, &Render2D_Component::size, rectangle.size);
		NodeHelper::set_value(entity, &Render2D_Component::color, rectangle.fill_color);


		NodeHelper::set_value(entity, &Border_Component::thickness, rectangle.border_thickness);
		NodeHelper::set_value(entity, &Border_Component::color, rectangle.border_color);

		NodeHelper::set_value(entity, &Corner_Component::corner, rectangle.corner_radius);

		if (Engine::is_runtime())
			NodeHelper::set_value(entity, &UI_Component::mode, rectangle.mode);


		Renderer2D::draw_rectangle(rectangle, transform);
	}



}