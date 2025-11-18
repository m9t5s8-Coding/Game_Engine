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

		template<typename T>
		void set(const std::string& key, T&& value) {
			m_env[key] = std::forward<T>(value);
		}

	private:
		sol::environment m_env;
	};
}