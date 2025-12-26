#pragma once

#include <string>
#include <Scene/SceneComponent.hpp>

namespace ag::NodeHelper
{
	inline static NodeType get_nodetype(Entity entity)
	{
		if (entity.has_component<Tag>())
		{
			return entity.get_component<Tag>().node_type;
		}
    return NodeType::None;
	}

	//C = Component R = ReturnType  D = DefaultValue
	template< typename C, typename R, typename D = R>
	inline static R get_comp_value(ag::Entity& entity, R C::* member_ptr, D default_value = D{})
	{
		if (entity.has_component<C>())
		{
			return entity.get_component<C>().*member_ptr;
		}
		return default_value;
	}

	//C = Component  V = ValueType
	template <typename C, typename V>
	inline static void set_comp_value(ag::Entity& entity, V C::* member_ptr, const V& value)
	{
		if (entity.has_component<C>())
		{
			entity.get_component<C>().*member_ptr = value;
		}
	}

	template<typename C, typename V>
	inline static void set_value(ag::Entity& entity, V C::* member_ptr, V& value)
	{
		if (entity.has_component<C>())
		{
			const auto& component = entity.get_component<C>();
			value = component.*member_ptr;
		}
	}

	template<typename T>
	void save_component(Entity entity, json& j, const std::string& key)
	{
		if (entity.has_component<T>())
		{
			j[key] = T::save_json(entity);
		}
	}

	template<typename T>
	void load_component(Entity entity, const json& j, const std::string& key)
	{
		if (j.contains(key))
		{
			T::load_json(entity, j[key]);
		}
	}

	inline Node_Capability operator|(Node_Capability a, Node_Capability b)
	{
		return (Node_Capability)((AG_uint)a | (AG_uint)b);
	}

	inline bool has_capability(Node_Capability a, Node_Capability b)
	{
		return ((AG_uint)a & (AG_uint)b) != 0;
	}

	inline Node_Capability get_node_capabilities(NodeType type)
	{
		switch (type)
		{
		case ag::NodeType::None:
			return Node_Capability::None;

		case ag::NodeType::Rectangle:
			return Node_Capability::RectShape |
				Node_Capability::Render2D |
				Node_Capability::UI;

		case ag::NodeType::Circle:
			return Node_Capability::CircleShape | 
				Node_Capability::Render2D | 
				Node_Capability::UI;

		case ag::NodeType::Sprite:
		case ag::NodeType::AnimatedSprite2D:
			return Node_Capability::Render2D;

		case ag::NodeType::Camera:
		case ag::NodeType::TileMap:
		case ag::NodeType::Scene2D:
		case ag::NodeType::TextNode:
		case ag::NodeType::Button:
		case ag::NodeType::TextureButton:
			return Node_Capability::None;

		case ag::NodeType::CollisionShape:
			return Node_Capability::Physics2D;
		default:
			break;
		}
	}
}

