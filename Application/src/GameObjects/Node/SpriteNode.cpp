#include <GameObjects/Node/SpriteNode.hpp>
#include <Renderer/Renderer2D.hpp>

namespace ag
{

  void SpriteNode::create_node(Entity entity)
  {
    entity.add_component<Transform_Component>();
    entity.add_component<Render2D_Component>();
    entity.add_component<Texture_Component>();
  }
  
  void SpriteNode::delete_node(Entity entity)
  {
    ScriptComponent::destroy(entity);
    entity.delete_entity();
  }
  
  void SpriteNode::clone_node(Entity original, Entity clone)
  {
    Script_Component::clone_entity(original, clone);
    Transform_Component::clone_entity(original, clone);
    Render2D_Component::clone_entity(original, clone);

    Texture_Component::clone_entity(original, clone);
    TextureFlip_Component::clone_entity(original, clone);
    TextureRect_Component::clone_entity(original, clone);

    UI_Component::clone_entity(original, clone);
  }
  
  json SpriteNode::save_json(Entity entity)
  {
    json j;
    
    NodeHelper::save_component<Transform_Component>(entity, j);
    NodeHelper::save_component<Render2D_Component>(entity, j);
    NodeHelper::save_component<Texture_Component>(entity, j);
    NodeHelper::save_component<TextureFlip_Component>(entity, j);
    NodeHelper::save_component<TextureRect_Component>(entity, j);
    NodeHelper::save_component<Script_Component>(entity, j);
    NodeHelper::save_component<UI_Component>(entity, j);
    return j;
  }
  
  void SpriteNode::load_json(Entity entity, const json& j)
  {
    NodeHelper::load_component<Transform_Component>(entity, j);
    NodeHelper::load_component<Render2D_Component>(entity, j);
    NodeHelper::load_component<Script_Component>(entity, j);
    NodeHelper::load_component<Texture_Component>(entity, j);
    NodeHelper::load_component<TextureFlip_Component>(entity, j);
    NodeHelper::load_component<TextureRect_Component>(entity, j);
    NodeHelper::load_component<UI_Component>(entity, j);
  }
  
  void SpriteNode::update(Entity entity, TimeStamp ts)
  {
    Script_Component::update(entity, ts);
  }
  
  void SpriteNode::draw(Entity entity)
  {
    if (!entity.get_component<Tag_Component>().visible)
      return;

    const auto& transform = Transform_Component::get_world_transform(entity);
    Sprite sprite;

    NodeHelper::set_value(entity, &Render2D_Component::size, sprite.size);

    if (entity.has_component<Texture_Component>())
      Renderer2D::set_texture(entity.get_component<Texture_Component>().texture);

    NodeHelper::set_value(entity, &TextureRect_Component::rect, sprite.texture_rect);

    NodeHelper::set_value(entity, &TextureFlip_Component::horizontal, sprite.flip_horizontal);
    NodeHelper::set_value(entity, &TextureFlip_Component::vertical, sprite.flip_vertical);

    if (Engine::is_runtime())
      NodeHelper::set_value(entity, &UI_Component::mode, sprite.mode);


    Renderer2D::draw_sprite(sprite, transform);
  }

}