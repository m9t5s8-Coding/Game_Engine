#include <GameObjects/Node/TileMapNode.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>
#include <GameObjects/GameObjects.hpp>

namespace ag
{
	void TileMapNode::create_node(Entity entity)
	{
		Texture_Component::add_component(entity);
		Tile_Component::add_component(entity);
	}
	void TileMapNode::delete_node(Entity entity)
	{
		Script_Component::destroy(entity);
		entity.delete_entity();
	}
	void TileMapNode::clone_node(Entity original, Entity clone)
	{
		Texture_Component::clone_entity(original, clone);
		Tile_Component::clone_entity(original, clone);
		TileSet_Component::clone_entity(original, clone);
		SolidSet_Component::clone_entity(original, clone);
		Script_Component::clone_entity(original, clone);
		AutoTiling_Component::clone_entity(original, clone);
	}
	json TileMapNode::save_json(Entity entity)
	{
		json j;

		NodeHelper::save_component<Texture_Component>(entity, j);
		NodeHelper::save_component<Tile_Component>(entity, j);
		NodeHelper::save_component<TileSet_Component>(entity, j);
		NodeHelper::save_component<SolidSet_Component>(entity, j);
		NodeHelper::save_component<Script_Component>(entity, j);
		NodeHelper::save_component<AutoTiling_Component>(entity, j);

		return j;
	}
	void TileMapNode::load_json(Entity entity, const json& j)
	{
		NodeHelper::load_component<Texture_Component>(entity, j);
		NodeHelper::load_component<Tile_Component>(entity, j);
		NodeHelper::load_component<TileSet_Component>(entity, j);
		NodeHelper::load_component<SolidSet_Component>(entity, j);
		NodeHelper::load_component<Script_Component>(entity, j);
		NodeHelper::load_component<AutoTiling_Component>(entity, j);
	}
	void TileMapNode::update(Entity entity, TimeStamp ts)
	{
		TileSet_Component::update(entity);
		SolidSet_Component::update(entity);
		Script_Component::update(entity, ts);
	}
	void TileMapNode::draw(Entity entity)
	{
		if (!Tag_Component::get_visibility(entity))
			return;

		if (entity.has_component<Texture_Component>() )
			Renderer2D::set_texture(entity.get_component<Texture_Component>().texture);

		TileSet_Component::draw(entity);
		SolidSet_Component::draw(entity);
	}
}