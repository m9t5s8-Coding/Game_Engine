#include <GameObjects/Components/Components.hpp>
#include <GameObjects/GameObjects.hpp>
#include <Helper.hpp>
#include <Project/Assetmanager.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Renderer/ViewController.hpp>

namespace ag
{

void Tag_Component::add_component(Entity entity)
{
}
void Tag_Component::remove_component(Entity entity)
{
}
json Tag_Component::save_json(Entity entity)
{
  json  j;
  auto& tag = entity.get_component<Tag_Component>();

  Helper::save_json(j, "Name", tag.name);
  Helper::save_json(j, "NodeType", static_cast<int>(tag.node_type));
  Helper::save_json(j, "Visible", tag.visible);
  Helper::load_json(j, "Locked", tag.locked);
  Helper::save_json(j, "ID", tag.index);

  if (tag.parent.get_id() != INVALID_ENTITY)
  {
    auto parent_index = tag.parent.get_component<Tag_Component>().index;
    Helper::save_json(j, "Parent", parent_index);
  }

  if (!tag.children.empty())
  {
    json children_json = json::array();
    for (auto& child : tag.children)
    {
      auto child_index = child.get_component<Tag_Component>().index;
      children_json.push_back(child_index);
    }
    j["Children"] = children_json;
  }
  return j;
}
void Tag_Component::load_json(Entity entity, const json& j)
{
  auto& tag = entity.get_component<Tag_Component>();
  Helper::load_json(j, "Name", tag.name);
  Helper::load_json(j, "NodeType", tag.node_type);
  Helper::load_json(j, "ID", tag.index);
  Helper::load_json(j, "Visible", tag.visible);
  Helper::load_json(j, "Parent", tag.parent_id);

  if (j.contains("Children") && j["Children"].is_array())
  {
    auto& child_array = j["Children"];
    tag.children_id.clear();
    for (auto& id_json : child_array)
    {
      AG_uint child_id = id_json.get<AG_uint>();
      tag.children_id.push_back(child_id);
    }
  }

  index_map[tag.index] = entity;
}
void Tag_Component::load_children(Entity entity)
{
  auto& tag = entity.get_component<Tag_Component>();
  if (tag.parent_id != INVALID_ENTITY)
  {
    if (index_map.contains(tag.parent_id))
    {
      tag.parent = index_map[tag.parent_id];
    }
  }

  for (auto& child_id : tag.children_id)
  {
    if (!index_map.contains(child_id))
      continue;

    Entity child = index_map[child_id];
    tag.children.push_back(child);
  }
}
void Tag_Component::clone_entity(Entity original, Entity duplicate, Entity parent)
{
  const auto& original_tag  = original.get_component<Tag_Component>();
  auto&       duplicate_tag = duplicate.get_component<Tag_Component>();
  duplicate_tag.visible     = original_tag.visible;
  if (parent.get_id() != INVALID_ENTITY)
  {
    duplicate_tag.parent = parent;
    auto& parent_tag     = parent.get_component<Tag_Component>();
    parent_tag.children.push_back(duplicate);
  }
  duplicate_tag.children.clear();
  auto scene = Scene::get_active_scene();
  for (auto& children : original_tag.children)
  {
    scene->duplicate_entity(children, duplicate);
  }
}
bool Tag_Component::is_compatible(NodeType type)
{
  return true;
}
const char* Tag_Component::get_name()
{
  return "Tag";
}
bool Tag_Component::get_visibility(Entity entity)
{
  if (!entity.has_component<Tag_Component>())
    return true;

  const auto& tag = entity.get_component<Tag_Component>();

  if (!tag.visible)
    return false;

  if (tag.parent && tag.parent.get_id() != INVALID_ENTITY)
    return get_visibility(tag.parent);

  return true;
}
bool Tag_Component::get_lock(Entity entity)
{
  if (!entity.has_component<Tag_Component>())
    return true;

  const auto& tag = entity.get_component<Tag_Component>();

  if (!tag.locked)
    return false;

  if (tag.parent && tag.parent.get_id() != INVALID_ENTITY)
    return get_lock(tag.parent);

  return true;
}
bool Tag_Component::is_parent(Entity child, Entity parent)
{
  if (child.get_id() == INVALID_ENTITY || parent.get_id() == INVALID_ENTITY)
    return false;

  if (child.get_id() == parent.get_id())
    return true;
  std::unordered_set<uint32_t> visited;
  Entity                       current = child;

  while (current.get_id() != INVALID_ENTITY)
  {
    if (!current.has_component<Tag_Component>())
      return false;

    auto& tag = current.get_component<Tag_Component>();

    if (tag.parent.get_id() == parent.get_id())
      return true;

    if (!visited.insert(current.get_id()).second)
      return false;

    current = tag.parent;
  }

  return false;
}

void Transform_Component::add_component(Entity entity)
{
  entity.add_component<Transform_Component>();
}
void Transform_Component::remove_component(Entity entity)
{
  entity.remove_component<Transform_Component>();
}
json Transform_Component::save_json(Entity entity)
{
  json        j;
  const auto& props = entity.get_component<Transform_Component>();
  Helper::save_json(j, "Position", props.position);
  Helper::save_json(j, "Scale", props.scale);
  Helper::save_json(j, "Rotation", props.rotation);

  return j;
}
void Transform_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<Transform_Component>())
    entity.add_component<Transform_Component>();

  auto& props = entity.get_component<Transform_Component>();
  Helper::load_json(j, "Position", props.position);
  Helper::load_json(j, "Scale", props.scale);
  Helper::load_json(j, "Rotation", props.rotation);
}
void Transform_Component::clone_entity(Entity original, Entity clone)
{
  if (original.has_component<Transform_Component>())
  {
    clone.add_component<Transform_Component>(original.get_component<Transform_Component>());
  }
}
bool Transform_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::Render2D);
}
const char* Transform_Component::get_name()
{
  return "Transform";
}
Transform_Component Transform_Component::get_world_transform(Entity entity)
{
  Transform_Component world_transform;
  if (!entity.has_component<Transform_Component>())
    return world_transform;

  world_transform = entity.get_component<Transform_Component>();
  auto& tag       = entity.get_component<Tag_Component>();

  if (tag.parent.get_id() != INVALID_ENTITY)
  {
    Transform_Component parent_world = get_world_transform(tag.parent);

    world_transform.position = parent_world.position + world_transform.position;
    world_transform.scale    = parent_world.scale * world_transform.scale;
    world_transform.rotation = parent_world.rotation + world_transform.rotation;
  }
  return world_transform;
}
void Transform_Component::get_local_transform(Entity                     entity,
                                              const Transform_Component& world_transform)
{
  auto& transform = entity.get_component<Transform_Component>();
  auto& tag       = entity.get_component<Tag_Component>();

  if (tag.parent.get_id() != INVALID_ENTITY)
  {
    Transform_Component parent_world = get_world_transform(tag.parent);
    transform.position               = world_transform.position - parent_world.position;
    transform.scale                  = world_transform.scale / parent_world.scale;
    transform.rotation               = world_transform.rotation - parent_world.rotation;
  }
  else
  {
    transform = world_transform;
  }
}

void Script_Component::add_component(Entity entity)
{
  entity.add_component<Script_Component>();
}
void Script_Component::remove_component(Entity entity)
{
  auto& comp = entity.get_component<Script_Component>();
  comp.env.get().clear();
  comp.on_create  = LuaFunc();
  comp.on_update  = LuaFunc();
  comp.on_destroy = LuaFunc();
  comp.on_event   = LuaFunc();

  entity.remove_component<Script_Component>();
}
json Script_Component::save_json(Entity entity)
{
  json j;

  auto& comp = entity.get_component<Script_Component>();
  Helper::save_json(j, "Path", comp.path);

  return j;
}
void Script_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<Script_Component>())
    entity.add_component<Script_Component>();

  auto& props = entity.get_component<Script_Component>();
  Helper::load_json(j, "Path", props.path);
}
void Script_Component::clone_entity(Entity original, Entity clone)
{
  if (original.has_component<Script_Component>())
  {
    const auto&      original_path = original.get_component<Script_Component>().path;
    Script_Component comps;
    comps.path = original_path;
    clone.add_component<Script_Component>(comps);
    load_scripts(clone);
  }
}
void Script_Component::create(Entity entity)
{
  if (!entity.has_component<Script_Component>() || !Engine::is_runtime())
    return;
  auto& comp = entity.get_component<Script_Component>();
  if (comp.on_create.is_valid())
  {
    comp.on_create.call();
  }
}
void Script_Component::update(Entity entity, TimeStamp ts)
{
  if (!entity.has_component<Script_Component>() || !Engine::is_runtime())
    return;

  auto& comp = entity.get_component<Script_Component>();

  if (comp.on_update.is_valid())
  {
    comp.on_update.call(ts.get_seconds());
  }
}
void Script_Component::destroy(Entity entity)
{
  if (!entity.has_component<Script_Component>() || !Engine::is_runtime())
    return;

  auto& comp = entity.get_component<Script_Component>();

  if (comp.on_destroy.is_valid())
  {
    comp.on_destroy.call();
  }

  comp.env.get().clear();
  comp.on_create  = LuaFunc();
  comp.on_update  = LuaFunc();
  comp.on_destroy = LuaFunc();
  comp.on_event   = LuaFunc();
}
bool Script_Component::event(Entity entity, Event& e)
{
  if (!entity.has_component<Script_Component>() || !Engine::is_runtime())
    return false;

  auto& comp = entity.get_component<Script_Component>();
  if (!comp.on_event.is_valid())
    return false;

  comp.on_event.call(e);

  return false;
}
void Script_Component::load_scripts(Entity entity)
{
  if (!Engine::is_runtime() || !entity.has_component<Script_Component>())
    return;

  auto& comp = entity.get_component<Script_Component>();

  auto        project   = Project::get_active_project();
  std::string full_path = project->get_directory() + project->get_scripts_directory() + comp.path;

  comp.env.get().set_function("get_entity", [entity]() -> Entity { return entity; });

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
      ScriptManager::run_script_in_env(code, comp.env);
    }
    catch (const sol::error& e)
    {
      AERO_CORE_ERROR("Script error in {0}: {1}", full_path, e.what());
      return;
    }
  }
  else
  {
    ScriptManager::load_script(full_path, comp.env);
  }

  comp.on_create.set_function(comp.env, "on_create");
  comp.on_update.set_function(comp.env, "on_update");
  comp.on_destroy.set_function(comp.env, "on_destroy");
  comp.on_event.set_function(comp.env, "on_event");
  create(entity);
}
bool Script_Component::is_compatible(NodeType type)
{
  return true;
}
const char* Script_Component::get_name()
{
  return "Script";
}

json Render2D_Component::save_json(Entity entity)
{
  json  j;
  auto& props = entity.get_component<Render2D_Component>();
  Helper::save_json(j, "Size", props.size);
  Helper::save_json(j, "Color", props.color);

  return j;
}
void Render2D_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<Render2D_Component>())
    entity.add_component<Render2D_Component>();

  auto& props = entity.get_component<Render2D_Component>();
  Helper::load_json(j, "Size", props.size);
  Helper::load_json(j, "Color", props.color);
}
bool Render2D_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::Render2D);
}

json Border_Component::save_json(Entity entity)
{
  json j;

  auto& props = entity.get_component<Border_Component>();
  Helper::save_json(j, "Thickness", props.thickness);
  Helper::save_json(j, "Color", props.color);

  return j;
}
void Border_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<Border_Component>())
    entity.add_component<Border_Component>();

  auto& props = entity.get_component<Border_Component>();
  Helper::load_json(j, "Thickness", props.thickness);
  Helper::load_json(j, "Color", props.color);
}
bool Border_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::RectShape) ||
         NodeHelper::has_capability(caps, Node_Capability::CircleShape);
}

json Corner_Component::save_json(Entity entity)
{
  json j;

  auto& props = entity.get_component<Corner_Component>();
  Helper::save_json(j, "Corner", props.corner);
  Helper::save_json(j, "Uniform", props.uniform);

  return j;
}
void Corner_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<Corner_Component>())
    entity.add_component<Corner_Component>();

  auto& props = entity.get_component<Corner_Component>();
  Helper::load_json(j, "Corner", props.corner);
  Helper::load_json(j, "Uniform", props.uniform);
}
bool Corner_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::RectShape);
}

json UI_Component::save_json(Entity entity)
{
  json j;

  auto& props = entity.get_component<UI_Component>();
  Helper::save_json(j, "Mode", static_cast<int>(props.mode));

  return j;
}
void UI_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<UI_Component>())
    entity.add_component<UI_Component>();

  auto& props = entity.get_component<UI_Component>();
  Helper::load_json(j, "Mode", props.mode);
}
bool UI_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::UI);
}

json Texture_Component::save_json(Entity entity)
{
  json j;

  auto& props = entity.get_component<Texture_Component>();
  Helper::save_json(j, "Path", props.path);
  Helper::save_json(j, "Mode", (int)props.filter_mode);

  return j;
}
void Texture_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<Texture_Component>())
    entity.add_component<Texture_Component>();

  auto& props = entity.get_component<Texture_Component>();
  Helper::load_json(j, "Path", props.path);
  Helper::load_json(j, "Mode", props.filter_mode);

  if (!props.path.empty())
    props.texture = NodeHelper::load_texture(props.path, true, props.filter_mode);
}
bool Texture_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::Texture2D);
}

json TextureRect_Component::save_json(Entity entity)
{
  json j;

  auto& props = entity.get_component<TextureRect_Component>();
  Helper::save_json(j, "Rect", props.rect);

  return j;
}
void TextureRect_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<TextureRect_Component>())
    entity.add_component<TextureRect_Component>();

  auto& props = entity.get_component<TextureRect_Component>();
  Helper::load_json(j, "Rect", props.rect);
}
bool TextureRect_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::Texture2D);
}

json TextureFlip_Component::save_json(Entity entity)
{
  json j;

  auto& props = entity.get_component<TextureFlip_Component>();
  Helper::save_json(j, "Horizontal", props.horizontal);
  Helper::save_json(j, "Vertical", props.vertical);

  return j;
}
void TextureFlip_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<TextureFlip_Component>())
    entity.add_component<TextureFlip_Component>();

  auto& props = entity.get_component<TextureFlip_Component>();
  Helper::load_json(j, "Horizontal", props.horizontal);
  Helper::load_json(j, "Vertical", props.vertical);
}
bool TextureFlip_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::Texture2D);
}

json Camera_Component::save_json(Entity entity)
{
  json j;

  auto& props = entity.get_component<Camera_Component>();
  Helper::save_json(j, "Size", props.size);
  Helper::save_json(j, "Center", props.center);

  return j;
}
void Camera_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<Camera_Component>())
    entity.add_component<Camera_Component>();

  auto& props = entity.get_component<Camera_Component>();
  Helper::load_json(j, "Size", props.size);
  Helper::load_json(j, "Center", props.center);
}
bool Camera_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::Camera);
}

json Window_Component::save_json(Entity entity)
{
  json j;

  auto& props = entity.get_component<Window_Component>();
  Helper::save_json(j, "Size", props.size);

  return j;
}
void Window_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<Window_Component>())
    entity.add_component<Window_Component>();

  auto& props = entity.get_component<Window_Component>();
  Helper::load_json(j, "Size", props.size);
}
bool Window_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::Camera);
}

vec2f apply_camera_bounds(const vec2f& position, Entity entity)
{
  if (!entity.has_component<CameraBounds_Component>())
    return position;

  auto& bounds  = entity.get_component<CameraBounds_Component>();
  vec2f clamped = position;

  // Clamp X axis
  if (bounds.x_axis.x != std::numeric_limits<float>::min() &&
      bounds.x_axis.y != std::numeric_limits<float>::max())
  {
    clamped.x = std::clamp(clamped.x, bounds.x_axis.x, bounds.x_axis.y);
  }

  // Clamp Y axis
  if (bounds.y_axis.x != std::numeric_limits<float>::min() &&
      bounds.y_axis.y != std::numeric_limits<float>::max())
  {
    clamped.y = std::clamp(clamped.y, bounds.y_axis.x, bounds.y_axis.y);
  }

  return clamped;
}

void CameraBounds_Component::add_component(Entity entity)
{
  if (!entity.has_component<CameraBounds_Component>())
  {
    CameraBounds_Component comps;
    comps.x_axis.x = std::numeric_limits<float>::min();
    comps.x_axis.y = std::numeric_limits<float>::max();
    comps.y_axis.x = std::numeric_limits<float>::min();
    comps.y_axis.y = std::numeric_limits<float>::max();
    entity.add_component<CameraBounds_Component>(comps);
  }
}
json CameraBounds_Component::save_json(Entity entity)
{
  json  j;
  auto& bounds = entity.get_component<CameraBounds_Component>();
  Helper::save_json(j, "X_axis", bounds.x_axis);
  Helper::save_json(j, "Y_axis", bounds.y_axis);
  return j;
}
void CameraBounds_Component::load_json(Entity entity, const json& j)
{
  CameraBounds_Component::add_component(entity);
  auto& bounds = entity.get_component<CameraBounds_Component>();
  Helper::load_json(j, "X_axis", bounds.x_axis);
  Helper::load_json(j, "Y_axis", bounds.y_axis);
}
bool CameraBounds_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::Camera);
}
void CameraBounds_Component::on_update(Entity entity)
{
  // Apply bounds even without follow component
  if (!Engine::is_runtime() || !entity.has_component<CameraBounds_Component>())
    return;

  auto& bounds = entity.get_component<CameraBounds_Component>();

  // Check if bounds are actually set (not default values)
  bool x_bounds_set = (bounds.x_axis.x != std::numeric_limits<float>::min() ||
                       bounds.x_axis.y != std::numeric_limits<float>::max());
  bool y_bounds_set = (bounds.y_axis.x != std::numeric_limits<float>::min() ||
                       bounds.y_axis.y != std::numeric_limits<float>::max());

  if (!x_bounds_set && !y_bounds_set)
    return;  // No bounds actually set

  auto  view_controller = ViewController::get_main_controller();
  auto& view            = view_controller->get_view();
  vec2f current_pos     = view.get_center();
  vec2f clamped_pos     = current_pos;

  // Clamp X axis
  if (x_bounds_set)
  {
    clamped_pos.x = std::clamp(clamped_pos.x, bounds.x_axis.x, bounds.x_axis.y);
  }

  // Clamp Y axis
  if (y_bounds_set)
  {
    clamped_pos.y = std::clamp(clamped_pos.y, bounds.y_axis.x, bounds.y_axis.y);
  }

  // Only update if position changed
  if (clamped_pos.x != current_pos.x || clamped_pos.y != current_pos.y)
  {
    view.set_center(clamped_pos);
  }
}

json CameraFollow_Component::save_json(Entity entity)
{
  json  j;
  auto& comps = entity.get_component<CameraFollow_Component>();
  int   index = -1;
  if (comps.target && comps.target.get_id() != INVALID_ENTITY)
    index = comps.target.get_component<Tag_Component>().index;
  Helper::save_json(j, "Type", (int)comps.type);
  Helper::save_json(j, "FollowIndex", index);
  Helper::save_json(j, "LerpSpeed", comps.lerp_speed);
  Helper::save_json(j, "SpringStiffness", comps.spring_stiffness);
  Helper::save_json(j, "SpringDamping", comps.spring_damping);
  Helper::save_json(j, "DeadZone", comps.dead_zone);
  return j;
}
void CameraFollow_Component::load_json(Entity entity, const json& j, bool load)
{
  if (!entity.has_component<CameraFollow_Component>())
    entity.add_component<CameraFollow_Component>();
  auto& comps = entity.get_component<CameraFollow_Component>();
  int   index = -1;
  Helper::load_json(j, "Type", comps.type);
  Helper::load_json(j, "FollowIndex", index);
  Helper::load_json(j, "LerpSpeed", comps.lerp_speed);
  Helper::load_json(j, "SpringStiffness", comps.spring_stiffness);
  Helper::load_json(j, "SpringDamping", comps.spring_damping);
  Helper::load_json(j, "DeadZone", comps.dead_zone);
  if (index >= 0)
  {
    auto it = index_map.find(index);
    if (it != index_map.end())
      comps.target = it->second;
  }
}
bool CameraFollow_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::Camera);
}
void CameraFollow_Component::on_update(Entity entity, TimeStamp ts)
{
  if (!Engine::is_runtime() || !entity.has_component<CameraFollow_Component>())
    return;

  auto& props = entity.get_component<CameraFollow_Component>();
  if (!props.target.has_component<Transform_Component>() || props.type == FollowType::NONE)
    return;

  auto  view_controller = ViewController::get_main_controller();
  auto& view            = view_controller->get_view();

  float dt          = ts.get_seconds();
  vec2f target_pos  = Transform_Component::get_world_transform(props.target).position;
  vec2f current_pos = view.get_center();
  vec2f new_pos     = current_pos;

  switch (props.type)
  {
    case FollowType::LOCK_ON_TARGET:
    {
      new_pos = target_pos;
      break;
    }

    case FollowType::LERP_SMOOTH:
    {
      // Smooth interpolation using lerp
      // Default speed is 5.0f if not set
      float speed = (props.lerp_speed > 0.0f) ? props.lerp_speed : 5.0f;
      float t     = 1.0f - std::exp(-speed * dt);
      new_pos     = current_pos + (target_pos - current_pos) * t;
      break;
    }

    case FollowType::SPRING:
    {
      // Spring-damper system for smooth, natural following
      // Defaults: stiffness = 150.0f, damping = 20.0f
      float stiffness = (props.spring_stiffness > 0.0f) ? props.spring_stiffness : 150.0f;
      float damping   = (props.spring_damping > 0.0f) ? props.spring_damping : 20.0f;

      // Calculate spring force
      vec2f displacement = target_pos - current_pos;
      vec2f spring_force = displacement * stiffness;

      // Calculate damping force
      vec2f damping_force = props.velocity * damping;

      // Apply forces (F = ma, assuming mass = 1)
      vec2f acceleration = spring_force - damping_force;

      // Update velocity and position using semi-implicit Euler
      props.velocity = props.velocity + acceleration * dt;
      new_pos        = current_pos + props.velocity * dt;
      break;
    }

    case FollowType::DEAD_ZONE:
    {
      // Only move camera when target leaves the dead zone
      // Default dead zone size
      vec2f dead_zone = (props.dead_zone.x > 0.0f && props.dead_zone.y > 0.0f)
                            ? props.dead_zone
                            : vec2f(100.0f, 100.0f);

      vec2f offset         = target_pos - current_pos;
      vec2f half_dead_zone = dead_zone * 0.5f;

      new_pos = current_pos;

      // Check X axis
      if (std::abs(offset.x) > half_dead_zone.x)
      {
        float excess = std::abs(offset.x) - half_dead_zone.x;
        new_pos.x += (offset.x > 0.0f ? excess : -excess);
      }

      // Check Y axis
      if (std::abs(offset.y) > half_dead_zone.y)
      {
        float excess = std::abs(offset.y) - half_dead_zone.y;
        new_pos.y += (offset.y > 0.0f ? excess : -excess);
      }

      // Smooth the dead zone transition
      float speed = (props.lerp_speed > 0.0f) ? props.lerp_speed : 10.0f;
      float t     = 1.0f - std::exp(-speed * dt);
      new_pos     = current_pos + (new_pos - current_pos) * t;
      break;
    }

    default:
      return;
  }

  // Apply camera bounds if they exist
  new_pos = apply_camera_bounds(new_pos, entity);

  view.set_center(new_pos);
}

json Animation::save_json(const Animation& animation)
{
  json j;
  Helper::save_json(j, "Name", animation.name);
  Helper::save_json(j, "FPS", animation.fps);
  Helper::save_json(j, "PingPong", animation.ping_pong);
  Helper::save_json(j, "Loop", animation.loop);
  j["Frames"] = json::array();
  for (const auto& frame : animation.frames)
  {
    json frame_json;
    Helper::save_json(frame_json, "Rect", frame.frame_rect);
    j["Frames"].push_back(frame_json);
  }
  return j;
}
Animation Animation::load_json(const json& j)
{
  Animation animation;
  Helper::load_json(j, "Name", animation.name);
  Helper::load_json(j, "FPS", animation.fps);
  Helper::load_json(j, "PingPong", animation.ping_pong);
  Helper::load_json(j, "Loop", animation.loop);
  if (j.contains("Frames") && j["Frames"].is_array())
  {
    for (const json& frame_json : j["Frames"])
    {
      Frame frame;
      Helper::load_json(frame_json, "Rect", frame.frame_rect);
      animation.frames.push_back(frame);
    }
  }
  return animation;
}

json Animation_Component::save_json(Entity entity)
{
  json  j;
  auto& props = entity.get_component<Animation_Component>();
  Helper::save_json(j, "Current", props.current_animation);
  Helper::save_json(j, "Playing", props.playing);

  j["Animations"] = json::object();
  for (const auto& [name, animation] : props.animations)
  {
    j["Animations"][name] = Animation::save_json(animation);
  }
  return j;
}
void Animation_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<Animation_Component>())
    entity.add_component<Animation_Component>();

  auto& props = entity.get_component<Animation_Component>();
  Helper::load_json(j, "Current", props.current_animation);
  Helper::load_json(j, "Playing", props.playing);

  if (j.contains("Animations") && j["Animations"].is_object())
  {
    props.animations.clear();

    for (const auto& [name, animation_json] : j["Animations"].items())
    {
      Animation animation    = Animation::load_json(animation_json);
      props.animations[name] = animation;
    }
  }
}
void Animation_Component::update(Entity entity, TimeStamp ts)
{
  if (!entity.has_component<Animation_Component>())
    return;
  float dt    = ts.get_seconds();
  auto& props = entity.get_component<Animation_Component>();
  if (!entity.has_component<Texture_Component>() ||
      !entity.get_component<Texture_Component>().texture)
    return;
  if (props.current_animation.empty())
  {
    auto& texture = entity.get_component<Texture_Component>();
    if (texture.texture)
    {
      props.rect = {0, 0, texture.texture->get_size()};
    }
    return;
  }
  auto it = props.animations.find(props.current_animation);
  if (props.animations.find(props.current_animation) == props.animations.end())
  {
    auto& texture = entity.get_component<Texture_Component>();
    if (texture.texture)
    {
      props.rect = {0, 0, texture.texture->get_size()};
    }
    return;
  }
  Animation& anim = it->second;
  if (!props.playing)
  {
    props.rect = anim.frames[props.current_frame].frame_rect;
    return;
  }
  if (anim.frames.empty())
  {
    auto& texture = entity.get_component<Texture_Component>();
    props.rect    = {0, 0, texture.texture->get_size()};
    return;
  }

  Frame& current_frame = anim.frames[props.current_frame];
  props.timer += dt;
  float duration = 1.0f / anim.fps;
  if (props.timer >= duration)
  {
    props.timer -= duration;
    if (anim.ping_pong)
    {
      props.current_frame += props.reverse_direction ? -1 : 1;

      if (props.current_frame >= anim.frames.size() && props.current_frame >= 0)
      {
        if (anim.loop)
        {
          props.current_frame     = anim.frames.size() - 2;
          props.reverse_direction = true;
        }
        else
        {
          props.current_frame = anim.frames.size() - 1;
          props.playing       = false;
        }
      }
      else if (props.current_frame < 0)
      {
        props.current_frame     = 1;
        props.reverse_direction = false;
      }
    }
    else
    {
      props.current_frame++;
      if (props.current_frame >= anim.frames.size())
      {
        if (anim.loop)
        {
          props.current_frame               = 0;
          props.current_animation_completed = true;
        }
        else
        {
          props.current_frame               = anim.frames.size() - 1;
          props.playing                     = false;
          props.current_animation_completed = true;
        }
      }
    }
  }
  if (props.current_animation_completed && anim.loop)
  {
    props.current_animation_completed = false;
  }
  props.rect = anim.frames[props.current_frame].frame_rect;
}
bool Animation_Component::play_animation(Entity entity, const std::string& name, bool restart)
{
  if (!entity.has_component<Animation_Component>())
    return false;

  auto& anim = entity.get_component<Animation_Component>();

  if (anim.animations.find(name) == anim.animations.end())
    return false;

  if (anim.current_animation == name and !restart)
    return false;

  anim.current_animation           = name;
  anim.current_frame               = 0;
  anim.timer                       = 0.0f;
  anim.current_animation_completed = false;
  return true;
}

json Tile_Defination::save_json(const Tile_Defination& def)
{
  json j;
  Helper::save_json(j, "Rect", def.texture_rect);
  Helper::save_json(j, "Solid", def.is_solid);

  return j;
}
void Tile_Defination::load_json(Tile_Defination& def, const json& j)
{
  Helper::load_json(j, "Rect", def.texture_rect);
  Helper::load_json(j, "Solid", def.is_solid);
}

json Tile::save_json(const Tile& tile)
{
  json j;
  Helper::save_json(j, "ID", tile.tile_id);
  Helper::save_json(j, "SetID", tile.set_id);
  Helper::save_json(j, "AutoTile", tile.use_autotile);
  return j;
}
Tile Tile::load_json(const json& j)
{
  Tile tile;
  Helper::load_json(j, "ID", tile.tile_id);
  Helper::load_json(j, "SetID", tile.set_id);
  Helper::load_json(j, "AutoTile", tile.use_autotile);

  return tile;
}
json Tile_Component::save_json(Entity entity)
{
  json  j;
  auto& tileset = entity.get_component<Tile_Component>();
  Helper::save_json(j, "Size", tileset.size);
  Helper::save_json(j, "Offset", tileset.offset);

  return j;
}
void Tile_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<Tile_Component>())
    entity.add_component<Tile_Component>();

  auto& tileset = entity.get_component<Tile_Component>();
  Helper::load_json(j, "Size", tileset.size);
  Helper::load_json(j, "Offset", tileset.offset);
}

json SolidSet_Component::save_json(Entity entity)
{
  json        j;
  const auto& comps = entity.get_component<SolidSet_Component>();

  for (const auto& [grid, value] : comps.placed_tiles)
  {
    std::string key = std::to_string(grid.x) + "," + std::to_string(grid.y);
    j["Grid"][key]  = value;
  }

  return j;
}
void SolidSet_Component::load_json(Entity entity, const json& j)
{
  SolidSet_Component::add_component(entity);
  auto& comps = entity.get_component<SolidSet_Component>();
  if (j.contains("Grid"))
  {
    for (auto& [key, id_json] : j["Grid"].items())
    {
      vec2u pos;
      sscanf(key.c_str(), "%u,%u", &pos.x, &pos.y);
      bool value;
      Helper::load_json(id_json, value);
      comps.placed_tiles[pos] = value;
    }
  }
}
void SolidSet_Component::clone_entity(Entity original, Entity clone)
{
  if (original.has_component<SolidSet_Component>())
  {
    auto&              orig = original.get_component<SolidSet_Component>();
    SolidSet_Component comps;

    comps.placed_tiles = orig.placed_tiles;

    clone.add_component<SolidSet_Component>(comps);
  }
}
void SolidSet_Component::update(Entity entity)
{
  if (!entity.has_component<SolidSet_Component>() || !entity.has_component<Tile_Component>() ||
      !Engine::is_runtime())
    return;
  create_body(entity);
}
void SolidSet_Component::draw(Entity entity)
{
  if (Engine::is_runtime())
    return;

  if (!entity.has_component<SolidSet_Component>() || !entity.has_component<Tile_Component>())
    return;

  auto&               solid_set = entity.get_component<SolidSet_Component>();
  auto&               props     = entity.get_component<Tile_Component>();
  Transform_Component trans;
  int                 entity_id = (int)(entity.get_id());
  Rectangle           rect;
  rect.size       = props.size;
  rect.fill_color = Color(80, 180, 255, 150);

  for (const auto& [position, tile] : solid_set.placed_tiles)
  {
    trans.position = (position * props.size) + props.size / 2 + props.offset;
    Renderer2D::draw_rectangle(rect, trans);
  }
}
bool SolidSet_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::TileMap);
}
void SolidSet_Component::create_body(Entity entity)
{
  auto& props = entity.get_component<SolidSet_Component>();
  if (props.body)
    return;

  auto& tile_comps = entity.get_component<Tile_Component>();

  b2BodyDef body_def;
  body_def.type = b2_staticBody;
  body_def.position.Set(0.0f, 0.0f);

  auto  scene = Scene::get_active_scene();
  auto& world = scene->get_world();
  props.body  = world.CreateBody(&body_def);

  vec2i min_pos(INT_MAX, INT_MAX);
  vec2i max_pos(INT_MIN, INT_MIN);

  for (const auto& [pos, tile_detail] : props.placed_tiles)
  {
    min_pos.x = std::min(min_pos.x, pos.x);
    min_pos.y = std::min(min_pos.y, pos.y);
    max_pos.x = std::max(max_pos.x, pos.x);
    max_pos.y = std::max(max_pos.y, pos.y);
  }

  std::vector<std::tuple<vec2i, vec2i>> rectangles;

  for (int y = min_pos.y; y <= max_pos.y; ++y)
  {
    for (int x = min_pos.x; x <= max_pos.x; ++x)
    {
      vec2i start_pos(x, y);

      if (props.placed_tiles.find(start_pos) == props.placed_tiles.end() ||
          !props.placed_tiles[start_pos])
      {
        continue;
      }

      int width = 1;
      while (props.placed_tiles.find(vec2i(x + width, y)) != props.placed_tiles.end() &&
             props.placed_tiles[vec2i(x + width, y)])
      {
        width++;
      }

      int  height     = 1;
      bool can_expand = true;
      while (can_expand)
      {
        for (int dx = 0; dx < width; ++dx)
        {
          vec2i check_pos(x + dx, y + height);
          if (props.placed_tiles.find(check_pos) == props.placed_tiles.end() ||
              !props.placed_tiles[check_pos])
          {
            can_expand = false;
            break;
          }
        }
        if (can_expand)
        {
          height++;
        }
      }

      for (int dy = 0; dy < height; ++dy)
      {
        for (int dx = 0; dx < width; ++dx)
        {
          props.placed_tiles[vec2i(x + dx, y + dy)] = false;
        }
      }

      rectangles.push_back(std::make_tuple(start_pos, vec2i(width, height)));
    }
  }

  for (const auto& [start_pos, size_in_tiles] : rectangles)
  {
    vec2f center =
        (vec2f(start_pos) + vec2f(size_in_tiles) * 0.5f) * tile_comps.size + tile_comps.offset;
    Math::pixels_to_meters(center);

    vec2f half_size = vec2f(size_in_tiles) * tile_comps.size * 0.5f;
    Math::pixels_to_meters(half_size);

    b2PolygonShape shape;
    shape.SetAsBox(half_size.x, half_size.y, {center.x, center.y}, 0.0f);

    b2FixtureDef fixture_def;
    fixture_def.shape       = &shape;
    fixture_def.density     = 0.0f;
    fixture_def.friction    = 0.0f;
    fixture_def.restitution = 0.0f;

    props.body->CreateFixture(&fixture_def);
  }

  props.body->GetUserData().pointer = (uintptr_t)entity.get_id();
}

json TileSet_Component::save_json(Entity entity)
{
  json  j;
  auto& tileset = entity.get_component<TileSet_Component>();
  if (!tileset.tile_definitions.empty())
    Helper::save_json(j, "Registered", tileset.is_tile_registered);

  Helper::save_json(j, "Size", tileset.tile_size);

  for (const auto& [id, def] : tileset.tile_definitions)
  {
    std::string key       = std::to_string(id.x) + "," + std::to_string(id.y);
    j["Definations"][key] = Tile_Defination::save_json(def);
  }

  for (const auto& [grid, tile] : tileset.placed_tiles)
  {
    std::string key = std::to_string(grid.x) + "," + std::to_string(grid.y);
    j["Grid"][key]  = Tile::save_json(tile);
  }

  return j;
}
void TileSet_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<TileSet_Component>())
    entity.add_component<TileSet_Component>();

  auto& tileset = entity.get_component<TileSet_Component>();

  Helper::load_json(j, "Size", tileset.tile_size);

  if (j.contains("Definations"))
  {
    for (auto& [key, def_json] : j["Definations"].items())
    {
      Tile_Defination def;
      Tile_Defination::load_json(def, def_json);

      vec2u id;
      sscanf(key.c_str(), "%u,%u", &id.x, &id.y);
      tileset.tile_definitions[id] = def;
    }
  }

  if (j.contains("Grid"))
  {
    for (auto& [key, id_json] : j["Grid"].items())
    {
      Tile  tile = Tile::load_json(id_json);
      vec2u pos;
      sscanf(key.c_str(), "%u,%u", &pos.x, &pos.y);
      tileset.placed_tiles[pos] = tile;
    }
  }

  Helper::load_json(j, "Registered", tileset.is_tile_registered);
  if (!tileset.tile_definitions.empty())
    tileset.tile_changed = true;
}
void TileSet_Component::clone_entity(Entity original, Entity clone)
{
  if (!original.has_component<TileSet_Component>())
    return;

  std::unordered_map<vec2u, Tile_Defination, vec2_hash<AG_uint>> tile_definitions;
  std::unordered_map<vec2i, Tile, vec2_hash<int>>                placed_tiles;
  bool                                                           is_tile_registered = false;
  bool                                                           tile_changed       = false;
  vec2i                                                          tile_size;

  const auto&       original_tileset = original.get_component<TileSet_Component>();
  TileSet_Component clone_tileset;
  clone_tileset.tile_definitions   = original_tileset.tile_definitions;
  clone_tileset.placed_tiles       = original_tileset.placed_tiles;
  clone_tileset.is_tile_registered = original_tileset.is_tile_registered;
  clone_tileset.tile_changed       = original_tileset.tile_changed;
  clone_tileset.tile_size          = original_tileset.tile_size;

  clone.add_component<TileSet_Component>(clone_tileset);
}
void TileSet_Component::update(Entity entity)
{
  if (!entity.has_component<TileSet_Component>() || !entity.has_component<Tile_Component>() ||
      !Engine::is_runtime())
    return;

  create_body(entity);
}
void TileSet_Component::draw(Entity entity)
{
  if (!entity.has_component<TileSet_Component>() || !entity.has_component<Tile_Component>())
    return;

  auto&               tile_set = entity.get_component<TileSet_Component>();
  auto&               props    = entity.get_component<Tile_Component>();
  Transform_Component trans;
  int                 entity_id = (int)(entity.get_id());
  Sprite              sprite;
  sprite.size = props.size;

  for (const auto& [position, tile] : tile_set.placed_tiles)
  {
    auto tex_it = tile_set.tile_definitions.find(tile.tile_id);
    if (tex_it == tile_set.tile_definitions.end())
    {
      continue;
    }
    const Tile_Defination& def = tex_it->second;
    sprite.texture_rect        = def.texture_rect;

    trans.position = (position * props.size) + props.size / 2 + props.offset;

    Renderer2D::draw_sprite(sprite, trans);
  }
}
bool TileSet_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::TileMap);
}
void TileSet_Component::create_body(Entity entity)
{
  auto& props = entity.get_component<TileSet_Component>();
  if (props.body)
    return;
  auto& tile_comps = entity.get_component<Tile_Component>();

  b2BodyDef body_def;
  body_def.type = b2_staticBody;
  body_def.position.Set(0.0f, 0.0f);

  auto  scene = Scene::get_active_scene();
  auto& world = scene->get_world();
  props.body  = world.CreateBody(&body_def);

  std::unordered_map<vec2i, bool, vec2_hash<int>> solid_grid;
  vec2i                                           min_pos(INT_MAX, INT_MAX);
  vec2i                                           max_pos(INT_MIN, INT_MIN);

  for (const auto& [pos, tile_detail] : props.placed_tiles)
  {
    auto tex_it = props.tile_definitions.find(tile_detail.tile_id);
    if (tex_it == props.tile_definitions.end())
    {
      continue;
    }
    const Tile_Defination& def = tex_it->second;
    if (def.is_solid)
    {
      solid_grid[pos] = true;
      min_pos.x       = std::min(min_pos.x, pos.x);
      min_pos.y       = std::min(min_pos.y, pos.y);
      max_pos.x       = std::max(max_pos.x, pos.x);
      max_pos.y       = std::max(max_pos.y, pos.y);
    }
  }

  if (solid_grid.empty())
    return;

  std::vector<std::tuple<vec2i, vec2i>> rectangles;

  for (int y = min_pos.y; y <= max_pos.y; ++y)
  {
    for (int x = min_pos.x; x <= max_pos.x; ++x)
    {
      vec2i start_pos(x, y);

      if (solid_grid.find(start_pos) == solid_grid.end() || !solid_grid[start_pos])
      {
        continue;
      }

      int width = 1;
      while (solid_grid.find(vec2i(x + width, y)) != solid_grid.end() &&
             solid_grid[vec2i(x + width, y)])
      {
        width++;
      }

      int  height     = 1;
      bool can_expand = true;
      while (can_expand)
      {
        for (int dx = 0; dx < width; ++dx)
        {
          vec2i check_pos(x + dx, y + height);
          if (solid_grid.find(check_pos) == solid_grid.end() || !solid_grid[check_pos])
          {
            can_expand = false;
            break;
          }
        }
        if (can_expand)
        {
          height++;
        }
      }

      for (int dy = 0; dy < height; ++dy)
      {
        for (int dx = 0; dx < width; ++dx)
        {
          solid_grid[vec2i(x + dx, y + dy)] = false;
        }
      }

      rectangles.push_back(std::make_tuple(start_pos, vec2i(width, height)));
    }
  }

  for (const auto& [start_pos, size_in_tiles] : rectangles)
  {
    vec2f center =
        (vec2f(start_pos) + vec2f(size_in_tiles) * 0.5f) * tile_comps.size + tile_comps.offset;
    Math::pixels_to_meters(center);

    vec2f half_size = vec2f(size_in_tiles) * tile_comps.size * 0.5f;
    Math::pixels_to_meters(half_size);

    b2PolygonShape shape;
    shape.SetAsBox(half_size.x, half_size.y, {center.x, center.y}, 0.0f);

    b2FixtureDef fixture_def;
    fixture_def.shape       = &shape;
    fixture_def.density     = 0.0f;
    fixture_def.friction    = 0.0f;
    fixture_def.restitution = 0.0f;

    props.body->CreateFixture(&fixture_def);
  }

  props.body->GetUserData().pointer = (uintptr_t)entity.get_id();
}

json Auto_Tiles::save_json(const Auto_Tiles& tiles)
{
  json j;

  for (const auto& [mask, id] : tiles.tile_bitmask)
  {
    std::string key = std::to_string(mask);
    Helper::save_json(j, key, id);
  }
  Helper::save_json(j, "SetID", tiles.set_id);
  return j;
}
Auto_Tiles Auto_Tiles::load_json(const json& j)
{
  Auto_Tiles tiles;

  for (auto& [key_str, value] : j.items())
  {
    if (key_str == "SetID")
      continue;

    uint16_t mask = static_cast<uint16_t>(std::stoi(key_str));

    vec2u tile_id;
    Helper::load_json(j, key_str, tile_id);
    tiles.tile_bitmask[mask] = tile_id;
  }
  Helper::load_json(j, "SetID", tiles.set_id);
  return tiles;
}

void AutoTiling_Component::remove_component(Entity entity)
{
  auto& props = entity.get_component<AutoTiling_Component>();
  props.auto_tiles.clear();
  entity.remove_component<AutoTiling_Component>();
}
json AutoTiling_Component::save_json(Entity entity)
{
  json        j;
  const auto& props = entity.get_component<AutoTiling_Component>();

  j["AutoTiles"] = json::object();
  for (const auto& [key, auto_tile] : props.auto_tiles)
  {
    j["AutoTiles"][key] = Auto_Tiles::save_json(auto_tile);
  }

  return j;
}
void AutoTiling_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<AutoTiling_Component>())
    entity.add_component<AutoTiling_Component>();

  auto& props = entity.get_component<AutoTiling_Component>();

  if (j.contains("AutoTiles") && j["AutoTiles"].is_object())
  {
    props.auto_tiles.clear();

    for (const auto& [name, autotile_json] : j["AutoTiles"].items())
    {
      Auto_Tiles auto_tile   = Auto_Tiles::load_json(autotile_json);
      props.auto_tiles[name] = auto_tile;
      props.next_id          = std::max(auto_tile.set_id, props.next_id);
    }
  }
}
bool AutoTiling_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::TileMap);
}

void CollisionShape_Component::add_component(Entity entity)
{
  entity.add_component<CollisionShape_Component>();
}
json CollisionShape_Component::save_json(Entity entity)
{
  json j;

  const auto& shape = entity.get_component<CollisionShape_Component>();

  j["Shapes"] = json::object();
  for (const auto& [id, shape] : shape.shapes)
  {
    json shape_json;
    Helper::save_json(shape_json, "Shape_Type", (int)shape.shape_type);
    Helper::save_json(shape_json, "Size", shape.size);
    Helper::save_json(shape_json, "Radius", shape.radius);
    Helper::save_json(shape_json, "Offset", shape.offset);
    Helper::save_json(shape_json, "Rotation", shape.rotation);
    Helper::save_json(shape_json, "Group", shape.group);
    Helper::save_json(shape_json, "Is_Sensor", shape.is_sensor);
    Helper::save_json(shape_json, "Name", shape.name);

    shape_json["Collide_With"] = json::array();
    for (int i = 0; i < 5; i++)
    {
      shape_json["Collide_With"].push_back(shape.collide_with[i]);
    }
    j["Shapes"][id] = shape_json;
  }

  return j;
}
void CollisionShape_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<CollisionShape_Component>())
    entity.add_component<CollisionShape_Component>();

  auto& shapes = entity.get_component<CollisionShape_Component>();

  if (j.contains("Shapes"))
  {
    for (auto& [id, shape_json] : j["Shapes"].items())
    {
      CollisionShape_Data shape;
      Helper::load_json(shape_json, "Shape_Type", shape.shape_type);
      Helper::load_json(shape_json, "Size", shape.size);
      Helper::load_json(shape_json, "Radius", shape.radius);
      Helper::load_json(shape_json, "Offset", shape.offset);
      Helper::load_json(shape_json, "Rotation", shape.rotation);
      Helper::load_json(shape_json, "Group", shape.group);
      Helper::load_json(shape_json, "Is_Sensor", shape.is_sensor);
      Helper::load_json(shape_json, "Name", shape.name);

      for (int i = 0; i < 5; i++)
      {
        shape.collide_with[i] = shape_json["Collide_With"][i];
      }
      shapes.shapes[id] = shape;
    }
  }
}

void PhysicsBody_Component::add_component(Entity entity)
{
  if (!entity.has_component<PhysicsBody_Component>())
    entity.add_component<PhysicsBody_Component>();
  {
    CollisionShape_Component::add_component(entity);
    PhysicsMaterial_Component::add_component(entity);
  }
}
void PhysicsBody_Component::remove_component(Entity entity)
{
  if (entity.has_component<PhysicsBody_Component>())
  {
    auto& props = entity.get_component<PhysicsBody_Component>();
    if (props.body)
    {
      auto  scene = Scene::get_active_scene();
      auto& world = scene->get_world();
      world.DestroyBody(props.body);
      props.body = nullptr;
    }
    entity.remove_component<PhysicsBody_Component>();
  }
  entity.remove_component<CollisionShape_Component>();
  entity.remove_component<PhysicsMaterial_Component>();
}
json PhysicsBody_Component::save_json(Entity entity)
{
  json  j;
  auto& props = entity.get_component<PhysicsBody_Component>();
  Helper::save_json(j, "Body", static_cast<int>(props.body_type));
  Helper::save_json(j, "Rotation", props.rotation);

  NodeHelper::save_component<CollisionShape_Component>(entity, j);
  NodeHelper::save_component<PhysicsMaterial_Component>(entity, j);

  return j;
}
void PhysicsBody_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<PhysicsBody_Component>())
    entity.add_component<PhysicsBody_Component>();

  auto& props = entity.get_component<PhysicsBody_Component>();
  Helper::load_json(j, "Body", props.body_type);
  Helper::load_json(j, "Rotation", props.rotation);

  NodeHelper::load_component<CollisionShape_Component>(entity, j);
  NodeHelper::load_component<PhysicsMaterial_Component>(entity, j);
}
void PhysicsBody_Component::clone_entity(Entity original, Entity clone)
{
  if (!original.has_component<PhysicsBody_Component>())
    return;
  const auto&           props = original.get_component<PhysicsBody_Component>();
  PhysicsBody_Component comps;
  comps.body_type = props.body_type;
  comps.rotation  = props.rotation;

  clone.add_component<PhysicsBody_Component>(comps);

  CollisionShape_Component::clone_entity(original, clone);
  PhysicsMaterial_Component::clone_entity(original, clone);
}
void PhysicsBody_Component::update_entity(Entity entity)
{
  if (!entity.has_component<PhysicsBody_Component>() ||
      !entity.has_component<Transform_Component>() || !Engine::is_runtime())
    return;

  auto& props = entity.get_component<PhysicsBody_Component>();
  if (!props.body)
    create_body(entity);

  float angle    = props.body->GetAngle();
  vec2f position = props.body->GetPosition();
  Math::meters_to_pixels(position);
  auto& transform    = entity.get_component<Transform_Component>();
  transform.position = position;
  transform.rotation = Math::to_degree(angle);
  Transform_Component::get_local_transform(entity, transform);
}
bool PhysicsBody_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::Physics2D);
}
void PhysicsBody_Component::create_body(Entity entity)
{
  if (!entity.has_component<PhysicsBody_Component>() ||
      !entity.has_component<Transform_Component>() ||
      !entity.has_component<CollisionShape_Component>())
    return;

  auto& props     = entity.get_component<PhysicsBody_Component>();
  auto& collision = entity.get_component<CollisionShape_Component>();

  if (props.body)
    return;

  // Create body definition
  b2BodyDef body_def;
  switch (props.body_type)
  {
    case BodyType::Dynamic:
      body_def.type = b2_dynamicBody;
      break;
    case BodyType::Static:
      body_def.type = b2_staticBody;
      break;
    case BodyType::Kinematic:
      body_def.type = b2_kinematicBody;
      break;
    default:
      body_def.type = b2_dynamicBody;
      break;
  }

  Transform_Component trans = Transform_Component::get_world_transform(entity);
  Math::pixels_to_meters(trans.position);
  body_def.position.Set(trans.position.x, trans.position.y);
  body_def.angle         = Math::to_radians(trans.rotation);
  body_def.fixedRotation = !props.rotation;

  auto  scene                       = Scene::get_active_scene();
  auto& world                       = scene->get_world();
  props.body                        = world.CreateBody(&body_def);
  props.body->GetUserData().pointer = (uintptr_t)entity.get_id();

  // Get physics material if available
  float density     = 1.0f;
  float friction    = 0.3f;
  float restitution = 0.0f;

  if (entity.has_component<PhysicsMaterial_Component>())
  {
    auto& material_comps = entity.get_component<PhysicsMaterial_Component>();
    if (material_comps.preset != MaterialPreset::Custom)
      PhysicsMaterial_Component::apply_preset(material_comps);

    if (props.body_type == BodyType::Dynamic)
    {
      density     = material_comps.density;
      friction    = material_comps.friction;
      restitution = material_comps.restitution;
    }
  }

  // Create all fixtures from collision shapes
  for (auto& [id, shape_data] : collision.shapes)
  {
    b2FixtureDef fixture_def;
    fixture_def.isSensor = shape_data.is_sensor;

    // Set material properties for non-sensor dynamic bodies
    if (props.body_type == BodyType::Dynamic && !shape_data.is_sensor)
    {
      fixture_def.density     = density;
      fixture_def.friction    = friction;
      fixture_def.restitution = restitution;
    }
    else
    {
      fixture_def.density = 0.0f;
    }

    // Convert offset to meters
    vec2f offset = shape_data.offset;
    Math::pixels_to_meters(offset);

    // Create shape based on type
    if (shape_data.shape_type == ShapeType::Rectangle)
    {
      vec2f size = shape_data.size * trans.scale;
      Math::pixels_to_meters(size);

      b2PolygonShape box;
      box.SetAsBox(size.x / 2.0f, size.y / 2.0f, b2Vec2(offset.x, offset.y), shape_data.rotation);
      fixture_def.shape = &box;

      shape_data.fixture = props.body->CreateFixture(&fixture_def);
    }
    else if (shape_data.shape_type == ShapeType::Circle)
    {
      float radius = shape_data.radius * ((trans.scale.x + trans.scale.y) / 2.0f);
      Math::pixels_to_meters(radius);

      b2CircleShape circle;
      circle.m_radius = radius;
      circle.m_p.Set(offset.x, offset.y);
      fixture_def.shape = &circle;

      shape_data.fixture = props.body->CreateFixture(&fixture_def);
    }

    // Set collision filtering
    if (shape_data.fixture)
    {
      b2Filter filter;
      filter.categoryBits = 1 << (shape_data.group - 1);
      filter.maskBits     = 0;
      for (int i = 0; i < 5; i++)
      {
        if (shape_data.collide_with[i])
          filter.maskBits |= 1 << i;
      }
      shape_data.fixture->SetFilterData(filter);

      if (shape_data.is_sensor && id == "foot")
      {
        shape_data.fixture->GetUserData().pointer = 1;
      }
      else
      {
        shape_data.fixture->GetUserData().pointer = reinterpret_cast<uintptr_t>(&shape_data);
      }
    }
  }
}
void PhysicsBody_Component::recreate_fixtures(Entity entity)
{
  auto& props     = entity.get_component<PhysicsBody_Component>();
  auto& collision = entity.get_component<CollisionShape_Component>();

  if (!props.body)
    return;

  // Destroy all existing fixtures
  b2Fixture* fixture = props.body->GetFixtureList();
  while (fixture)
  {
    b2Fixture* next = fixture->GetNext();
    props.body->DestroyFixture(fixture);
    fixture = next;
  }

  // Clear fixture pointers
  for (auto& [id, shape] : collision.shapes)
  {
    shape.fixture = nullptr;
  }

  // Get transform and material properties
  Transform_Component trans = Transform_Component::get_world_transform(entity);

  float density     = 1.0f;
  float friction    = 0.3f;
  float restitution = 0.0f;

  if (entity.has_component<PhysicsMaterial_Component>())
  {
    auto& material_comps = entity.get_component<PhysicsMaterial_Component>();
    if (material_comps.preset != MaterialPreset::Custom)
      PhysicsMaterial_Component::apply_preset(material_comps);

    if (props.body_type == BodyType::Dynamic)
    {
      density     = material_comps.density;
      friction    = material_comps.friction;
      restitution = material_comps.restitution;
    }
  }

  // Recreate all fixtures
  for (auto& [id, shape_data] : collision.shapes)
  {
    b2FixtureDef fixture_def;
    fixture_def.isSensor = shape_data.is_sensor;

    // Set material properties for non-sensor dynamic bodies
    if (props.body_type == BodyType::Dynamic && !shape_data.is_sensor)
    {
      fixture_def.density     = density;
      fixture_def.friction    = friction;
      fixture_def.restitution = restitution;
    }
    else
    {
      fixture_def.density = 0.0f;
    }

    // Convert offset to meters
    vec2f offset = shape_data.offset;
    Math::pixels_to_meters(offset);

    // Create shape based on type
    if (shape_data.shape_type == ShapeType::Rectangle)
    {
      vec2f size = shape_data.size * trans.scale;
      Math::pixels_to_meters(size);

      b2PolygonShape box;
      box.SetAsBox(size.x / 2.0f, size.y / 2.0f, b2Vec2(offset.x, offset.y), shape_data.rotation);
      fixture_def.shape = &box;

      shape_data.fixture = props.body->CreateFixture(&fixture_def);
    }
    else if (shape_data.shape_type == ShapeType::Circle)
    {
      float radius = shape_data.radius * ((trans.scale.x + trans.scale.y) / 2.0f);
      Math::pixels_to_meters(radius);

      b2CircleShape circle;
      circle.m_radius = radius;
      circle.m_p.Set(offset.x, offset.y);
      fixture_def.shape = &circle;

      shape_data.fixture = props.body->CreateFixture(&fixture_def);
    }

    // Set collision filtering
    if (shape_data.fixture)
    {
      b2Filter filter;
      filter.categoryBits = 1 << (shape_data.group - 1);
      filter.maskBits     = 0;
      for (int i = 0; i < 5; i++)
      {
        if (shape_data.collide_with[i])
          filter.maskBits |= 1 << i;
      }
      shape_data.fixture->SetFilterData(filter);

      if (shape_data.is_sensor && id == "FootSensor")
      {
        shape_data.fixture->GetUserData().pointer = 1;
      }
      else
      {
        shape_data.fixture->GetUserData().pointer = reinterpret_cast<uintptr_t>(&shape_data);
      }
    }
  }
}

json PhysicsMaterial_Component::save_json(Entity entity)
{
  json        j;
  const auto& comps = entity.get_component<PhysicsMaterial_Component>();

  Helper::save_json(j, "Preset", (int)comps.preset);
  if (comps.preset == MaterialPreset::Custom)
  {
    Helper::save_json(j, "Density", comps.density);
    Helper::save_json(j, "Friction", comps.friction);
    Helper::save_json(j, "Restitution", comps.restitution);
  }
  return j;
}
void PhysicsMaterial_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<PhysicsMaterial_Component>())
    entity.add_component<PhysicsMaterial_Component>();

  auto& comps = entity.get_component<PhysicsMaterial_Component>();

  Helper::load_json(j, "Preset", comps.preset);
  apply_preset(comps);
  if (comps.preset == MaterialPreset::Custom)
  {
    Helper::load_json(j, "Density", comps.density);
    Helper::load_json(j, "Friction", comps.friction);
    Helper::load_json(j, "Restitution", comps.restitution);
  }
}
void PhysicsMaterial_Component::apply_preset(PhysicsMaterial_Component& comps)
{
  switch (comps.preset)
  {
    case MaterialPreset::Wood:
      comps.density     = 0.6f;
      comps.friction    = 0.4f;
      comps.restitution = 0.2f;
      break;
    case MaterialPreset::Metal:
      comps.density     = 8.0f;
      comps.friction    = 0.3f;
      comps.restitution = 0.1f;
      break;
    case MaterialPreset::Rubber:
      comps.density     = 1.5f;
      comps.friction    = 0.9f;
      comps.restitution = 0.8f;
      break;
    case MaterialPreset::Ice:
      comps.density     = 0.9f;
      comps.friction    = 0.05f;
      comps.restitution = 0.1f;
      break;
    case MaterialPreset::Bouncy:
      comps.density     = 0.5f;
      comps.friction    = 0.3f;
      comps.restitution = 0.95f;
      break;
    case MaterialPreset::Stone:
      comps.density     = 2.5f;
      comps.friction    = 0.6f;
      comps.restitution = 0.0f;
      break;
  }
}
bool PhysicsMaterial_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::Physics2D);
}

json Tween_Component::save_json(Entity entity)
{
  json  j;
  auto& comps = entity.get_component<Tween_Component>();

  Helper::save_json(j, "State", (int)comps.state);
  Helper::save_json(j, "EaseType", (int)comps.ease_type);
  Helper::save_json(j, "LoopType", (int)comps.loop_type);
  Helper::save_json(j, "TweenTarget", (int)comps.tween_target);
  Helper::save_json(j, "StartPos", comps.start_position);
  Helper::save_json(j, "EndPos", comps.end_position);
  Helper::save_json(j, "StartScale", comps.start_scale);
  Helper::save_json(j, "EndScale", comps.end_scale);
  Helper::save_json(j, "Rotation", comps.rotation);
  Helper::save_json(j, "Duration", comps.duration);
  Helper::save_json(j, "ElapsedTime", comps.elapsed_time);
  Helper::save_json(j, "Reverse", comps.reverse_direction);

  return j;
}
void Tween_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<Tween_Component>())
    entity.add_component<Tween_Component>();

  auto& comps = entity.get_component<Tween_Component>();

  Helper::load_json(j, "State", comps.state);
  Helper::load_json(j, "EaseType", comps.ease_type);
  Helper::load_json(j, "LoopType", comps.loop_type);
  Helper::load_json(j, "TweenTarget", comps.tween_target);
  Helper::load_json(j, "StartPos", comps.start_position);
  Helper::load_json(j, "EndPos", comps.end_position);
  Helper::load_json(j, "StartScale", comps.start_scale);
  Helper::load_json(j, "EndScale", comps.end_scale);
  Helper::load_json(j, "Rotation", comps.rotation);
  Helper::load_json(j, "Duration", comps.duration);
  Helper::load_json(j, "ElapsedTime", comps.elapsed_time);
  Helper::load_json(j, "Reverse", comps.reverse_direction);
}
bool Tween_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::Transform);
}
void Tween_Component::update(Entity entity, TimeStamp ts)
{
  if (!entity.has_component<Tween_Component>())
    return;

  auto& tween = entity.get_component<Tween_Component>();

  if (tween.state != State::PLAYING)
    return;

  float dt = ts.get_seconds();

  tween.elapsed_time += dt;

  float t = tween.duration > 0.0f ? (tween.elapsed_time / tween.duration) : 1.0f;

  if (t >= 1.0f)
  {
    switch (tween.loop_type)
    {
      case LoopType::ONCE:
        t           = 1.0f;
        tween.state = State::COMPLETED;
        break;

      case LoopType::LOOP:
        tween.elapsed_time = 0.0f;
        t                  = 0.0f;
        break;

      case LoopType::PING_PONG:
        tween.reverse_direction = !tween.reverse_direction;
        tween.elapsed_time      = 0.0f;
        t                       = 0.0f;
        break;
    }
  }

  if (tween.reverse_direction)
    t = 1.0f - t;

  float eased_t = Tween_Component::apply_ease(t, tween.ease_type);

  auto transform = Transform_Component::get_world_transform(entity);

  switch (tween.tween_target)
  {
    case TweenTarget::POSITION:
    {
      transform.position.x =
          tween.start_position.x + (tween.end_position.x - tween.start_position.x) * eased_t;
      transform.position.y =
          tween.start_position.y + (tween.end_position.y - tween.start_position.y) * eased_t;
      break;
    }

    case TweenTarget::SCALE:
    {
      transform.scale.x = tween.start_scale.x + (tween.end_scale.x - tween.start_scale.x) * eased_t;
      transform.scale.y = tween.start_scale.y + (tween.end_scale.y - tween.start_scale.y) * eased_t;
      break;
    }

    case TweenTarget::ROTATION:
    {
      transform.rotation = tween.rotation.x + (tween.rotation.y - tween.rotation.x) * eased_t;
      break;
    }
  }

  Transform_Component::get_local_transform(entity, transform);
}

bool Tween_Component::play_tween(Entity entity)
{
  if (!entity.has_component<Tween_Component>())
    return false;

  auto& tween = entity.get_component<Tween_Component>();

  tween.elapsed_time      = 0.0f;
  tween.reverse_direction = false;
  tween.state             = State::PLAYING;

  if (!entity.has_component<Transform_Component>())
    return false;

  auto transform = Transform_Component::get_world_transform(entity);

  switch (tween.tween_target)
  {
    case TweenTarget::POSITION:
      transform.position = tween.start_position;
      break;

    case TweenTarget::SCALE:
      transform.scale = tween.start_scale;
      break;

    case TweenTarget::ROTATION:
      transform.rotation = tween.rotation.x;
      break;
  }

  Transform_Component::get_local_transform(entity, transform);
  return true;
}
float Tween_Component::apply_ease(float t, EaseType ease_type)
{
  t = std::max(0.0f, std::min(1.0f, t));

  switch (ease_type)
  {
    case ag::Tween_Component::EaseType::LINEAR:
      return t;
    case ag::Tween_Component::EaseType::EASE_IN:
      return t * t;
      break;
    case ag::Tween_Component::EaseType::EASE_OUT:
      return t * (2.0 - t);
    case ag::Tween_Component::EaseType::EASE_IN_OUT:
    {
      if (t < 0.5f)
        return 2.0f * t * t;
      else
        return -1.0f + (4.0f - 2.0f * t) * t;
    }
    default:
      return t;
  }
}

json Text_Component::save_json(Entity entity)
{
  json j;

  const auto& props = entity.get_component<Text_Component>();

  Helper::save_json(j, "Text", props.text);
  Helper::save_json(j, "FontSize", props.font_size);

  return j;
}
void Text_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<Text_Component>())
    entity.add_component<Text_Component>();

  auto& props = entity.get_component<Text_Component>();
  Helper::load_json(j, "Text", props.text);
  Helper::load_json(j, "FontSize", props.font_size);
}
bool Text_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::Text);
}

json FontStyle_Component::save_json(Entity entity)
{
  json j;

  const auto& props = entity.get_component<FontStyle_Component>();

  Helper::save_json(j, "H_Allignment", (int)props.h_allignment);
  Helper::save_json(j, "V_Allignment", (int)props.v_allignment);
  Helper::save_json(j, "Style", props.style);
  Helper::save_json(j, "Line_Height", props.line_height);
  Helper::save_json(j, "Bounds", props.bounds);
  Helper::save_json(j, "Color", props.color);

  return j;
}
void FontStyle_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<FontStyle_Component>())
    entity.add_component<FontStyle_Component>();

  auto& props = entity.get_component<FontStyle_Component>();

  Helper::load_json(j, "H_Allignment", props.h_allignment);
  Helper::load_json(j, "V_Allignment", props.v_allignment);
  Helper::load_json(j, "Style", props.style);
  Helper::load_json(j, "Line_Height", props.line_height);
  Helper::load_json(j, "Bounds", props.bounds);
  Helper::load_json(j, "Color", props.color);
}
bool FontStyle_Component::is_compatible(NodeType type)
{
  auto caps = NodeHelper::get_node_capabilities(type);
  return NodeHelper::has_capability(caps, Node_Capability::Text);
}

constexpr float TEXT_EPSILON = 0.10f;
// calculate the longest text_size
vec2f Text::calc_longest_size(const Text& text, const vec2f& s)
{
  const vec2f scale = s * text.font_size / TextLoader::font.em_size;
  vec2f       size  = {0, 0};
  const float line_height =
      TextLoader::font.line_height * TextLoader::font.em_size * scale.y * text.line_height;

  if (text.bounds.x != 0 && text.bounds.y != 0)
  {
    float max_height = std::max((float)text.bounds.y, line_height);
    return vec2f(text.bounds.x, max_height);
  }

  size.y      = line_height;
  float width = 0.0f;

  for (char c : text.text)
  {
    if (c == '\n')
    {
      size.x = std::max(size.x, width);
      width  = 0.0f;
      size.y += line_height;
      continue;
    }
    if (c == '\t')
    {
      auto it = TextLoader::font.glyphs.find(' ');
      if (it != TextLoader::font.glyphs.end())
      {
        float length = (it->second.advance * scale.x) * 4.0f;
        width += length;
      }
      continue;
    }
    auto it = TextLoader::font.glyphs.find(c);
    if (it != TextLoader::font.glyphs.end())
    {
      width += it->second.advance * scale.x;
      if (width >= text.bounds.x && text.bounds.x != 0)
      {
        width -= it->second.advance * scale.x;
        size.x = std::max(size.x, width);
        width  = 0.0f;
        size.y += line_height;
      }
    }
  }
  size.x = std::max(size.x, width);
  size.y = std::max(size.y, line_height);
  return size;
}

vec2f Text::calc_line_text_size(const Text&  text,
                                const vec2f& s,
                                const vec2f& longest_size,
                                size_t       start_index,
                                size_t*      break_index)
{
  vec2f scale = s * (text.font_size / TextLoader::font.em_size);

  const float line_height =
      TextLoader::font.line_height * TextLoader::font.em_size * scale.y * text.line_height;

  float width       = 0.0f;
  float space_width = 0.0f;

  if (auto it = TextLoader::font.glyphs.find(' '); it != TextLoader::font.glyphs.end())
  {
    space_width = it->second.advance * scale.x;
  }

  size_t i = start_index;

  while (i < text.text.size())
  {
    // Handle newline
    if (text.text[i] == '\n')
    {
      *break_index = i;
      return {width, line_height};
    }

    // Skip leading spaces
    if (text.text[i] == ' ')
    {
      i++;
      continue;
    }

    float  word_width = 0.0f;
    size_t word_start = i;

    while (i < text.text.size())
    {
      char c = text.text[i];

      if (c == ' ' || c == '\n')
      {
        // AERO_CORE_INFO("I break it");
        break;
      }

      if (c == '\t')
      {
        word_width += space_width * 4.0f;
        i++;
        continue;
      }

      auto it = TextLoader::font.glyphs.find(c);
      if (it != TextLoader::font.glyphs.end())
      {
        float char_width = it->second.advance * scale.x;

        if (word_width + char_width > longest_size.x)
        {
          if (word_start != start_index)
          {
            *break_index = word_start;
            // width += word_width;
          }
          else
          {
            *break_index = i;
            width += word_width;
          }

          return {width, line_height};
        }

        word_width += char_width;
      }

      i++;
    }

    float total = width + space_width + word_width;
    if (longest_size.x > 0 && width > 0 && total - longest_size.x > TEXT_EPSILON)
    {
      *break_index = word_start;
      return {width, line_height};
    }

    // Add word
    if (width > 0)
      width += space_width;

    width += word_width;

    // Skip trailing space
    if (i < text.text.size() && text.text[i] == ' ')
      i++;
  }

  *break_index = i;
  return {width, line_height};
}
// center the text
vec2f Text::center_text(const Text&                text,
                        const Transform_Component& transform,
                        const vec2f&               longest_size)
{
  return transform.position - (longest_size * 0.5f);
}

vec2f Text::center_single_line_text(const Text&                text,
                                    const Transform_Component& transform,
                                    const vec2f&               longest_size,
                                    size_t                     start_index,
                                    size_t*                    break_index)
{
  vec2f position;
  switch (text.h_allignment)
  {
    case Text_Allignment_Horizontal::Left:
    {
      calc_line_text_size(text, transform.scale, longest_size, start_index, break_index);
      position.x = transform.position.x;
      break;
    }

    case Text_Allignment_Horizontal::Center:
    {
      vec2f size =
          calc_line_text_size(text, transform.scale, longest_size, start_index, break_index);
      position.x = transform.position.x + (longest_size * 0.5).x - (size * 0.5f).x;
      break;
    }

    case Text_Allignment_Horizontal::Right:
    {
      vec2f size =
          calc_line_text_size(text, transform.scale, longest_size, start_index, break_index);
      position.x = transform.position.x + longest_size.x - size.x;
      break;
    }
  }

  switch (text.v_allignment)
  {
    case Text_Allignment_Vertical::Top:
    {
      calc_line_text_size(text, transform.scale, longest_size, start_index, break_index);
      position.y = transform.position.y;
      break;
    }

    case Text_Allignment_Vertical::Center:
    {
      vec2f size =
          calc_line_text_size(text, transform.scale, longest_size, start_index, break_index);
      position.y = transform.position.y + (longest_size * 0.5).y - (size * 0.5f).y;
      break;
    }

    case Text_Allignment_Vertical::Bottom:
    {
      vec2f size =
          calc_line_text_size(text, transform.scale, longest_size, start_index, break_index);
      position.y = transform.position.y + longest_size.y - size.y;
      break;
    }
  }
  return position;
}

json ButtonState_Component::save_json(Entity entity)
{
  json j;

  return j;
}
void ButtonState_Component::load_json(Entity entity, const json& j)
{
}
bool ButtonState_Component::is_compatible(NodeType type)
{
  return true;
}

json Button_Visual::save_json(const Button_Visual& visual)
{
  json j;
  Helper::save_json(j, "Background", visual.background);
  Helper::save_json(j, "Border", visual.border);
  Helper::save_json(j, "Text", visual.text);
  Helper::save_json(j, "Thickness", visual.border_thickness);
  Helper::save_json(j, "Corner", visual.corner);
  return j;
}
Button_Visual Button_Visual::load_json(const json& j)
{
  Button_Visual visual;
  Helper::load_json(j, "Background", visual.background);
  Helper::load_json(j, "Border", visual.border);
  Helper::load_json(j, "Text", visual.text);
  Helper::load_json(j, "Thickness", visual.border_thickness);
  Helper::load_json(j, "Corner", visual.corner);
  return visual;
}

json Button_Layout::save_json(const Button_Layout& layout)
{
  json j;
  Helper::save_json(j, "Size", layout.size);
  Helper::save_json(j, "HAllignment", (int)layout.h_allignment);
  Helper::save_json(j, "VAllignment", (int)layout.v_allignment);
  Helper::save_json(j, "Uniform", layout.uniform);
  return j;
}
Button_Layout Button_Layout::load_json(const json& j)
{
  Button_Layout layout;
  Helper::load_json(j, "Size", layout.size);
  Helper::load_json(j, "HAllignment", layout.h_allignment);
  Helper::load_json(j, "VAllignment", layout.v_allignment);
  Helper::load_json(j, "Uniform", layout.uniform);
  return layout;
}

void Button_Component::add_component(Entity entity)
{
  Button_Component comps;
  comps.overrides[Button_Visual_State::Normal] = comps.base;

  entity.add_component<Button_Component>(comps);
}
json Button_Component::save_json(Entity entity)
{
  json  j;
  auto& comps = entity.get_component<Button_Component>();

  j["Layout"] = Button_Layout::save_json(comps.layout);

  j["Visual"] = json::object();

  comps.overrides[comps.current_state] = comps.base;

  for (const auto& [state, visual] : comps.overrides)
  {
    j["Visual"][Button_Visual::to_string(state)] = Button_Visual::save_json(visual);
  }

  return j;
}
void Button_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<Button_Component>())
    entity.add_component<Button_Component>();

  auto& comps = entity.get_component<Button_Component>();

  comps.layout = Button_Layout::load_json(j["Layout"]);

  if (j.contains("Visual") && j["Visual"].is_object())
  {
    comps.overrides.clear();

    for (const auto& [state, visual_json] : j["Visual"].items())
    {
      Button_Visual visual                               = Button_Visual::load_json(visual_json);
      comps.overrides[Button_Visual::form_string(state)] = visual;
    }
  }

  comps.base = comps.overrides[Button_Visual_State::Normal];
}

void Textured_Button_Component::add_component(Entity entity)
{
  Textured_Button_Component comps;
  comps.overrides[Button_Visual_State::Normal] = comps.base_rect;

  entity.add_component<Textured_Button_Component>(comps);
}
json Textured_Button_Component::save_json(Entity entity)
{
  json j;

  auto& comps = entity.get_component<Textured_Button_Component>();
  j["Visual"] = json::object();

  comps.overrides[comps.current_state] = comps.base_rect;
  for (const auto& [state, rect] : comps.overrides)
  {
    Helper::save_json(j["Visual"], Button_Visual::to_string(state), rect);
  }
  return j;
}

void Textured_Button_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<Textured_Button_Component>())
    entity.add_component<Textured_Button_Component>();

  auto& comps = entity.get_component<Textured_Button_Component>();

  if (j.contains("Visual") && j["Visual"].is_object())
  {
    comps.overrides.clear();

    for (const auto& [state_str, rect_json] : j["Visual"].items())
    {
      Button_Visual_State state = Button_Visual::form_string(state_str);
      uint_rect           rect;
      Helper::load_json(rect_json, rect);

      comps.overrides[state] = rect;
    }
  }

  comps.base_rect = comps.overrides[Button_Visual_State::Normal];
  if (entity.has_component<TextureRect_Component>())
  {
    auto& props = entity.get_component<TextureRect_Component>();
    props.rect  = comps.base_rect;
  }
}

json Audio_Component::save_json(Entity entity)
{
  json j;

  auto& props = entity.get_component<Audio_Component>();
  Helper::save_json(j, "Path", props.path);
  Helper::save_json(j, "Loop", props.source->is_looping());
  Helper::save_json(j, "Pitch", props.source->get_pitch());
  Helper::save_json(j, "Volume", props.source->get_volume());
  return j;
}
void Audio_Component::load_json(Entity entity, const json& j)
{
  if (!entity.has_component<Audio_Component>())
    entity.add_component<Audio_Component>();

  auto& props = entity.get_component<Audio_Component>();
  Helper::load_json(j, "Path", props.path);

  bool  loop;
  float pitch, volume;
  Helper::load_json(j, "Loop", loop);
  Helper::load_json(j, "Pitch", pitch);
  Helper::load_json(j, "Volume", volume);

  if (!props.path.empty())
  {
    props.audio_buffer = NodeHelper::create_sound(props.path);
    if (props.audio_buffer)
    {
      props.source = AudioSource::create();
      props.source->set_buffer(props.audio_buffer);
      props.source->set_loop(loop);
      props.source->set_pitch(pitch);
      props.source->set_volume(volume);
    }
  }
}
void Audio_Component::clone_entity(Entity original, Entity clone)
{
  if (!original.has_component<Audio_Component>())
    return;

  const auto&     original_props = original.get_component<Audio_Component>();
  Audio_Component comps;
  comps.path         = original_props.path;
  comps.audio_buffer = original_props.audio_buffer;

  comps.source = AudioSource::create();
  comps.source->set_buffer(comps.audio_buffer);

  clone.add_component<Audio_Component>(std::move(comps));
}
void Audio_Component::delete_entity(Entity entity)
{
}

}  // namespace ag
