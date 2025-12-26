#pragma once

#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Renderer/Renderer2D.hpp>
#include <Scene/SceneComponent.hpp>
#include <GameObjects/GameObjects.hpp>

namespace ag
{

	struct Render2D_Component
	{
		vec2u size;
		Color color;

		static json save_json(Entity entity)
		{
			json j;
			auto& props = entity.get_component<Render2D_Component >();
			Helper::save_json(j, "Size", props.size);
			Helper::save_json(j, "Color", props.color);

			return j;
		}

		static void load_json(Entity entity, const json& j)
		{
			if (!entity.has_component<Render2D_Component >())
				entity.add_component<Render2D_Component >();

			auto& props = entity.get_component<Render2D_Component >();
			Helper::load_json(j, "Size", props.size);
			Helper::load_json(j, "Color", props.color);
		}

		static void clone_entity(Entity original, Entity clone)
		{
			if (original.has_component<Render2D_Component >())
			{
				clone.add_component<Render2D_Component >(original.get_component<Render2D_Component >());
			}
		}

		static bool is_compatible(NodeType type)
		{
			auto caps = NodeHelper::get_node_capabilities(type);
			return NodeHelper::has_capability(caps, Node_Capability::Render2D);
		}

		static const char* get_name()
		{
			return "Render2D Component";
		}

		static void imgui_render(Entity entity);
	};

	struct Border_Component
	{
		float thickness;
		Color color;

		static json save_json(Entity entity)
		{
			json j;

			auto& props = entity.get_component<Border_Component>();
			Helper::save_json(j, "Thickness", props.thickness);
			Helper::save_json(j, "Color", props.color);

			return j;
		}

		static void load_json(Entity entity, const json& j)
		{
			if (!entity.has_component<Border_Component>())
				entity.add_component<Border_Component>();

			auto& props = entity.get_component<Border_Component>();
			Helper::load_json(j, "Thickness", props.thickness);
			Helper::load_json(j, "Color", props.color);
		}
	
		static void clone_entity(Entity original, Entity clone)
		{
			if (original.has_component<Border_Component>())
			{
				clone.add_component<Border_Component>(original.get_component<Border_Component>());
			}
		}
	
		static bool is_compatible(NodeType type)
		{
			auto caps = NodeHelper::get_node_capabilities(type);
			return NodeHelper::has_capability(caps, Node_Capability::RectShape) ||
				NodeHelper::has_capability(caps, Node_Capability::CircleShape);
		}
		
		static const char* get_name()
		{
			return "Border Component";
		}
	
		static void imgui_render(Entity entity);
	};

	struct Corner_Component
	{
		float corner;
		bool uniform = true;

		static json save_json(Entity entity)
		{
			json j;

			auto& props = entity.get_component<Corner_Component>();
			Helper::save_json(j, "Corner", props.corner);
			Helper::save_json(j, "Uniform", props.uniform);

			return j;
		}

		static void load_json(Entity entity, const json& j)
		{
			if (!entity.has_component<Corner_Component>())
				entity.add_component<Corner_Component>();

			auto& props = entity.get_component < Corner_Component>();
			Helper::load_json(j, "Corner", props.corner);
			Helper::load_json(j, "Uniform", props.uniform);
		}
	
		static void clone_entity(Entity original, Entity clone)
		{
			if (original.has_component<Corner_Component>())
			{
				clone.add_component<Corner_Component>(original.get_component<Corner_Component>());
			}
		}
	
		static bool is_compatible(NodeType type)
		{
			auto caps = NodeHelper::get_node_capabilities(type);
			return NodeHelper::has_capability(caps, Node_Capability::RectShape);
		}
		
		static const char* get_name()
		{
			return "Corner Component";
		}
	
		static void imgui_render(Entity entity);
	};

	struct UI_Component
	{
		RenderMode mode = RenderMode::World;

		static json save_json(Entity entity)
		{
			json j;

			auto& props = entity.get_component<UI_Component>();
			Helper::save_json(j, "Mode", static_cast<int>(props.mode));

			return j;
		}

		static void load_json(Entity entity, const json& j)
		{
			if (!entity.has_component<UI_Component>())
				entity.add_component<UI_Component>();

			auto& props = entity.get_component<UI_Component>();
			Helper::load_json(j, "Mode", props.mode);
		}
	
		static void clone_entity(Entity original, Entity clone)
		{
			if (original.has_component<UI_Component>())
			{
				clone.add_component<UI_Component>(original.get_component<UI_Component>());
			}
		}

		static bool is_compatible(NodeType type)
		{
			auto caps = NodeHelper::get_node_capabilities(type);
			return NodeHelper::has_capability(caps, Node_Capability::UI);
		}
		
		static const char* get_name()
		{
			return "UI Component";
		}
	
		static void imgui_render(Entity entity);
	};
}