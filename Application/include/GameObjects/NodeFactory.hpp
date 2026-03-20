#pragma once

#include <functional>
#include <nlohmann/json.hpp>
#include <Scene/Entity.hpp>
#include <string>
#include <unordered_map>

using json = nlohmann::json;

namespace ag
{
class NodeFactory
{
public:
  static std::unordered_map<NodeType, std::function<void(Entity)>>            create_map;
  static std::unordered_map<NodeType, std::function<void(Entity)>>            draw_map;
  static std::unordered_map<NodeType, std::function<void(Entity, TimeStamp)>> update_map;
  static std::unordered_map<NodeType, std::function<void(Entity)>>            properties_map;
  static std::unordered_map<NodeType, std::function<void(Entity, Entity)>>    clone_map;
  static std::unordered_map<NodeType, std::function<json(Entity)>>            save_map;
  static std::unordered_map<NodeType, std::function<void(Entity, json&)>>     load_map;
  static std::unordered_map<NodeType, std::function<void(Entity)>>            clear_map;
  static std::unordered_map<NodeType, std::string>                            nodes;

  static void init();
  static void shut_down();
};
}  // namespace ag
