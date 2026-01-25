#pragma once

#include <string>
#include <Scene/SceneComponent.hpp>

namespace ag::NodeHelper
{
	NodeType get_nodetype(Entity entity);

	AG_ref<Texture2D> create_texture(const std::string& path);

	AG_ref<Texture2D> load_texture(std::string& path);


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

	template<typename C, typename V, typename S>
	inline static void set_value(ag::Entity& entity, V C::* member_ptr, S& value)
	{
		if (entity.has_component<C>())
		{
			auto& component = entity.get_component<C>();
			value = component.*member_ptr;
		}
	}

	template<typename T>
	void save_component(Entity entity, json& j)
	{
		if (entity.has_component<T>())
		{
			j[T::get_name()] = T::save_json(entity);
		}
	}

	template<typename T>
	void load_component(Entity entity, const json& j)
	{
		if (j.contains(T::get_name()))
		{
			T::load_json(entity, j.at(T::get_name()));
		}
	}

	inline Node_Capability operator|(Node_Capability a, Node_Capability b)
	{
		return (Node_Capability)((AG_uint)a | (AG_uint)b);
	}

	bool has_capability(Node_Capability a, Node_Capability b);

	Node_Capability get_node_capabilities(NodeType type);
}