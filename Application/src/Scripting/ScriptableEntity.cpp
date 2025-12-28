#include <Scripting/ScriptableEntity.hpp>
#include <Scene/SceneComponent.hpp>
#include <GameObjects/Node.hpp>

namespace ag
{
	sol::object ScriptableEntity::get(sol::this_state s)
	{
		sol::state_view lua(s);
		const auto& type = m_entity.get_component<Tag>().node_type;

		switch (type)
		{
		case NodeType::Rectangle:
		{
			sol::table rect_table = lua.create_table();
			auto& transform = m_entity.get_component<Transform>();

			return rect_table;
		}
		case NodeType::Circle:
		{
			sol::table circle_table = lua.create_table();
			return circle_table;
		}
		default:
			return sol::nil;
			break;
		}
	}
}