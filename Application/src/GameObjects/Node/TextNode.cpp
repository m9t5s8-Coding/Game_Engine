#include <GameObjects/Node/TextNode.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>
#include <GameObjects/GameObjects.hpp>

namespace ag
{
	void TextNode::create_node(Entity entity)
	{
		Transform_Component::add_component(entity);
		Text_Component::add_component(entity);
	}
	void TextNode::delete_node(Entity entity)
	{
		Script_Component::destroy(entity);
		entity.delete_entity();
	}
	void TextNode::clone_node(Entity original, Entity clone)
	{
		Transform_Component::clone_entity(original, clone);
		Text_Component::clone_entity(original, clone);

		FontStyle_Component::clone_entity(original, clone);

		UI_Component::clone_entity(original, clone);
		Script_Component::clone_entity(original, clone);
	}
	json TextNode::save_json(Entity entity)
	{
		json j;

		NodeHelper::save_component<Transform_Component>(entity, j);
		NodeHelper::save_component<Text_Component>(entity, j);
		NodeHelper::save_component<FontStyle_Component>(entity, j);
		NodeHelper::save_component<UI_Component>(entity, j);
		NodeHelper::save_component<Script_Component>(entity, j);

		return j;
	}
	void TextNode::load_json(Entity entity, const json& j)
	{
		NodeHelper::load_component<Transform_Component>(entity, j);
		NodeHelper::load_component<Text_Component>(entity, j);
		NodeHelper::load_component<FontStyle_Component>(entity, j);
		NodeHelper::load_component<UI_Component>(entity, j);
		NodeHelper::load_component<Script_Component>(entity, j);
	}
	void TextNode::update(Entity entity, TimeStamp ts)
	{
		Script_Component::update(entity, ts);
	}
	void TextNode::draw(Entity entity)
	{
		if (!Tag_Component::get_visibility(entity))
			return;

		int entity_id = (int)(entity.get_id());
		const auto& transform = Transform_Component::get_world_transform(entity);
		Text text;


		NodeHelper::set_value(entity, &Text_Component::text, text.text);
		NodeHelper::set_value(entity, &Text_Component::font_size, text.font_size);



		NodeHelper::set_value(entity, &FontStyle_Component::color, text.text_color);

		if (Engine::is_runtime())
			NodeHelper::set_value(entity, &UI_Component::mode, text.mode);

		AERO_CORE_INFO("Rendering Text: {0}", text.text);
		text.text = "AEROENGINE";
		Renderer2D::draw_text(text, transform, entity_id);
	}
}