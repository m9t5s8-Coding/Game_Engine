#include <GameObjects/NodeFactory.hpp>
#include <GameObjects/Node.hpp>

namespace ag
{
	std::unordered_map<NodeType, std::function<void(Entity)>> NodeFactory::create_map;
	std::unordered_map<NodeType, std::function<void(Entity, TimeStamp)>> NodeFactory::draw_map;
	std::unordered_map<NodeType, std::function<void(Entity)>> NodeFactory::properties_map;
	std::unordered_map<NodeType, std::string> NodeFactory::nodes;

	void NodeFactory::init()
	{
		create_map[NodeType::Rectangle] = RectangleNode::create_node;
		create_map[NodeType::Circle] = CircleNode::create_node;
		create_map[NodeType::Sprite] = SpriteNode::create_node;
		create_map[NodeType::AnimatedSprite2D] = AnimatedSprite2DNode::create_node;


		draw_map[NodeType::Rectangle] = RectangleNode::draw;
		draw_map[NodeType::Circle] = CircleNode::draw;
		draw_map[NodeType::Sprite] = SpriteNode::draw;
		draw_map[NodeType::AnimatedSprite2D] = AnimatedSprite2DNode::draw;


		properties_map[NodeType::Rectangle] = RectangleNode::show_properties;
		properties_map[NodeType::Circle] = CircleNode::show_properties;
		properties_map[NodeType::Sprite] = SpriteNode::show_properties;
		properties_map[NodeType::AnimatedSprite2D] = AnimatedSprite2DNode::show_properties;

		nodes[NodeType::Rectangle] = "Rectangle";
		nodes[NodeType::Circle] = "Circle";
		nodes[NodeType::Sprite] = "Sprite";
		nodes[NodeType::AnimatedSprite2D] = "AnimatedSprite2D";
	}
}
