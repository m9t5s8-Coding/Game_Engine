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
#include <Renderer/Text.hpp>
#include <Renderer/Texture.hpp>

namespace ag
{
	static std::string default_path = "assets/textures/";
	inline std::unordered_map<AG_uint, Entity> index_map;

	enum class RenderMode
	{
		Screen = 0,
		World = 1
	};

	enum class NodeType
	{
		None = -1,
		Rectangle = 0,
		Circle = 1,
		Sprite = 2,
		AnimatedSprite2D = 3,
		Camera = 4,
		TileMap = 5,
		Scene2D = 6,
		TextNode = 7,
		Button = 8,
		TextureButton = 9,
		CollisionShape = 10
	};

	enum class Node_Capability : uint32_t
	{
		None = 0,
		Render2D = 1 << 0,
		Physics2D = 1 << 1,
		Scriptable = 1 << 2,
		UI = 1 << 3,
		RectShape = 1 << 4,
		CircleShape = 1 << 5,
		Texture2D = 1 << 6,
		Transform = 1 << 7,
		Camera = 1 << 8,
		TileMap = 1 << 9
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

		bool is_visible = true;
		bool locked = false;

		AG_uint parent_id = INVALID_ENTITY;
		std::vector<AG_uint> children_id;

		static void show_properties(Entity entity)
		{
			ImGui::PushID("TagComponent");
			if (ImGui::TreeNodeEx("Tag", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
			{
				auto& tag = entity.get_component<Tag>();
				ImGui::Dummy(ImVec2(0, 2));

				UI::draw_string("Tag", tag.tag);
				ImGui::Dummy(ImVec2(0, 2));

				UI::draw_bool("Visible", tag.is_visible);
				ImGui::Dummy(ImVec2(0, 2));

				UI::draw_bool("Lock", tag.locked);
				ImGui::Dummy(ImVec2(0, 2));


				ImGui::TreePop();
			}
			ImGui::PopID();
		}

		static json save_json(Entity entity)
		{
			json j;
			if (!entity.has_component<Tag>())
				return j;

			auto& tag = entity.get_component<Tag>();

			Helper::save_json(j, "Tag", tag.tag);
			Helper::save_json(j, "NodeType", static_cast<int>(tag.node_type));
			Helper::save_json(j, "Visible", tag.is_visible);
			Helper::save_json(j, "ID", tag.index);

			if (tag.parent.get_id() != INVALID_ENTITY)
			{
				auto parent_index = tag.parent.get_component<Tag>().index;
				Helper::save_json(j, "Parent", parent_index);
			}

			if (!tag.children.empty())
			{
				json children_json = json::array();
				for (auto& child : tag.children)
				{
					auto child_index = child.get_component<Tag>().index;
					children_json.push_back(child_index);
				}
				j["Children"] = children_json;
			}
			return j;

		}

		static void load_json(Entity entity, const json& j)
		{
			auto& tag = entity.get_component<Tag>();
			Helper::load_json(j, "Tag", tag.tag);
			Helper::load_json(j, "NodeType", tag.node_type);
			Helper::load_json(j, "ID", tag.index);
			Helper::load_json(j, "Visible", tag.is_visible);

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

		static void load_children(Entity entity)
		{
			auto& tag = entity.get_component<Tag>();

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

		static void clone(Entity original, Entity duplicate, Entity parent)
		{
			const auto& original_tag = original.get_component<Tag>();
			auto& duplicate_tag = duplicate.get_component<Tag>();
			duplicate_tag.layer = original_tag.layer;
			duplicate_tag.is_visible = original_tag.is_visible;
			if (parent.get_id() != INVALID_ENTITY)
			{
				duplicate_tag.parent = parent;
				auto& parent_tag = parent.get_component<Tag>();
				parent_tag.children.push_back(duplicate);
			}
			duplicate_tag.children.clear();
			auto scene = Scene::get_active_scene();
			for (auto& children : original_tag.children)
			{
				scene->duplicate_entity(children, duplicate);
			}

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
			ImGui::PushID("TransformComponent");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));
			ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 12.0f);

			bool open = ImGui::TreeNodeEx("Transform",
				ImGuiTreeNodeFlags_DefaultOpen |
				ImGuiTreeNodeFlags_Framed |
				ImGuiTreeNodeFlags_SpanAvailWidth);

			if (open)
			{
				auto& transform = entity.get_component<Transform>();

				ImGui::Dummy(ImVec2(0, 2));
				UI::draw_vec2("Position", transform.position, { 0, 0 });

				ImGui::Dummy(ImVec2(0, 2));
				UI::draw_vec2("Scale", transform.scale, { 1.0f, 1.0f });

				ImGui::Dummy(ImVec2(0, 2));
				UI::draw_value("Rotation", transform.rotation);


				ImGui::Dummy(ImVec2(0, 2));
				ImGui::TreePop();
			}

			ImGui::PopStyleVar(3);
			ImGui::PopID();
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

		static Transform get_world_transform(Entity entity)
		{
			Transform world;
			if (!entity.has_component<Transform>())
				return world;

			world = entity.get_component<Transform>();
			auto& tag = entity.get_component<Tag>();

			if (tag.parent.get_id() != INVALID_ENTITY)
			{
				Transform parent_world = get_world_transform(tag.parent);

				world.position = parent_world.position + world.position;
				world.scale = parent_world.scale * world.scale;
				world.rotation = parent_world.rotation + world.rotation;
			}

			return world;
		}

		static void get_world(Entity entity)
		{
			if (!entity.has_component<Transform>())
				return;

			auto& world = entity.get_component<Transform>();
			auto& tag = entity.get_component<Tag>();

			if (tag.parent.get_id() != INVALID_ENTITY)
			{
				get_world(tag.parent);
				const auto& parent_world = tag.parent.get_component<Transform>();

				world.position = parent_world.position + world.position;
				world.scale = parent_world.scale * world.scale;
				world.rotation = parent_world.rotation + world.rotation;
			}

		}

		static void get_local_transform(Entity entity, const Transform& world_transform)
		{
			auto& transform = entity.get_component<Transform>();
			auto& tag = entity.get_component<Tag>();

			if (tag.parent.get_id() != INVALID_ENTITY)
			{
				Transform parent_world = get_world_transform(tag.parent);

				transform.position = world_transform.position - parent_world.position;
				transform.scale = world_transform.scale / parent_world.scale;
				transform.rotation = world_transform.rotation - parent_world.rotation;
			}
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

		static void show_properties(Entity entity)
		{

		}

		static void create(Entity entity)
		{
			if (!entity.has_component<ScriptComponent>() || !Engine::is_runtime())
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
			if (!entity.has_component<ScriptComponent>() || !Engine::is_runtime())
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

		static void load_scripts(Entity entity)
		{
			if (!Engine::is_runtime())
				return;

			if (!entity.has_component<ScriptComponent>())
				return;

			auto& comp = entity.get_component<ScriptComponent>();

			auto project = Project::get_active_project();
			std::string full_path = project->get_directory() + project->get_scripts_directory() + comp.script_path;

			comp.env.get().set_function("get_entity", [entity]() -> Entity {
				return entity;
				});

			comp.env.get().set_function("get_children", [](ag::Entity& e, const std::string& name) -> Entity {

				if (!e.has_component<Tag>())
					return {};

				auto& tag = e.get_component<Tag>();
				for (auto& child : tag.children)
				{
					auto& child_tag = child.get_component<Tag>();
					if (child_tag.tag == name)
					{
						return child;
					}
				}

				return {};
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
		float corner_radius = 0.0f;
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
		bool flip_horizontal = false;
		bool flip_vertical = false;
	};

	
}
