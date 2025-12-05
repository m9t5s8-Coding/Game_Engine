#pragma once

#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag
{
  struct Scene2D
  {
    static void create_node(Entity entity)
    {
      entity.add_component<Transform>();
    }

    static void delete_node(Entity entity)
    {
      ScriptComponent::destroy(entity);
      entity.delete_entity();
    }

    static void clone_node(Entity original, Entity clone)
    {
      clone.add_component<Transform>(original.get_component<Transform>());
      if (original.has_component<ScriptComponent>())
      {
        clone.add_component<ScriptComponent>(original.get_component<ScriptComponent>());
      }
    }

    static json save_json(Entity entity)
    {
      json j;
      j["Transform"] = Transform::save(entity);
      if (entity.has_component<ScriptComponent>())
      {
        j["ScriptComponent"] = ScriptComponent::save_json(entity);
      }

      return j;
    }

    static void load_json(Entity entity, json j)
    {
      if (j.contains("Transform"))
      {
        Transform::load(entity, j["Transform"]);
      }
      
      if (j.contains("ScriptComponent"))
      {
        ScriptComponent::load_json(entity, j["ScriptComponent"]);
      }
    }

    static void show_properties(Entity entity)
    {
      {
        Tag::show_properties(entity);
        Transform::show_properties(entity);
      }
    }

    static void update(Entity entity, TimeStamp ts)
    {
      ScriptComponent::update(entity, ts);
    }

    static void draw(Entity entity)
    {
      
    }

  };
}
