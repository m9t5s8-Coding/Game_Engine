#pragma once

#include <Math/Math.hpp>
#include <Renderer/Color.hpp>
#include <Renderer/View.hpp>
#include <Scene/Entity.hpp>
#include <Project/Project.hpp>
#include <string>
#include <UI/DrawUI.hpp>
#include <cstdint>
#include <Project/FileDialogs.hpp>
#include <Helper.hpp>
#include <Scripting/LuaEnv.hpp>
#include <Scripting/LuaFunc.hpp>
#include <Scripting/ScriptableEntity.hpp>
#include <Core/Core.hpp>
#include <Events/KeyEvent.hpp>
#include <Events/MouseEvent.hpp>
#include <Events/WindowEvent.hpp>

namespace ag
{
	static std::string default_path = "assets/textures/";

	enum class RenderMode
	{
		Screen = 0,
		World = 1
	};

	enum class NodeType
	{
		Rectangle,
		Circle,
		Sprite,
		AnimatedSprite2D,
		Camera,
		TileMap
	};

	enum class RenderLayer
	{
		BackGround,
		ForeGround,
		MidGround
	};

	// SceneComponents
	struct Tag
	{
		std::string tag;
		AG_uint index = 0;
		RenderLayer layer;
		NodeType node_type;

		Entity parent;
		std::vector<Entity> children;

		static void show_properties(Entity entity)
		{
			auto& tag = entity.get_component<Tag>();
			UI::draw_string("Tag", tag.tag);
		}
	};
	struct Transform
	{
		vec2f position;
		vec2f scale = { 1, 1 };
		//vec2u origin;
		float rotation = 0.0f;

		static void show_properties(Entity entity)
		{
			UI::draw_title("Transform");
			auto& transform = entity.get_component<Transform>();
			AG_uint id = entity.get_id();
			UI::draw_vec2("Position", transform.position, { 0, 0 });
			UI::draw_vec2("Scale", transform.scale, { 1.0f, 1.0f });
			//UI::draw_vec2("Origin", transform.origin, { 0, 0 });
			UI::draw_value("Rotation", transform.rotation);
		}

		static json save(Entity entity)
		{
			json j;
			const auto& transform = entity.get_component<Transform>();
			j["Position"] = transform.position.save();
			j["Scale"] = transform.scale.save();
			j["Rotation"] = transform.rotation;

			return j;
		}

		static void load(Entity entity, const json& j)
		{
			auto& transform = entity.get_component<Transform>();

			transform.position.load(j["Position"]);
			transform.scale.load(j["Scale"]);
			transform.rotation = j["Rotation"].get<float>();
		}
	};

	struct ScriptComponent
	{
		std::string script_path = "";
		LuaEnv env;
		LuaFunc on_create;
		LuaFunc on_update;
		LuaFunc on_destroy;
		LuaFunc on_event;

		static json save_json(Entity entity)
		{
			json j;
			if (!entity.has_component<ScriptComponent>())
				return j;

			auto& comp = entity.get_component<ScriptComponent>();
			Helper::save_json(j, "Script Path", comp.script_path);

			return j;
		}

		static void load_json(Entity entity, const json& j)
		{
			std::string path = "";
			Helper::load_json(j, "Script Path", path);

			if (path.empty())
				return;
			ScriptComponent comp;
			comp.script_path = path;

			if (entity.has_component<ScriptComponent>())
			{
				auto& s_component = entity.get_component<ScriptComponent>();
				s_component = comp;
			}
			else
			{
				entity.add_component<ScriptComponent>(comp);
			}
		}

		static void create(Entity entity)
		{
			if (!entity.has_component<ScriptComponent>() && !Engine::is_runtime())
				return;
			auto& comp = entity.get_component<ScriptComponent>();
			if (comp.on_create.is_valid())
			{
				comp.on_create.call();
			}
		}

		static void update(Entity entity, TimeStamp ts)
		{
			if (!entity.has_component<ScriptComponent>() || !Engine::is_runtime())
				return;

			auto& comp = entity.get_component<ScriptComponent>();

			if (comp.on_update.is_valid())
			{
				comp.on_update.call(ts.get_seconds());
			}
		}

		static void destroy(Entity entity)
		{
			if (!entity.has_component<ScriptComponent>() && !Engine::is_runtime())
				return;

			auto& comp = entity.get_component<ScriptComponent>();

			if (comp.on_destroy.is_valid())
			{
				comp.on_destroy.call();
			}

			comp.env.get().clear();
			comp.on_create = LuaFunc();
			comp.on_update = LuaFunc();
			comp.on_destroy = LuaFunc();
			comp.on_event = LuaFunc();
		}

		static bool event(Entity entity, Event& e)
		{
			if (!entity.has_component<ScriptComponent>() || !Engine::is_runtime())
				return false;

			auto& comp = entity.get_component<ScriptComponent>();
			if (!comp.on_event.is_valid())
				return false;

			comp.on_event.call(e);

			return false;
		}

		static void load_scripts(Entity entity, const json& j)
		{
			if (!Engine::is_runtime())
				return;

			auto& comp = entity.get_component<ScriptComponent>();

			auto project = Project::get_active_project();
			std::string full_path = project->get_directory() + project->get_scripts_directory() + comp.script_path;

			comp.env.get().set_function("get_entity", [entity]() -> Entity {
				return entity;
				});

			ScriptManager::load_script(full_path, comp.env);

			

			comp.on_create.set_function(comp.env, "on_create");
			comp.on_update.set_function(comp.env, "on_update");
			comp.on_destroy.set_function(comp.env, "on_destroy");
			comp.on_event.set_function(comp.env, "on_event");
			create(entity);
		}
	};

	struct Rectangle
	{
		vec2f size;
		Color fill_color;
		float border_thickness = 0.0f;
		Color border_color;
		RenderMode mode = RenderMode::World;
	};

	struct Circle
	{
		vec2f size;
		Color fill_color;
		float border_thickness = 0.0f;
		Color border_color;
		RenderMode mode = RenderMode::World;
	};

	struct Sprite
	{
		vec2f size;
		uint_rect texture_rect;
		RenderMode mode = RenderMode::World;
	};

}
