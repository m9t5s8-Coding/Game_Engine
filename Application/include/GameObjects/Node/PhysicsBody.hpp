#pragma once

#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag
{
  struct PhysicsBody
  {
    struct PhysicsBodyProps
    {
      b2BodyId body;
      b2ShapeId shape;

      bool dynamic = true;

      static json save(Entity entity)
      {
        json j;
       

        return j;
      }

      static void load(Entity entity, const json& j)
      {
        
      }
    };

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
     
    }

    static json save_json(Entity entity)
    {
      json j;
      

      return j;
    }

    static void load_json(Entity entity, json j)
    {
    

    }

    static void show_properties(Entity entity)
    {
     
    }

    static void update(Entity entity, TimeStamp ts)
    {
      ScriptComponent::update(entity, ts);

      auto is_visible = entity.get_component<Tag>().is_visible;
      if (!is_visible)
        return;
    }

    static void draw(Entity entity)
    {

      auto is_visible = entity.get_component<Tag>().is_visible;
      if (!is_visible)
        return;

     
    }

  };
}
