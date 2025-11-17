#pragma once
#include <Scripting/ScriptManager.hpp>

namespace ag
{
	class LuaEnv
	{
	public:
		LuaEnv()
		{
			auto& lua = ScriptManager::get_lua();
			m_env = sol::environment(lua, sol::create, lua.globals());
		}

		sol::environment& get() { return m_env; }
	private:
		sol::environment m_env;
	};
}