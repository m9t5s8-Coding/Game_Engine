#include <GameObjects/Node/RectangleNode.hpp>

namespace ag
{
  // Rectangle Node Properties
  json RectangleNode::Rectangle_Component::save(Entity entity)
  {
    json j;
    const auto& props = entity.get_component<Rectangle_Component>();

    Helper::save_json(j, "Size", props.size);
    Helper::save_json(j, "Color", props.color);

    return j;
  }
  void RectangleNode::Rectangle_Component::load(Entity entity, const json& j)
  {
    auto& props = entity.get_component<Rectangle_Component>();

    Helper::load_json(j, "Size", props.size);
    Helper::load_json(j, "Color", props.color);
  }


  // Rectangle Node
  void RectangleNode::create_node(Entity entity)
  {
    entity.add_component<Transform>();
    entity.add_component<Render2D_Component >();
  }

  void RectangleNode::delete_node(Entity entity)
  {
    ScriptComponent::destroy(entity);
    entity.delete_entity();
  }

  void RectangleNode::clone_node(Entity original, Entity clone)
  {
    clone.add_component<Transform>(original.get_component<Transform>());
    
    if (original.has_component<ScriptComponent>())
    {
      clone.add_component<ScriptComponent>(original.get_component<ScriptComponent>());
    }
    Render2D_Component::clone_entity(original, clone);
    Border_Component::clone_entity(original, clone);
    Corner_Component::clone_entity(original, clone);
    UI_Component::clone_entity(original, clone);
  }
  
  json RectangleNode::save_json(Entity entity)
  {
    json j;
    j["Transform"] = Transform::save(entity);

    NodeHelper::save_component<Render2D_Component>(entity,j, "Render2DComponent");

    NodeHelper::save_component<ScriptComponent>(entity,j, "ScriptComponent");
    NodeHelper::save_component<Border_Component>(entity,j, "BorderComponent");
    NodeHelper::save_component<Corner_Component>(entity,j, "CornerComponent");
    NodeHelper::save_component<UI_Component>(entity,j, "UIComponent");
    
    return j;
  }
  
  void RectangleNode::load_json(Entity entity, json j)
  {
    Transform::load(entity, j["Transform"]);

    NodeHelper::load_component<Render2D_Component>(entity, j, "Render2DComponent");
    NodeHelper::load_component<ScriptComponent>(entity, j, "ScriptComponent");
    NodeHelper::load_component<Border_Component>(entity, j, "BorderComponent");
    NodeHelper::load_component<Corner_Component>(entity, j, "CornerComponent");
    NodeHelper::load_component<UI_Component>(entity, j, "UIComponent");
  }
  
  void RectangleNode::update(Entity entity, TimeStamp ts)
  {
    ScriptComponent::update(entity, ts);

    auto is_visible = entity.get_component<Tag>().is_visible;
    if (!is_visible)
      return;
  }
  
  void RectangleNode::draw(Entity entity)
  {

    auto is_visible = entity.get_component<Tag>().is_visible;
    if (!is_visible)
      return;

    const auto& transform = Transform::get_world_transform(entity);
    auto& rect = entity.get_component<Rectangle_Component>();
    Rectangle rectangle;

    NodeHelper::set_value(entity, &Render2D_Component::size, rectangle.size);
    NodeHelper::set_value(entity, &Render2D_Component::color, rectangle.fill_color);


    NodeHelper::set_value(entity, &Border_Component::thickness, rectangle.border_thickness);
    NodeHelper::set_value(entity, &Border_Component::color, rectangle.border_color);

    NodeHelper::set_value(entity, &Corner_Component::corner, rectangle.corner_radius);

    if (Engine::is_runtime())
      NodeHelper::set_value(entity, &UI_Component::mode, rectangle.mode);


    Renderer2D::draw_rectangle(rectangle, transform);
  }



}