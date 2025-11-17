#include <Scripting/ScriptManager.hpp>
#include <Scripting/LuaEnv.hpp>
#include <Scripting/ScriptBinding/RegisterFunction.hpp>
#include <Math/Math.hpp>


namespace ag
{
	sol::state ScriptManager::s_lua_state;

	void ScriptManager::init()
	{
		AERO_CORE_INFO("Initializing Lua");

		s_lua_state.open_libraries(
			sol::lib::base,
			sol::lib::math,
			sol::lib::table,
			sol::lib::string,
			sol::lib::os
		);

		s_lua_state.set_function("aero_print", [](const std::string& msg) {
			AERO_CORE_INFO("[Lua] {0}", msg);
			});

    RegisterFunction::init();

		AERO_CORE_INFO("Lua Initialized");
	}

	void ScriptManager::shut_down()
	{
		AERO_CORE_INFO("Shutting Down Lua");
		s_lua_state = sol::state();
	}

	void ScriptManager::load_script(const std::string& path)
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

	void ScriptManager::load_script(const std::string& path, LuaEnv& env)
	{
		auto& e = env.get();

		sol::load_result script = s_lua_state.load_file(path);

		if (!script.valid())
		{
			sol::error err = script;
			AERO_CORE_ERROR("[Lua Error] {0}", err.what());
			return;
		}

		sol::protected_function_result result = script(e);

		if (!result.valid())
		{
			sol::error err = result;
			AERO_CORE_ERROR("[Lua RunTime Error] {0}", err.what());
			return;
		}
	}

	void ScriptManager::run_script(const std::string& script)
	{
		s_lua_state.script(script);
	}
}
