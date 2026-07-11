#pragma once

extern "C"
{
#include "lua.h"
}
#include <iostream>
#include <sol/sol.hpp>
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
  static void run_script_in_env(const std::string& code, LuaEnv& env);

  static void run_script(const std::string& script);

  static sol::state& get_lua()
  {
    return s_lua_state;
  }

  template <typename... Args>
  static void call(const std::string& fn, Args&&... args)
  {
    sol::protected_function f = s_lua_state[fn];
    if (!f.valid())
      return;

    auto result = f(std::forward<Args>(args)...);
    if (!result.valid())
    {
      sol::error err = result;
      AERO_CORE_ERROR("Lua Error in {0}: {1}", fn, err.what());
    }
  }

private:
  static sol::state s_lua_state;
};

}  // namespace ag
