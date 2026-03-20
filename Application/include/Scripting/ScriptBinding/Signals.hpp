#pragma once

#include <Core/Core.hpp>
#include <memory>
#include <sol/sol.hpp>
#include <vector>
namespace ag
{
class Scene;
class Signals
{
  struct Connection
  {
    sol::function        callback;
    std::weak_ptr<Scene> scene;
    bool                 once = false;
  };

  std::vector<Connection> m_connections;
  sol::object             m_stored_value;
  bool                    m_has_value = false;

public:
  void        connect(sol::function fn, const AG_ref<Scene>& scene);
  void        connect_once(sol::function fn, const AG_ref<Scene>& scene);
  void        fire(sol::variadic_args args);
  void        store_value(sol::object value);
  sol::object get_value()
  {
    return m_stored_value;
  };
  void clear_value();
  void disconnect_all();
  void disconnect_scene(const AG_ref<Scene>& scene);

private:
  void clean_up();
};
}  // namespace ag
