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
}

