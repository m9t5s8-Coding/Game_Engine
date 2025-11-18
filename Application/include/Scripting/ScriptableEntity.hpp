#pragma once

#include <Scene/Entity.hpp>
#include <Scripting/ScriptManager.hpp>

namespace ag
{
	class ScriptableEntity
	{
	public:
		Entity m_entity;

		ScriptableEntity(Entity entity) : m_entity(entity) {}

		sol::object get(sol::this_state s);

	private:
		
	};
}