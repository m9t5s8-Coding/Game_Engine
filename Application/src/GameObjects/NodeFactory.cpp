#include <GameObjects/NodeFactory.hpp>
#include <GameObjects/Node.hpp>

namespace ag
{
	std::unordered_map<NodeType, std::function<void(Entity)>> NodeFactory::create_map;
	std::unordered_map<NodeType, std::function<void(Entity, TimeStamp)>> NodeFactory::draw_map;
	std::unordered_map<NodeType, std::function<void(Entity)>> NodeFactory::properties_map;
	std::unordered_map<NodeType, std::function<void(Entity, Entity)>> NodeFactory::clone_map;
	std::unordered_map<NodeType, std::function<json(Entity)>> NodeFactory::save_map;
	std::unordered_map<NodeType, std::function<void(Entity, json&)>> NodeFactory::load_map;
	std::unordered_map<NodeType, std::function<void(Entity)>> NodeFactory::clear_map;
	std::unordered_map<NodeType, std::string> NodeFactory::nodes;

	void NodeFactory::init()
	{
		create_map[NodeType::Rectangle] = RectangleNode::create_node;
		create_map[NodeType::Circle] = CircleNode::create_node;
		create_map[NodeType::Sprite] = SpriteNode::create_node;
		create_map[NodeType::AnimatedSprite2D] = AnimatedSprite2DNode::create_node;
		create_map[NodeType::Camera] = CameraComponent::create_node;
		create_map[NodeType::TileMap] = TileMapNode::create_node;
		create_map[NodeType::Scene2D] = Scene2D::create_node;
		create_map[NodeType::TextNode] = TextNode::create_node;



		draw_map[NodeType::Rectangle] = RectangleNode::draw;
		draw_map[NodeType::Circle] = CircleNode::draw;
		draw_map[NodeType::Sprite] = SpriteNode::draw;
		draw_map[NodeType::AnimatedSprite2D] = AnimatedSprite2DNode::draw;
		draw_map[NodeType::Camera] = CameraComponent::draw;
		draw_map[NodeType::TileMap] = TileMapNode::draw;
		draw_map[NodeType::Scene2D] = Scene2D::draw;
		draw_map[NodeType::TextNode] = TextNode::draw;



		properties_map[NodeType::Rectangle] = RectangleNode::show_properties;
		properties_map[NodeType::Circle] = CircleNode::show_properties;
		properties_map[NodeType::Sprite] = SpriteNode::show_properties;
		properties_map[NodeType::AnimatedSprite2D] = AnimatedSprite2DNode::show_properties;
		properties_map[NodeType::Camera] = CameraComponent::show_properties;
		properties_map[NodeType::TileMap] = TileMapNode::show_properties;
		properties_map[NodeType::Scene2D] = Scene2D::show_properties;
		properties_map[NodeType::TextNode] = TextNode::show_properties;



		clone_map[NodeType::Rectangle] = RectangleNode::clone_node;
		clone_map[NodeType::Circle] = CircleNode::clone_node;
		clone_map[NodeType::Sprite] = SpriteNode::clone_node;
		clone_map[NodeType::AnimatedSprite2D] = AnimatedSprite2DNode::clone_node;
		clone_map[NodeType::Camera] = CameraComponent::clone_node;
		clone_map[NodeType::TileMap] = TileMapNode::clone_node;
		clone_map[NodeType::Scene2D] = Scene2D::clone_node;
		clone_map[NodeType::TextNode] = TextNode::clone_node;


		save_map[NodeType::Rectangle] = RectangleNode::save_json;
		save_map[NodeType::Circle] = CircleNode::save_json;
		save_map[NodeType::Sprite] = SpriteNode::save_json;
		save_map[NodeType::AnimatedSprite2D] = AnimatedSprite2DNode::save_json;
		save_map[NodeType::Camera] = CameraComponent::save_json;
		save_map[NodeType::TileMap] = TileMapNode::save_json;
		save_map[NodeType::Scene2D] = Scene2D::save_json;
		save_map[NodeType::TextNode] = TextNode::save_json;


		load_map[NodeType::Rectangle] = RectangleNode::load_json;
		load_map[NodeType::Circle] = CircleNode::load_json;
		load_map[NodeType::Sprite] = SpriteNode::load_json;
		load_map[NodeType::AnimatedSprite2D] = AnimatedSprite2DNode::load_json;
		load_map[NodeType::Camera] = CameraComponent::load_json;
		load_map[NodeType::TileMap] = TileMapNode::load_json;
		load_map[NodeType::Scene2D] = Scene2D::load_json;
		load_map[NodeType::TextNode] = TextNode::load_json;


		clear_map[NodeType::Rectangle] = RectangleNode::delete_node;
		clear_map[NodeType::Circle] = CircleNode::delete_node;
		clear_map[NodeType::Sprite] = SpriteNode::delete_node;
		clear_map[NodeType::AnimatedSprite2D] = AnimatedSprite2DNode::delete_node;
		clear_map[NodeType::Camera] = CameraComponent::delete_node;
		clear_map[NodeType::TileMap] = TileMapNode::delete_node;
		clear_map[NodeType::Scene2D] = Scene2D::delete_node;
		clear_map[NodeType::TextNode] = TextNode::delete_node;


		nodes[NodeType::Rectangle] = "Rectangle2D";
		nodes[NodeType::Circle] = "Circle2D";
		nodes[NodeType::Sprite] = "Sprite2D";
		nodes[NodeType::AnimatedSprite2D] = "AnimatedSprite2D";
		nodes[NodeType::Camera] = "Camera2D";
		nodes[NodeType::TileMap] = "MapLayer2D";
		nodes[NodeType::Scene2D] = "Scene2D";
		nodes[NodeType::TextNode] = "Text2D";

	}

	void NodeFactory::shut_down()
	{
		create_map.clear();
		draw_map.clear();
		properties_map.clear();
		clone_map.clear();
		save_map.clear();
		load_map.clear();
		nodes.clear();
	}
}