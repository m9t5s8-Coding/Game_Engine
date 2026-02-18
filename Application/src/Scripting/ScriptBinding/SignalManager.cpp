#include <Scene/Scene.hpp>
#include <Scripting/ScriptBinding/SignalManager.hpp>
#include <Scripting/ScriptManager.hpp>

#include "Core/Log.hpp"
#include "Scripting/ScriptBinding/Signals.hpp"

namespace ag
{

Signal_Manager* Signal_Manager::s_instance = nullptr;

AG_ref<Signals> Signal_Manager::get_signals(const std::string& name)
{
    if (!s_instance) std::cout << "NO Instance Created" << std::endl;

    AG_uint id = get_id(name);

    auto it = s_instance->m_signals.find(id);
    if (it != s_instance->m_signals.end()) return it->second;

    auto sig = std::make_shared<Signals>();
    s_instance->m_signals[id] = sig;
    return sig;
}

AG_ref<Signals> Signal_Manager::get_signals(AG_uint id)
{
    auto it = s_instance->m_signals.find(id);
    if (it != s_instance->m_signals.end()) return it->second;
    return nullptr;
}

AG_uint Signal_Manager::get_id(const std::string& name)
{
    auto it = s_instance->m_name_to_id.find(name);
    if (it != s_instance->m_name_to_id.end()) return it->second;

    AG_uint id = create_id(name);
    s_instance->m_name_to_id[name] = id;
    return id;
}

AG_uint Signal_Manager::create_id(const std::string& name)
{
    AG_uint hash = 0x811c9dc5;
    for (char c : name)
    {
        hash ^= static_cast<AG_uint>(c);
        hash *= 0x01000193;
    }
    return hash;
}

void Signal_Manager::bind_signals()
{
    auto& lua = ScriptManager::get_lua();

    lua.new_usertype<Signals>(
        "signal", "connect", [](Signals& s, sol::function fn)
        { s.connect(fn, Scene::get_active_scene()); }, "connect_once",
        [](Signals& s, sol::function fn) { s.connect_once(fn, Scene::get_active_scene()); }, "fire",
        [](Signals& s, sol::variadic_args args) { s.fire(args); }, "store_value",
        [](Signals& s, sol::object val) { s.store_value(val); }, "get_value", [](Signals& s)
        { return s.get_value(); }, "disconnect_all", [](Signals& s) { s.disconnect_all(); });

    sol::table sig_table = lua.create_named_table("signals");

    sig_table.set_function(
        "get", [](const std::string& name) { return Signal_Manager::get_signals(name); });

    sig_table.set_function(
        "get_id", [](const std::string& name) -> AG_uint { return Signal_Manager::get_id(name); });
}

}  // namespace ag
