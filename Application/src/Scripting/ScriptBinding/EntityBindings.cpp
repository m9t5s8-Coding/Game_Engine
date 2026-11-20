#include <Aero.hpp>
#include <Scripting/ScriptBinding/ScriptBinding.hpp>
#include <Scripting/ScriptBinding/ScriptBindingHelpers.hpp>

namespace ag {

void ScriptBinding::register_node() {
  auto& lua         = ScriptManager::get_lua();
  auto  entity_type = lua.new_usertype<Entity>("Entity");  // TAG COMPONENT FUNCTIONS
  // auto entity_type = lua["Entity"].get<sol::usertype<Entity>>();

  entity_type.set_function("get_children",
                           [](Entity& entity, const std::string& name) -> ag::Entity {
                             if (!entity.has_component<Tag_Component>())
                               return {};

                             auto& tag = entity.get_component<Tag_Component>();
                             for (auto& child : tag.children) {
                               auto& child_tag = child.get_component<Tag_Component>();
                               if (child_tag.name == name) {
                                 return child;
                               }
                             }
                             return {};
                           });

  entity_type.set_function("get_parent", [](Entity& entity) -> ag::Entity {
    return detail::safe_get_comp_value(entity, &Tag_Component::parent, Entity());
  });

  // TAG COMPONENT

  entity_type.set_function("get_name", [](Entity& entity) -> std::string {
    return NodeHelper::get_comp_value(entity, &Tag_Component::name, "default");
  });

  entity_type.set_function("set_name", [](Entity& entity, const std::string& name) {
    detail::safe_set_comp_value(entity, &Tag_Component::name, name);
  });

  entity_type.set_function("is_visible", [](Entity& entity) -> bool {
    return detail::safe_get_comp_value(entity, &Tag_Component::visible, true);
  });

  entity_type.set_function("set_visible", [](Entity& entity, bool visible) {
    detail::safe_set_comp_value(entity, &Tag_Component::visible, visible);
  });

  entity_type.set_function("is_locked", [](ag::Entity& entity) -> bool {
    return detail::safe_get_comp_value(entity, &Tag_Component::locked, false);
  });

  entity_type.set_function("set_lock", [](ag::Entity& entity, bool locked) {
    detail::safe_set_comp_value(entity, &Tag_Component::locked, locked);
  });

  // TRANSFORM COMPONENT FUNCTIONS

  // Position
  entity_type.set_function("get_position", [](Entity& entity) -> vec2f {
    return detail::safe_get_comp_value(entity, &Transform_Component::position, vec2f(0, 0));
  });

  entity_type.set_function("set_position", [](Entity& entity, const vec2f& position) {
    detail::safe_set_comp_value(entity, &Transform_Component::position, position);
  });

  entity_type.set_function("move", [](Entity& entity, const vec2f& delta) {
    auto position =
        detail::safe_get_comp_value(entity, &Transform_Component::position, vec2f(0, 0));
    position += delta;
    detail::safe_set_comp_value(entity, &Transform_Component::position, position);
  });

  // Scale
  entity_type.set_function("get_scale", [](Entity& entity) -> vec2f {
    return detail::safe_get_comp_value(entity, &Transform_Component::scale, vec2f(1, 1));
  });

  entity_type.set_function("set_scale", [](Entity& entity, const vec2f& scale) {
    detail::safe_set_comp_value(entity, &Transform_Component::scale, scale);
  });

  // Rotation
  entity_type.set_function("get_rotation", [](Entity& entity) -> float {
    return detail::safe_get_comp_value(entity, &Transform_Component::rotation, 0.0f);
  });

  entity_type.set_function("set_rotation", [](Entity& entity, float rotation) {
    detail::safe_set_comp_value(entity, &Transform_Component::rotation, rotation);
  });

  entity_type.set_function("rotate", [](Entity& entity, float delta) {
    auto rotation = detail::safe_get_comp_value(entity, &Transform_Component::rotation, 0.0f);
    rotation += delta;
    detail::safe_set_comp_value(entity, &Transform_Component::rotation, rotation);
  });

  // Render2D Component
  // Fill color
  entity_type.set_function("get_fill_color", [](Entity& entity) -> Color {
    return detail::safe_get_comp_value(entity, &Render2D_Component::color, Color::Transparent);
  });

  entity_type.set_function("set_fill_color", [](Entity& entity, const Color& color) {
    detail::safe_set_comp_value(entity, &Render2D_Component::color, color);
  });

  // Size
  entity_type.set_function("get_size", [](Entity& entity) -> vec2f {
    auto size = detail::safe_get_comp_value(entity, &Render2D_Component::size, vec2u(0, 0));
    return vec2f(size);
  });

  entity_type.set_function("set_size", [](Entity& entity, const vec2f& size) {
    detail::safe_set_comp_value(entity, &Render2D_Component::size, vec2u(size));
  });

  // Border Component
  entity_type.set_function("get_border_color", [](Entity& entity) -> Color {
    return detail::safe_get_comp_value(entity, &Border_Component::color, Color::Transparent);
  });

  entity_type.set_function("set_border_color", [](Entity& entity, const Color& color) {
    detail::safe_set_comp_value(entity, &Border_Component::color, color);
  });

  entity_type.set_function("get_border_thickness", [](Entity& entity) -> float {
    return detail::safe_get_comp_value(entity, &Border_Component::thickness, 0.0f);
  });

  entity_type.set_function("set_border_thickness", [](Entity& entity, float thickness) {
    detail::safe_set_comp_value(entity, &Border_Component::thickness, thickness);
  });

  // Corner Component
  entity_type.set_function("set_corner_radius", [](Entity& entity, float corner) {
    detail::safe_set_comp_value(entity, &Corner_Component::corner, corner);
  });

  entity_type.set_function("get_corner_radius", [](Entity& entity) -> float {
    return detail::safe_get_comp_value(entity, &Corner_Component::corner, 0.0f);
  });

  // UI Component
  lua.new_enum<RenderMode>("RenderMode",
                           {
                               {"Screen", RenderMode::Screen},
                               { "World",  RenderMode::World}
  });

  entity_type.set_function("get_render_mode", [](Entity& entity) -> RenderMode {
    return detail::safe_get_comp_value(entity, &UI_Component::mode, RenderMode::World);
  });

  entity_type.set_function("set_render_mode", [](Entity& entity, RenderMode mode) {
    detail::safe_set_comp_value(entity, &UI_Component::mode, mode);
  });

  entity_type.set_function("play_animation", [](Entity& entity, const std::string& name) -> bool {
    return Animation_Component::play_animation(entity, name, false);
  });

  entity_type.set_function("play_animation_restart",
                           [](Entity& entity, const std::string& name) -> bool {
                             return Animation_Component::play_animation(entity, name, true);
                           });

  entity_type.set_function("is_animation_complete", [](Entity& entity) -> bool {
    if (detail::has_component<Animation_Component>(entity)) {
      return entity.get_component<Animation_Component>().current_animation_completed;
    }
    return false;
  });

  entity_type.set_function("get_current_animation", [](Entity& entity) -> std::string {
    if (detail::has_component<Animation_Component>(entity)) {
      return entity.get_component<Animation_Component>().current_animation;
    }
    return "";
  });

  entity_type.set_function("flip_vertical", [](Entity& entity, bool vertical) {
    detail::safe_set_comp_value(entity, &TextureFlip_Component::vertical, vertical);
  });

  entity_type.set_function("flip_horizontal", [](Entity& entity, bool horizontal) {
    detail::safe_set_comp_value(entity, &TextureFlip_Component::horizontal, horizontal);
  });

  entity_type.set_function("duplicate_entity", [](Entity& entity) -> Entity {
    auto& parent = entity.get_component<Tag_Component>().parent;
    return Scene::get_active_scene()->duplicate_entity(entity, parent);
  });

  entity_type.set_function("delete_entity", [](Entity& entity) {
    Scene::get_active_scene()->destroy_entity(entity);
  });

  entity_type.set_function("set_view_size", [](Entity& entity, const vec2f& view_size) {
    detail::safe_set_comp_value(entity, &Camera_Component::size, view_size);
  });

  entity_type.set_function("get_view_size", [](Entity& entity) -> vec2f {
    return detail::safe_get_comp_value(entity, &Camera_Component::size, vec2f(1280, 720));
  });

  entity_type.set_function("get_view_center", [](Entity& entity) -> vec2f {
    return detail::safe_get_comp_value(entity, &Camera_Component::center, vec2f(640, 360));
  });

  entity_type.set_function("set_view_center", [](Entity& entity, const vec2f& view_center) {
    detail::safe_set_comp_value(entity, &Camera_Component::center, view_center);
  });

  entity_type.set_function("move_view", [](Entity& entity, const vec2f& offset) {
    auto view_center = detail::safe_get_comp_value(entity, &Camera_Component::center, vec2f(0, 0));
    detail::safe_set_comp_value(entity, &Camera_Component::center, view_center + offset);
  });

  // TEXT COMPONENT FUNCTIONS

  entity_type.set_function("set_text", [](Entity& entity, const std::string& value) {
    detail::safe_set_comp_value(entity, &Text_Component::text, value);
  });

  entity_type.set_function("get_text", [](Entity& entity) -> std::string {
    return detail::safe_get_comp_value(entity, &Text_Component::text, std::string(""));
  });

  // TWEEN COMPONENT FUNCTIONS

  lua.new_enum<Tween_Component::EaseType>(
      "EaseType",
      {
          {     "LINEAR",      Tween_Component::EaseType::LINEAR},
          {    "EASE_IN",     Tween_Component::EaseType::EASE_IN},
          {   "EASE_OUT",    Tween_Component::EaseType::EASE_OUT},
          {"EASE_IN_OUT", Tween_Component::EaseType::EASE_IN_OUT}
  });

  lua.new_enum<Tween_Component::LoopType>("LoopType",
                                          {
                                              {     "ONCE",      Tween_Component::LoopType::ONCE},
                                              {     "LOOP",      Tween_Component::LoopType::LOOP},
                                              {"PING_PONG", Tween_Component::LoopType::PING_PONG}
  });

  lua.new_enum<Tween_Component::TweenTarget>(
      "TweenTarget",
      {
          {"POSITION", Tween_Component::TweenTarget::POSITION},
          {   "SCALE",    Tween_Component::TweenTarget::SCALE},
          {"ROTATION", Tween_Component::TweenTarget::ROTATION}
  });

  entity_type.set_function("play_tween", [](Entity& entity) -> bool {
    return Tween_Component::play_tween(entity);
  });

  entity_type.set_function("set_tween_start", [](Entity& entity, const vec2f& position) {
    detail::safe_set_comp_value(entity, &Tween_Component::start_position, position);
  });

  entity_type.set_function("set_tween_position",
                           [](Entity&                   entity,
                              const vec2f&              start,
                              const vec2f&              end,
                              float                     duration,
                              Tween_Component::EaseType type,
                              Tween_Component::LoopType loop) {
                             if (!entity.has_component<Tween_Component>())
                               entity.add_component<Tween_Component>();

                             auto& tween          = entity.get_component<Tween_Component>();
                             tween.tween_target   = Tween_Component::TweenTarget::POSITION;
                             tween.start_position = start;
                             tween.end_position   = end;
                             tween.duration       = duration;
                             tween.ease_type      = type;
                             tween.loop_type      = loop;
                           });

  entity_type.set_function("set_tween_scale",
                           [](ag::Entity&               entity,
                              const vec2f&              start,
                              const vec2f               end,
                              float                     duration,
                              Tween_Component::EaseType type,
                              Tween_Component::LoopType loop) {
                             if (!entity.has_component<Tween_Component>())
                               entity.add_component<Tween_Component>();

                             auto& tween        = entity.get_component<Tween_Component>();
                             tween.tween_target = Tween_Component::TweenTarget::SCALE;
                             tween.start_scale  = start;
                             tween.end_scale    = end;
                             tween.duration     = duration;
                             tween.ease_type    = type;
                             tween.loop_type    = loop;
                           });

  entity_type.set_function("set_tween_rotation",
                           [](Entity&                   entity,
                              float                     start,
                              float                     end,
                              float                     duration,
                              Tween_Component::EaseType type,
                              Tween_Component::LoopType loop) {
                             if (!entity.has_component<Tween_Component>())
                               entity.add_component<Tween_Component>();

                             auto& tween        = entity.get_component<Tween_Component>();
                             tween.tween_target = Tween_Component::TweenTarget::ROTATION;
                             tween.rotation.x   = start;
                             tween.rotation.y   = end;
                             tween.duration     = duration;
                             tween.ease_type    = type;
                             tween.loop_type    = loop;
                           });

  entity_type.set_function("set_tween_end", [](Entity& entity, const vec2f& position) {
    detail::safe_set_comp_value(entity, &Tween_Component::end_position, position);
  });

  entity_type.set_function("set_tween_duration", [](Entity& entity, float duration) {
    detail::safe_set_comp_value(entity, &Tween_Component::duration, duration);
  });

  entity_type.set_function("is_tween_completed", [](Entity& entity) -> bool {
    auto state = detail::safe_get_comp_value(entity,
                                             &Tween_Component::state,
                                             Tween_Component::State::STOPPED);
    return state == Tween_Component::State::COMPLETED;
  });

  // UTILITY FUNCTIONS

  entity_type.set_function("is_hovered", [](Entity& entity) -> bool {
    auto state =
        detail::safe_get_comp_value(entity, &ButtonState_Component::button_state, (uint8_t)0);
    return state & Button_State::Hovered;
  });

  entity_type.set_function("is_pressed", [](Entity& entity) -> bool {
    auto state =
        detail::safe_get_comp_value(entity, &ButtonState_Component::button_state, (uint8_t)0);
    return state & Button_State::Pressed;
  });

  entity_type.set_function("is_disabled", [](Entity& entity) -> bool {
    auto state =
        detail::safe_get_comp_value(entity, &ButtonState_Component::button_state, (uint8_t)0);
    return state & Button_State::Disabled;
  });

  entity_type.set_function("disable_button", [](Entity& entity, bool disabled) {
    if (!entity.has_component<ButtonState_Component>())
      return;

    auto& state = entity.get_component<ButtonState_Component>().button_state;
    if (disabled)
      state |= Button_State::Disabled;
    else
      state &= ~Button_State::Disabled;
  });
}

}  // namespace ag
