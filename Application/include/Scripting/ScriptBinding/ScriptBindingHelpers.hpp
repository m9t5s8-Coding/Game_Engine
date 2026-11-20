#pragma once

#include <GameObjects/Components/Components.hpp>
#include <GameObjects/GameObjects.hpp>
#include <Scene/Entity.hpp>

namespace ag::detail {

template <typename Component, typename MemberType>
MemberType safe_get_comp_value(Entity&              entity,
                               MemberType Component::* member,
                               const MemberType&     default_value) {
  return NodeHelper::get_comp_value(entity, member, default_value);
}

template <typename Component, typename MemberType>
void safe_set_comp_value(Entity&              entity,
                         MemberType Component::* member,
                         const MemberType&     value) {
  NodeHelper::set_comp_value(entity, member, value);
}

template <typename Component>
bool has_component(Entity& entity) {
  return entity.has_component<Component>();
}

}  // namespace ag::detail
