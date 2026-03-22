#include <Project/Assetmanager.hpp>
#include <Project/GlobalScripts.hpp>
#include <Project/Project.hpp>

namespace ag
{

void GlobalScript_Component::create(GlobalScript_Component& comps)
{
  if (comps.on_create.is_valid())
  {
    comps.on_create.call();
  }
}

void GlobalScript_Component::update(GlobalScript_Component& comps, TimeStamp ts)
{
  if (comps.on_update.is_valid())
  {
    comps.on_update.call(ts.get_seconds());
  }
}

void GlobalScriptsManager::on_destroy()
{
  for (auto& comps : m_global_scripts)
  {
    comps.on_update = LuaFunc();
    comps.on_create = LuaFunc();
    comps.env.get().clear();
    comps.env.invalidate();
  }
  m_global_scripts.clear();
}

void GlobalScriptsManager::load_scripts()
{
  if (!Engine::is_runtime())
    return;

  auto  project        = Project::get_active_project();
  auto& global_scripts = project->get_global_scripts();

  for (auto& script_path : global_scripts.global_scripts)
  {
    std::string full_path =
        project->get_directory() + project->get_scripts_directory() + script_path;

    GlobalScript_Component script;
    if (AssetManager::is_packed(AssetManager::Domain::Project))
    {
      std::string code = AssetManager::read_string(full_path, AssetManager::Domain::Project);
      if (code.empty())
      {
        AERO_CORE_ERROR("Failed to load script from pak: {0}", full_path);
        return;
      }

      try
      {
        ScriptManager::run_script_in_env(code, script.env);
      }
      catch (const sol::error& e)
      {
        AERO_CORE_ERROR("Script error in {0}: {1}", full_path, e.what());
        return;
      }
    }
    else
    {
      ScriptManager::load_script(full_path, script.env);
    }

    script.on_create.set_function(script.env, "on_create");
    script.on_update.set_function(script.env, "on_update");

    m_global_scripts.push_back(script);
  }
}

void GlobalScriptsManager::on_create()
{
  for (auto& comps : m_global_scripts)
  {
    GlobalScript_Component::create(comps);
  }
}

void GlobalScriptsManager::on_update(TimeStamp ts)
{
  for (auto& comps : m_global_scripts)
  {
    GlobalScript_Component::update(comps, ts);
  }
}
}  // namespace ag
