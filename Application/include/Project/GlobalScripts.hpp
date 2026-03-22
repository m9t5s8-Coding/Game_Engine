#pragma once

#include <Core/Core.hpp>
#include <Core/TimeStamp.hpp>
#include <Scripting/LuaEnv.hpp>
#include <Scripting/LuaFunc.hpp>
#include <vector>

namespace ag
{

struct GlobalScript_Component
{
  LuaEnv  env;
  LuaFunc on_create;
  LuaFunc on_update;

  static void create(GlobalScript_Component& comps);
  static void update(GlobalScript_Component& comps, TimeStamp ts);
};
class GlobalScriptsManager
{
public:
  void on_update(TimeStamp ts);
  void on_create();
  void load_scripts();
  void on_destroy();

private:
  std::vector<GlobalScript_Component> m_global_scripts;
};
}  // namespace ag
