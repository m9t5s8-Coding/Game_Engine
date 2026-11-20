#include <Project/Assetmanager.hpp>
#include <Project/GlobalScripts.hpp>
#include <Project/Project.hpp>
#include <Scripting/ScriptHooks.hpp>

namespace ag {

void GlobalScript_Component::create(GlobalScript_Component& comps) {
  if (comps.on_create.is_valid()) {
    comps.on_create.call();
  }
}

void GlobalScript_Component::update(GlobalScript_Component& comps, TimeStamp ts) {
  if (comps.on_update.is_valid()) {
    comps.on_update.call(ts.get_seconds());
  }
}

void GlobalScript_Component::draw(GlobalScript_Component& comps) {
  if (comps.on_draw.is_valid()) {
    comps.on_draw.call();
  }
}

void GlobalScript_Component::pause(GlobalScript_Component& comps) {
  if (comps.on_pause.is_valid()) {
    comps.on_pause.call();
  }
}

void GlobalScript_Component::resume(GlobalScript_Component& comps) {
  if (comps.on_resume.is_valid()) {
    comps.on_resume.call();
  }
}

void GlobalScriptsManager::on_destroy() {
  for (auto& comps : m_global_scripts) {
    comps.on_update = LuaFunc();
    comps.on_draw   = LuaFunc();
    comps.on_create = LuaFunc();
    comps.on_pause  = LuaFunc();
    comps.on_resume = LuaFunc();
    comps.env.get().clear();
    comps.env.invalidate();
  }
  m_global_scripts.clear();
}

void GlobalScriptsManager::load_scripts() {
#ifdef AERO_EDITOR
  return;
#endif

  auto  project        = Project::get_active_project();
  auto& global_scripts = project->get_global_scripts();

  for (auto& script_path : global_scripts.global_scripts) {
    std::string full_path =
        project->get_directory() + project->get_scripts_directory() + script_path;

    GlobalScript_Component script;
    if (AssetManager::is_packed(AssetManager::Domain::Project)) {
      std::string code = AssetManager::read_string(full_path, AssetManager::Domain::Project);
      if (code.empty()) {
        AERO_CORE_ERROR("Failed to load script from pak: {0}", full_path);
        return;
      }

      try {
        ScriptManager::run_script_in_env(code, script.env);
      } catch (const sol::error& e) {
        AERO_CORE_ERROR("Script error in {0}: {1}", full_path, e.what());
        return;
      }
    } else {
      ScriptManager::load_script(full_path, script.env);
    }

    script.on_create.set_function(script.env, script_hooks::OnCreate);
    script.on_update.set_function(script.env, script_hooks::OnUpdate);
    script.on_draw.set_function(script.env, script_hooks::OnDraw);
    script.on_pause.set_function(script.env, script_hooks::OnPause);
    script.on_resume.set_function(script.env, script_hooks::OnResume);

    m_global_scripts.push_back(script);
  }
}

void GlobalScriptsManager::on_create() {
  for (auto& comps : m_global_scripts) {
    GlobalScript_Component::create(comps);
  }
}

void GlobalScriptsManager::on_update(TimeStamp ts) {
  for (auto& comps : m_global_scripts) {
    GlobalScript_Component::update(comps, ts);
  }
}

void GlobalScriptsManager::on_draw() {
  for (auto& comps : m_global_scripts) {
    GlobalScript_Component::draw(comps);
  }
}

void GlobalScriptsManager::on_pause() {
  for (auto& comps : m_global_scripts) {
    GlobalScript_Component::pause(comps);
  }
}

void GlobalScriptsManager::on_resume() {
  for (auto& comps : m_global_scripts) {
    GlobalScript_Component::resume(comps);
  }
}
}  // namespace ag
