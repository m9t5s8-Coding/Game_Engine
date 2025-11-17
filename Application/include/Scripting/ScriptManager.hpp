#pragma once

extern "C"
{
#include <lua.h>
}
#include <sol/sol.hpp>
#include <iostream>
#include <string>

namespace ag
{
	class LuaEnv;
	class ScriptManager
	{
	public:
		static void init();
		static void shut_down();

		static void load_script(const std::string& path);
		static void load_script(const std::string& path, LuaEnv& env);

		static void run_script(const std::string& script);

		static sol::state& get_lua() { return s_lua_state; }

	private:
		static sol::state s_lua_state;
	};
	
}