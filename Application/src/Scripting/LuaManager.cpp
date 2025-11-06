#include <Scripting/LuaManager.hpp>
#include <Core/Core.hpp>

namespace ag
{
	sol::state LuaManager::s_lua_state;
	void LuaManager::init()
	{
		AERO_CORE_INFO("Initializing Lua");

		s_lua_state.open_libraries(
			sol::lib::base,
			sol::lib::math,
			sol::lib::table,
			sol::lib::string,
			sol::lib::os
		);

		s_lua_state.set_function("cpp_print", [](const std::string& msg) {
			AERO_CORE_INFO("[Lua] {0}", msg);
			});

		AERO_CORE_INFO("Lua Initialized");
	}

	void LuaManager::shut_down()
	{
		AERO_CORE_INFO("Shutting Down Lua");
		s_lua_state = sol::state();
	}

	void LuaManager::load_script(const std::string& path)
	{
		std::ifstream file(path);
		if (!file.is_open())
		{
			AERO_CORE_INFO("Failed to open file: {0}", path);
			return;
		}
		std::stringstream buffer;
		buffer << file.rdbuf();

		sol::load_result script = s_lua_state.load(buffer.str());
		if (!script.valid())
		{
			sol::error err = script;
			AERO_CORE_INFO("Lua error: {0}", err.what());
			return;
		}

		try {
			script();
		}
		catch (const sol::error& err) {
			AERO_CORE_INFO("Runtime Lua error: {0}", err.what());
		}

	}

	void LuaManager::run_script(const std::string& code)
	{
		s_lua_state.script(code);
	}
}