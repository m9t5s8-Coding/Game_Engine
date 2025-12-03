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
     
    }

    static void delete_node(Entity entity)
    {
      ScriptComponent::destroy(entity);
      entity.delete_entity();
    }

    static void clone_node(Entity original, Entity clone)
    {
      if (original.has_component<ScriptComponent>())
      {
        clone.add_component<ScriptComponent>(original.get_component<ScriptComponent>());
      }
    }

    static json save_json(Entity entity)
    {
      json j;
      if (entity.has_component<ScriptComponent>())
      {
        j["ScriptComponent"] = ScriptComponent::save_json(entity);
      }

      return j;
    }

    static void load_json(Entity entity, json j)
    {
      if (j.contains("ScriptComponent"))
      {
        ScriptComponent::load_json(entity, j["ScriptComponent"]);
      }
    }

    static void show_properties(Entity entity)
    {
      {
        Tag::show_properties(entity);
      }
    }

    static void draw(Entity entity, TimeStamp ts)
    {
      ScriptComponent::update(entity, ts);
    }

  };
}
