#include <Aero.hpp>
#include <Scripting/ScriptBinding/ScriptBinding.hpp>
#include <Scripting/ScriptBinding/ScriptBindingHelpers.hpp>

namespace ag {

void ScriptBinding::register_physics() {
  auto& lua         = ScriptManager::get_lua();
  auto  entity_type = lua["Entity"].get<sol::usertype<Entity>>();

  entity_type.set_function("set_velocity", [](Entity& entity, const vec2f& velocity) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;

    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;

    vec2f v = velocity;
    Math::pixels_to_meters(v);
    body->SetLinearVelocity(b2Vec2(v.x, v.y));
  });

  entity_type.set_function("apply_impulse", [](Entity& entity, const vec2f& impulse) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;

    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;

    vec2f i = impulse;
    Math::pixels_to_meters(i);
    body->ApplyLinearImpulseToCenter(i.to_b2vec2(), true);
  });

  entity_type.set_function("set_velocity_x", [](Entity& entity, float velocity) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;

    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;

    float v = Math::pixels_to_meters(velocity);
    body->SetLinearVelocity(b2Vec2(v, body->GetLinearVelocity().y));
  });

  entity_type.set_function("set_velocity_y", [](Entity& entity, float velocity) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;

    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;

    float v = Math::pixels_to_meters(velocity);
    body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, v));
  });

  entity_type.set_function("apply_impulse_x", [](Entity& entity, float impulse) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;

    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;

    float v = Math::pixels_to_meters(impulse);
    body->ApplyLinearImpulseToCenter(b2Vec2(v, 0), true);
  });

  entity_type.set_function("apply_impulse_y", [](Entity& entity, float impulse) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;

    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;

    float v = Math::pixels_to_meters(impulse);
    body->ApplyLinearImpulseToCenter(b2Vec2(0, v), true);
  });

  entity_type.set_function("get_velocity", [](Entity& entity) -> vec2f {
    if (!detail::has_component<PhysicsBody_Component>(entity)) {
      return vec2f(0.0f, 0.0f);
    }

    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return vec2f(0.0f, 0.0f);

    vec2f velocity = body->GetLinearVelocity();
    Math::meters_to_pixels(velocity);
    return velocity;
  });

  lua.set_function("set_world_gravity", [](const vec2f& gravity) {
    auto& world = Scene::get_active_scene()->get_world();
    world.SetGravity(gravity.to_b2vec2());
  });

  lua.set_function("set_world_gravity_scale", [](float scale) {
    auto& world   = Scene::get_active_scene()->get_world();
    vec2f gravity = world.GetGravity();
    gravity *= scale;
    world.SetGravity(gravity.to_b2vec2());
  });

  entity_type.set_function("set_gravity_scale", [](Entity& entity, float scale) {
    if (entity.has_component<PhysicsBody_Component>()) {
      auto body = entity.get_component<PhysicsBody_Component>().body;
      if (!body)
        return;

      body->SetGravityScale(scale);
    }
  });

  entity_type.set_function("set_awake", [](Entity& entity, bool awake) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;

    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (body) {
      body->SetAwake(awake);
    }
  });

  entity_type.set_function("is_on_ground", [](Entity& entity) -> bool {
    auto scene = Scene::get_active_scene();
    return scene->get_contact_listener()->is_grounded(entity.get_id());
  });

  // Use sol::overload for multiple signatures
  entity_type.set_function(
      "is_collided",
      sol::overload(
          [](Entity& entity1, Entity& entity2) -> bool {
            auto scene = Scene::get_active_scene();
            return scene->get_contact_listener()->is_collided(entity1.get_id(), entity2.get_id());
          },
          [](Entity& entity1) -> bool {
            auto scene = Scene::get_active_scene();
            return scene->get_contact_listener()->is_collided(entity1.get_id());
          },
          [](Entity& entity, const std::string& name) -> bool {
            auto scene = Scene::get_active_scene();
            return scene->get_contact_listener()->is_collided(entity.get_id(), name);
          }));
  entity_type.set_function("get_mass", [](Entity& entity) -> float {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return 0.0f;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return 0.0f;
    return body->GetMass();
  });

  entity_type.set_function("get_inertia", [](Entity& entity) -> float {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return 0.0f;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return 0.0f;
    return body->GetInertia();
  });

  entity_type.set_function("get_center_of_mass", [](Entity& entity) -> vec2f {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return vec2f(0, 0);
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return vec2f(0, 0);
    vec2f com = body->GetWorldCenter();
    Math::meters_to_pixels(com);
    return com;
  });

  entity_type.set_function("get_local_center_of_mass", [](Entity& entity) -> vec2f {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return vec2f(0, 0);
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return vec2f(0, 0);
    vec2f com = body->GetLocalCenter();
    Math::meters_to_pixels(com);
    return com;
  });

  entity_type.set_function("get_linear_damping", [](Entity& entity) -> float {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return 0.0f;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return 0.0f;
    return body->GetLinearDamping();
  });

  entity_type.set_function("set_linear_damping", [](Entity& entity, float damping) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;
    body->SetLinearDamping(damping);
  });

  entity_type.set_function("get_angular_damping", [](Entity& entity) -> float {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return 0.0f;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return 0.0f;
    return body->GetAngularDamping();
  });

  entity_type.set_function("set_angular_damping", [](Entity& entity, float damping) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;
    body->SetAngularDamping(damping);
  });

  entity_type.set_function("get_angular_velocity", [](Entity& entity) -> float {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return 0.0f;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return 0.0f;
    return body->GetAngularVelocity();
  });

  entity_type.set_function("set_angular_velocity", [](Entity& entity, float velocity) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;
    body->SetAngularVelocity(velocity);
  });

  entity_type.set_function("apply_torque", [](Entity& entity, float torque) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;
    body->ApplyTorque(torque, true);
  });

  entity_type.set_function("apply_angular_impulse", [](Entity& entity, float impulse) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;
    body->ApplyAngularImpulse(impulse, true);
  });

  entity_type.set_function("apply_force", [](Entity& entity, const vec2f& force) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;
    vec2f f = force;
    Math::pixels_to_meters(f);
    body->ApplyForceToCenter(f.to_b2vec2(), true);
  });

  entity_type.set_function("apply_force_at_point",
                           [](Entity& entity, const vec2f& force, const vec2f& point) {
                             if (!detail::has_component<PhysicsBody_Component>(entity))
                               return;
                             auto& body = entity.get_component<PhysicsBody_Component>().body;
                             if (!body)
                               return;
                             vec2f f = force;
                             vec2f p = point;
                             Math::pixels_to_meters(f);
                             Math::pixels_to_meters(p);
                             body->ApplyForce(f.to_b2vec2(), p.to_b2vec2(), true);
                           });

  entity_type.set_function("apply_impulse_at_point",
                           [](Entity& entity, const vec2f& impulse, const vec2f& point) {
                             if (!detail::has_component<PhysicsBody_Component>(entity))
                               return;
                             auto& body = entity.get_component<PhysicsBody_Component>().body;
                             if (!body)
                               return;
                             vec2f i = impulse;
                             vec2f p = point;
                             Math::pixels_to_meters(i);
                             Math::pixels_to_meters(p);
                             body->ApplyLinearImpulse(i.to_b2vec2(), p.to_b2vec2(), true);
                           });

  entity_type.set_function("is_awake", [](Entity& entity) -> bool {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return false;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return false;
    return body->IsAwake();
  });

  entity_type.set_function("is_active", [](Entity& entity) -> bool {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return false;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return false;
    return body->IsEnabled();
  });

  entity_type.set_function("set_active", [](Entity& entity, bool active) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;
    body->SetEnabled(active);
  });

  entity_type.set_function("is_static", [](Entity& entity) -> bool {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return false;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return false;
    return body->GetType() == b2_staticBody;
  });

  entity_type.set_function("is_dynamic", [](Entity& entity) -> bool {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return false;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return false;
    return body->GetType() == b2_dynamicBody;
  });

  entity_type.set_function("is_kinematic", [](Entity& entity) -> bool {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return false;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return false;
    return body->GetType() == b2_kinematicBody;
  });

  lua.new_enum<b2BodyType>("BodyType",
                           {
                               {   "Static",    b2_staticBody},
                               {"Kinematic", b2_kinematicBody},
                               {  "Dynamic",   b2_dynamicBody}
  });

  entity_type.set_function("set_body_type", [](Entity& entity, b2BodyType type) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;
    body->SetType(type);
  });

  entity_type.set_function("get_body_type", [&lua](Entity& entity) -> sol::object {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return sol::nil;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return sol::nil;
    return sol::make_object(lua, body->GetType());
  });

  entity_type.set_function("get_fixed_rotation", [](Entity& entity) -> bool {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return false;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return false;
    return body->IsFixedRotation();
  });

  entity_type.set_function("set_fixed_rotation", [](Entity& entity, bool fixed) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;
    body->SetFixedRotation(fixed);
  });

  entity_type.set_function("is_bullet", [](Entity& entity) -> bool {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return false;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return false;
    return body->IsBullet();
  });

  entity_type.set_function("set_bullet", [](Entity& entity, bool bullet) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;
    body->SetBullet(bullet);
  });

  entity_type.set_function("get_friction", [](Entity& entity) -> float {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return 0.0f;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return 0.0f;
    auto* fixture = body->GetFixtureList();
    if (!fixture)
      return 0.0f;
    return fixture->GetFriction();
  });

  entity_type.set_function("set_friction", [](Entity& entity, float friction) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;
    auto* fixture = body->GetFixtureList();
    if (!fixture)
      return;
    fixture->SetFriction(friction);
  });

  entity_type.set_function("get_restitution", [](Entity& entity) -> float {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return 0.0f;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return 0.0f;
    auto* fixture = body->GetFixtureList();
    if (!fixture)
      return 0.0f;
    return fixture->GetRestitution();
  });

  entity_type.set_function("set_restitution", [](Entity& entity, float restitution) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;
    auto* fixture = body->GetFixtureList();
    if (!fixture)
      return;
    fixture->SetRestitution(restitution);
  });

  entity_type.set_function("get_density", [](Entity& entity) -> float {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return 0.0f;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return 0.0f;
    auto* fixture = body->GetFixtureList();
    if (!fixture)
      return 0.0f;
    return fixture->GetDensity();
  });

  entity_type.set_function("set_density", [](Entity& entity, float density) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;
    auto* fixture = body->GetFixtureList();
    if (!fixture)
      return;
    fixture->SetDensity(density);
    body->ResetMassData();  // must recalc after density change
  });

  entity_type.set_function("is_sensor", [](Entity& entity) -> bool {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return false;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return false;
    auto* fixture = body->GetFixtureList();
    if (!fixture)
      return false;
    return fixture->IsSensor();
  });

  entity_type.set_function("set_sensor", [](Entity& entity, bool sensor) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;
    auto* fixture = body->GetFixtureList();
    if (!fixture)
      return;
    fixture->SetSensor(sensor);
  });

  entity_type.set_function("get_world_gravity", [](Entity& entity) -> vec2f {
    auto& world = Scene::get_active_scene()->get_world();
    vec2f g     = world.GetGravity();
    Math::meters_to_pixels(g);
    return g;
  });

  entity_type.set_function("get_physics_position", [](Entity& entity) -> vec2f {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return vec2f(0, 0);
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return vec2f(0, 0);
    vec2f pos = body->GetPosition();
    Math::meters_to_pixels(pos);
    return pos;
  });

  entity_type.set_function("get_physics_angle", [](Entity& entity) -> float {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return 0.0f;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return 0.0f;
    return body->GetAngle();
  });

  entity_type.set_function("disable_collider", [](Entity& entity, bool enabled) {
    if (!detail::has_component<PhysicsBody_Component>(entity))
      return;
    auto& body = entity.get_component<PhysicsBody_Component>().body;
    if (!body)
      return;

    for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
      b2Filter filter;
      if (!enabled) {
        filter.categoryBits = 0x0000;
        filter.maskBits     = 0x0000;
        f->SetFilterData(filter);
      } else {
        PhysicsBody_Component::recreate_fixtures(entity);
      }
    }
  });
}

}  // namespace ag
