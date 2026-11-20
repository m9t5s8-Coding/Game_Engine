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
  LuaFunc on_draw;
  LuaFunc on_pause;
  LuaFunc on_resume;

  static void create(GlobalScript_Component& comps);
  static void update(GlobalScript_Component& comps, TimeStamp ts);
  static void draw(GlobalScript_Component& comps);
  static void pause(GlobalScript_Component& comps);
  static void resume(GlobalScript_Component& comps);
};
class GlobalScriptsManager
{
public:
  void on_update(TimeStamp ts);
  void on_draw();
  void on_create();
  void on_pause();
  void on_resume();
  void load_scripts();
  void on_destroy();

private:
  std::vector<GlobalScript_Component> m_global_scripts;
};
}  // namespace ag
