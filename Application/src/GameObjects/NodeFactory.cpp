#include <GameObjects/NodeFactory.hpp>
#include <GameObjects/Node.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag
{
	std::unordered_map<NodeType, std::function<void(Entity)>> NodeFactory::create_map;
	std::unordered_map<NodeType, std::function<void(Entity)>> NodeFactory::draw_map;
	std::unordered_map<NodeType, std::function<void(Entity, TimeStamp)>> NodeFactory::update_map;
	std::unordered_map<NodeType, std::function<void(Entity)>> NodeFactory::properties_map;
	std::unordered_map<NodeType, std::function<void(Entity, Entity)>> NodeFactory::clone_map;
	std::unordered_map<NodeType, std::function<json(Entity)>> NodeFactory::save_map;
	std::unordered_map<NodeType, std::function<void(Entity, json&)>> NodeFactory::load_map;
	std::unordered_map<NodeType, std::function<void(Entity)>> NodeFactory::clear_map;
	std::unordered_map<NodeType, std::string> NodeFactory::nodes;

	void NodeFactory::init()
	{
		// Create Node Function
		create_map[NodeType::Rectangle] = RectangleNode::create_node;
		create_map[NodeType::Circle] = CircleNode::create_node;
		create_map[NodeType::Sprite] = SpriteNode::create_node;
		create_map[NodeType::AnimatedSprite2D] = AnimatedSpriteNode::create_node;
		create_map[NodeType::Camera] = CameraNode::create_node;
		create_map[NodeType::TileMap] = TileMapNode::create_node;
		create_map[NodeType::Scene2D] = Scene2DNode::create_node;
		create_map[NodeType::Text] = TextNode::create_node;
		create_map[NodeType::Button] = ButtonNode::create_node;
		create_map[NodeType::TextureButton] = TextureButtonNode::create_node;
		create_map[NodeType::Audio] = AudioNode::create_node;


		// Draw Function
		draw_map[NodeType::Rectangle] = RectangleNode::draw;
		draw_map[NodeType::Circle] = CircleNode::draw;
		draw_map[NodeType::Sprite] = SpriteNode::draw;
		draw_map[NodeType::AnimatedSprite2D] = AnimatedSpriteNode::draw;
		draw_map[NodeType::Camera] = CameraNode::draw;
		draw_map[NodeType::TileMap] = TileMapNode::draw;
		draw_map[NodeType::Scene2D] = Scene2DNode::draw;
		draw_map[NodeType::Text] = TextNode::draw;
		draw_map[NodeType::Button] = ButtonNode::draw;
		draw_map[NodeType::TextureButton] = TextureButtonNode::draw;
		draw_map[NodeType::Audio] = AudioNode::draw;

		update_map[NodeType::Rectangle] = RectangleNode::update;
		update_map[NodeType::Circle] = CircleNode::update;
		update_map[NodeType::Sprite] = SpriteNode::update;
		update_map[NodeType::AnimatedSprite2D] = AnimatedSpriteNode::update;
		update_map[NodeType::Camera] = CameraNode::update;
		update_map[NodeType::TileMap] = TileMapNode::update;
		update_map[NodeType::Scene2D] = Scene2DNode::update;
		update_map[NodeType::Text] = TextNode::update;
		update_map[NodeType::Button] = ButtonNode::update;
		update_map[NodeType::TextureButton] = TextureButtonNode::update;
		update_map[NodeType::Audio] = AudioNode::update;



	



		clone_map[NodeType::Rectangle] = RectangleNode::clone_node;
		clone_map[NodeType::Circle] = CircleNode::clone_node;
		clone_map[NodeType::Sprite] = SpriteNode::clone_node;
		clone_map[NodeType::AnimatedSprite2D] = AnimatedSpriteNode::clone_node;
		clone_map[NodeType::Camera] = CameraNode::clone_node;
		clone_map[NodeType::TileMap] = TileMapNode::clone_node;
		clone_map[NodeType::Scene2D] = Scene2DNode::clone_node;
		clone_map[NodeType::Text] = TextNode::clone_node;
		clone_map[NodeType::Button] = ButtonNode::clone_node;
		clone_map[NodeType::TextureButton] = TextureButtonNode::clone_node;
		clone_map[NodeType::Audio] = AudioNode::clone_node;


		save_map[NodeType::Rectangle] = RectangleNode::save_json;
		save_map[NodeType::Circle] = CircleNode::save_json;
		save_map[NodeType::Sprite] = SpriteNode::save_json;
		save_map[NodeType::AnimatedSprite2D] = AnimatedSpriteNode::save_json;
		save_map[NodeType::Camera] = CameraNode::save_json;
		save_map[NodeType::TileMap] = TileMapNode::save_json;
		save_map[NodeType::Scene2D] = Scene2DNode::save_json;
		save_map[NodeType::Text] = TextNode::save_json;
		save_map[NodeType::Button] = ButtonNode::save_json;
		save_map[NodeType::TextureButton] = TextureButtonNode::save_json;
		save_map[NodeType::Audio] = AudioNode::save_json;


		load_map[NodeType::Rectangle] = RectangleNode::load_json;
		load_map[NodeType::Circle] = CircleNode::load_json;
		load_map[NodeType::Sprite] = SpriteNode::load_json;
		load_map[NodeType::AnimatedSprite2D] = AnimatedSpriteNode::load_json;
		load_map[NodeType::Camera] = CameraNode::load_json;
		load_map[NodeType::TileMap] = TileMapNode::load_json;
		load_map[NodeType::Scene2D] = Scene2DNode::load_json;
		load_map[NodeType::Text] = TextNode::load_json;
		load_map[NodeType::Button] = ButtonNode::load_json;
		load_map[NodeType::TextureButton] = TextureButtonNode::load_json;
		load_map[NodeType::Audio] = AudioNode::load_json;


		clear_map[NodeType::Rectangle] = RectangleNode::delete_node;
		clear_map[NodeType::Circle] = CircleNode::delete_node;
		clear_map[NodeType::Sprite] = SpriteNode::delete_node;
		clear_map[NodeType::AnimatedSprite2D] = AnimatedSpriteNode::delete_node;
		clear_map[NodeType::Camera] = CameraNode::delete_node;
		clear_map[NodeType::TileMap] = TileMapNode::delete_node;
		clear_map[NodeType::Scene2D] = Scene2DNode::delete_node;
		clear_map[NodeType::Text] = TextNode::delete_node;
		clear_map[NodeType::Button] = ButtonNode::delete_node;
		clear_map[NodeType::TextureButton] = TextureButtonNode::delete_node;
		clear_map[NodeType::Audio] = AudioNode::delete_node;


		nodes[NodeType::Rectangle] = "Rectangle2D";
		nodes[NodeType::Circle] = "Circle2D";
		nodes[NodeType::Sprite] = "Sprite2D";
		nodes[NodeType::AnimatedSprite2D] = "AnimatedSprite2D";
		nodes[NodeType::Camera] = "Camera2D";
		nodes[NodeType::TileMap] = "MapLayer2D";
		nodes[NodeType::Scene2D] = "Scene2D";
		nodes[NodeType::Text] = "Text2D";
		nodes[NodeType::Button] = "Button2D";
		nodes[NodeType::TextureButton] = "TextureButton2D";
		nodes[NodeType::Audio] = "Audio2D";

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
