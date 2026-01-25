#include <GameObjects/Node/TextureButtonNode.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>
#include <GameObjects/GameObjects.hpp>
#include <Core/Input.hpp>

namespace ag
{
	void TextureButtonNode::create_node(Entity entity)
	{
		ButtonState_Component::add_component(entity);
		Textured_Button_Component::add_component(entity);
		Transform_Component::add_component(entity);
		Texture_Component::add_component(entity);
		if (!Engine::is_runtime())
			TextureRect_Component::add_component(entity);
	}
	void TextureButtonNode::delete_node(Entity entity)
	{
		Script_Component::destroy(entity);
		entity.delete_entity();
	}
	void TextureButtonNode::clone_node(Entity original, Entity clone)
	{
		Script_Component::clone_entity(original, clone);
		Transform_Component::clone_entity(original, clone);
		UI_Component::clone_entity(original, clone);
		ButtonState_Component::clone_entity(original, clone);
		Textured_Button_Component::clone_entity(original, clone);
		Texture_Component::clone_entity(original, clone);
		TextureRect_Component::clone_entity(original, clone);
	}

	json TextureButtonNode::save_json(Entity entity)
	{
		json j;
		NodeHelper::save_component<Script_Component>(entity, j);
		NodeHelper::save_component<Transform_Component>(entity, j);
		NodeHelper::save_component<UI_Component>(entity, j);
		NodeHelper::save_component<Texture_Component>(entity, j);
		NodeHelper::save_component<ButtonState_Component>(entity, j);
		NodeHelper::save_component<Textured_Button_Component>(entity, j);

		return j;
	}

	void TextureButtonNode::load_json(Entity entity, const json& j)
	{
		NodeHelper::load_component<Script_Component>(entity, j);
		NodeHelper::load_component<Transform_Component>(entity, j);
		NodeHelper::load_component<UI_Component>(entity, j);
		NodeHelper::load_component<Texture_Component>(entity, j);
		NodeHelper::load_component<ButtonState_Component>(entity, j);
		NodeHelper::load_component<Textured_Button_Component>(entity, j);
	}

	void TextureButtonNode::update(Entity entity, TimeStamp ts)
	{
		Script_Component::update(entity, ts);

		if (!Engine::is_runtime())
			return;

		auto& comps = entity.get_component<Textured_Button_Component>();
		if (entity.has_component<Textured_Button_Component>())
		{
			auto& state = entity.get_component<ButtonState_Component>().button_state;
			if (state & Button_State::Hovered && Mouse::is_mouse_pressed(Button::ButtonLeft))
			{
				state |= Button_State::Active;
			}
			comps.current_state = Button_Visual::get_active_state(state);
		}
		else
			comps.current_state = Button_Visual_State::Normal;

		auto it = comps.overrides.find(comps.current_state);
		if (it != comps.overrides.end())
		{
			comps.base_rect = it->second;
			return;
		}
		comps.base_rect = comps.overrides.find(Button_Visual_State::Normal)->second;
	}

	void TextureButtonNode::draw(Entity entity)
	{
		if (!Tag_Component::get_visibility(entity))
			return;

		if (!entity.has_component<Texture_Component>() || !entity.get_component<Texture_Component>().texture)
			return;


		Renderer2D::set_texture(entity.get_component<Texture_Component>().texture);

		int entity_id = (int)(entity.get_id());

		const auto& transform = Transform_Component::get_world_transform(entity);
		Sprite sprite;




		if (!entity.has_component<Textured_Button_Component>())
			return;

		auto& props = entity.get_component<Textured_Button_Component>();

		sprite.size = props.base_rect.size;
		sprite.texture_rect = props.base_rect;


		if (entity.has_component<TextureFlip_Component>())
		{
			auto& flip = entity.get_component<TextureFlip_Component>();
			sprite.flip_horizontal = flip.horizontal;
			sprite.flip_vertical = flip.vertical;
		}
		if (Engine::is_runtime())
		{
			NodeHelper::set_value(entity, &UI_Component::mode, sprite.mode);
		}


		Renderer2D::draw_sprite(sprite, transform, entity_id);

		if (entity.has_component<ButtonState_Component>())
		{
			auto& state = entity.get_component<ButtonState_Component>().button_state;
			state = 0;
		}
	}
}