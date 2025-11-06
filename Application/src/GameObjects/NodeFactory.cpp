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
	std::unordered_map<NodeType, std::string> NodeFactory::nodes;

	void NodeFactory::init()
	{
		create_map[NodeType::Rectangle] = RectangleNode::create_node;
		create_map[NodeType::Circle] = CircleNode::create_node;
		create_map[NodeType::Sprite] = SpriteNode::create_node;
		create_map[NodeType::AnimatedSprite2D] = AnimatedSprite2DNode::create_node;
		create_map[NodeType::Camera] = CameraComponent::create_node;


		draw_map[NodeType::Rectangle] = RectangleNode::draw;
		draw_map[NodeType::Circle] = CircleNode::draw;
		draw_map[NodeType::Sprite] = SpriteNode::draw;
		draw_map[NodeType::AnimatedSprite2D] = AnimatedSprite2DNode::draw;
		draw_map[NodeType::Camera] = CameraComponent::draw;


		properties_map[NodeType::Rectangle] = RectangleNode::show_properties;
		properties_map[NodeType::Circle] = CircleNode::show_properties;
		properties_map[NodeType::Sprite] = SpriteNode::show_properties;
		properties_map[NodeType::AnimatedSprite2D] = AnimatedSprite2DNode::show_properties;
		properties_map[NodeType::Camera] = CameraComponent::show_properties;



		clone_map[NodeType::Rectangle] = RectangleNode::clone_node;
		clone_map[NodeType::Circle] = CircleNode::clone_node;
		clone_map[NodeType::Sprite] = SpriteNode::clone_node;
		clone_map[NodeType::AnimatedSprite2D] = AnimatedSprite2DNode::clone_node;
		clone_map[NodeType::Camera] = CameraComponent::clone_node;

		save_map[NodeType::Rectangle] = RectangleNode::save_json;
		save_map[NodeType::Circle] = CircleNode::save_json;
		save_map[NodeType::Sprite] = SpriteNode::save_json;
		save_map[NodeType::AnimatedSprite2D] = AnimatedSprite2DNode::save_json;
		save_map[NodeType::Camera] = CameraComponent::save_json;

		load_map[NodeType::Rectangle] = RectangleNode::load_json;
		load_map[NodeType::Circle] = CircleNode::load_json;
		load_map[NodeType::Sprite] = SpriteNode::load_json;
		load_map[NodeType::AnimatedSprite2D] = AnimatedSprite2DNode::load_json;
		load_map[NodeType::Camera] = CameraComponent::load_json;

		nodes[NodeType::Rectangle] = "Rectangle";
		nodes[NodeType::Circle] = "Circle";
		nodes[NodeType::Sprite] = "Sprite";
		nodes[NodeType::AnimatedSprite2D] = "AnimatedSprite2D";
		nodes[NodeType::Camera] = "CameraComponent";
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
