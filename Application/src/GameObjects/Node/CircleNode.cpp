#include <GameObjects/Node/CircleNode.hpp>

namespace ag
{

	json CircleNode::Circle_Component::save_json(Entity entity)
	{
		json j;
		const auto& props = entity.get_component<Circle_Component>();
		Helper::save_json(j, "Size", props.size);
		Helper::save_json(j, "Color", props.color);

		return j;
	}
	void CircleNode::Circle_Component::load_json(Entity entity, const json& j)
	{
		auto& props = entity.get_component<Circle_Component>();

		Helper::load_json(j, "Size", props.size);
		Helper::load_json(j, "Fill Color", props.color);
	}

	void CircleNode::create_node(Entity entity)
	{
		entity.add_component<Transform>();
		entity.add_component<Circle_Component>();
	}
	void CircleNode::delete_node(Entity entity)
	{
		ScriptComponent::destroy(entity);
		entity.delete_entity();
	}
	void CircleNode::clone_node(Entity original, Entity clone)
	{
		clone.add_component<Transform>(original.get_component<Transform>());
		clone.add_component<Circle_Component>(original.get_component<Circle_Component>());
		if (original.has_component<ScriptComponent>())
		{
			clone.add_component<ScriptComponent>(original.get_component<ScriptComponent>());
		}
		Border_Component::clone_entity(original, clone);
		UI_Component::clone_entity(original, clone);
	}
	json CircleNode::save_json(Entity entity)
	{
		json j;
		j["Transform"] = Transform::save(entity);

		NodeHelper::save_component<Circle_Component>(entity, j, "Circle");
		//NodeHelper::save_component<ScriptComponent>(entity, j, "ScriptComponent");

		NodeHelper::save_component<ScriptComponent>(entity, j, "ScriptComponent");
		NodeHelper::save_component<Border_Component>(entity, j, "BorderComponent");
		NodeHelper::save_component<UI_Component>(entity, j, "UIComponent");

		return j;
	}
	void CircleNode::load_json(Entity entity, const json& j)
	{
		Transform::load(entity, j["Transform"]);

		NodeHelper::load_component<Circle_Component>(entity, j, "Circle");
		//NodeHelper::load_component<ScriptComponent>(entity, j, "ScriptComponent");

		NodeHelper::load_component<ScriptComponent>(entity, j, "ScriptComponent");
		NodeHelper::load_component<Border_Component>(entity, j, "BorderComponent");
		NodeHelper::load_component<UI_Component>(entity, j, "UIComponent");
	}

	void CircleNode::update(Entity entity, TimeStamp ts)
	{
		ScriptComponent::update(entity, ts);

		auto is_visible = entity.get_component<Tag>().is_visible;
		if (!is_visible)
			return;
	}

	void CircleNode::draw(Entity entity)
	{
		auto is_visible = entity.get_component<Tag>().is_visible;
		if (!is_visible)
			return;

		const auto& transform = Transform::get_world_transform(entity);
		auto& c = entity.get_component<Circle_Component>();
		Circle circle;
		circle.size = c.size;
		circle.fill_color = c.color;


		NodeHelper::set_value(entity, &Border_Component::thickness, circle.border_thickness);
		NodeHelper::set_value(entity, &Border_Component::color, circle.border_color);

		if(Engine::is_runtime())
			NodeHelper::set_value(entity, &UI_Component::mode, circle.mode);


		Renderer2D::draw_circle(circle, transform);
	}

}