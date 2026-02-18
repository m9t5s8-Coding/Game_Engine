#include <Scene/Scene.hpp>
#include <Scripting/ScriptBinding/Signals.hpp>
#include <sol/forward.hpp>

namespace ag
{

void Signals::connect(sol::function fn, const AG_ref<Scene>& scene)
{
    m_connections.push_back({fn, scene, false});

    if (m_has_value && fn.valid()) fn(m_stored_value);
}

void Signals::connect_once(sol::function fn, const AG_ref<Scene>& scene)
{
    m_connections.push_back({fn, scene, true});

    if (m_has_value && fn.valid()) fn(m_stored_value);
}

void Signals::fire(sol::variadic_args args)
{
    for (auto& c : m_connections)
        if (c.callback.valid()) c.callback(args);

    clean_up();
}

void Signals::store_value(sol::object value)
{
    m_stored_value = value;
    m_has_value = true;
}

void Signals::clear_value()
{
    m_stored_value = sol::object{};
    m_has_value = false;
}

void Signals::disconnect_all() { m_connections.clear(); }

void Signals::disconnect_scene(const AG_ref<Scene>& scene)
{
    m_connections.erase(
        std::remove_if(m_connections.begin(), m_connections.end(),
                       [&scene](const Connection& c)
                       {
                           if (c.scene.expired()) return true;
                           return c.scene.lock() == scene;
                       }),
        m_connections.end());
}

void Signals::clean_up()
{
    m_connections.erase(
        std::remove_if(m_connections.begin(), m_connections.end(),
                       [](const Connection& c) { return c.once; }),
        m_connections.end());
}

}  // namespace ag
