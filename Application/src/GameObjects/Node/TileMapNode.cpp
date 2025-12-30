#include <GameObjects/Node/TileMapNode.hpp>

namespace ag
{
	void TileMapNode::create_node(Entity entity)
	{
		entity.add_component<Texture_Component>();
		entity.add_component<Tile_Component>();
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
		Script_Component::clone_entity(original, clone);
	}
	json TileMapNode::save_json(Entity entity)
	{
		json j;

		NodeHelper::save_component<Texture_Component>(entity, j);
		NodeHelper::save_component<Tile_Component>(entity, j);
		NodeHelper::save_component<Script_Component>(entity, j);

		return j;
	}
	void TileMapNode::load_json(Entity entity, const json& j)
	{
		NodeHelper::load_component<Texture_Component>(entity, j);
		//NodeHelper::load_component<Tile_Component>(entity, j);
		NodeHelper::load_component<Script_Component>(entity, j);
	}
	void TileMapNode::update(Entity entity, TimeStamp ts)
	{
		Script_Component::update(entity, ts);
	}
	void TileMapNode::draw(Entity entity)
	{
		if (!entity.get_component<Tag_Component>().visible)
			return;

		if (entity.has_component<Texture_Component>())
			Renderer2D::set_texture(entity.get_component<Texture_Component>().texture);

		Tile_Component::draw(entity);
	}
}