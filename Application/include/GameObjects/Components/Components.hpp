#pragma once

#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Scene/SceneComponent.hpp>
#include <GameObjects/GameObjects.hpp>

namespace ag
{
	//Tag Component
	struct Tag_Component
	{
		std::string name;
		AG_uint index = 0;
		NodeType node_type;
		Entity parent;
		std::vector<Entity> children;

		bool visible = true;
		bool locked = false;
		AG_uint parent_id = INVALID_ENTITY;
		std::vector<AG_uint> children_id;

		static json save_json(Entity entity)
		{
			json j;
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

		static void load_json(Entity entity, const json& j)
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

		static void load_children(Entity entity)
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

		static void clone_entity(Entity original, Entity duplicate, Entity parent)
		{
			const auto& original_tag = original.get_component<Tag_Component>();
			auto& duplicate_tag = duplicate.get_component<Tag_Component>();
			duplicate_tag.visible = original_tag.visible;
			if (parent.get_id() != INVALID_ENTITY)
			{
				duplicate_tag.parent = parent;
				auto& parent_tag = parent.get_component<Tag_Component>();
				parent_tag.children.push_back(duplicate);
			}
			duplicate_tag.children.clear();
			auto scene = Scene::get_active_scene();
			for (auto& children : original_tag.children)
			{
				scene->duplicate_entity(children, duplicate);
			}

		}

		static bool is_compatible(NodeType type)
		{
			return true;
		}

		static const char* get_name()
		{
			return "Tag";
		}

		static void imgui_render(Entity entity);
	};

	struct Transform_Component
	{
		vec2f position;
		vec2f scale = { 1.0f, 1.0f };
		float rotation = 0.0f;

		static json save_json(Entity entity)
		{
			json j;
			const auto& props = entity.get_component<Transform_Component>();
			Helper::save_json(j, "Position", props.position);
			Helper::save_json(j, "Scale", props.scale);
			Helper::save_json(j, "Rotation", props.rotation);

			return j;
		}

		static void load_json(Entity entity, const json& j)
		{
			if (!entity.has_component<Transform_Component>())
				entity.add_component<Transform_Component>();

			auto& props = entity.get_component<Transform_Component>();
			Helper::load_json(j, "Position", props.position);
			Helper::load_json(j, "Scale", props.scale);
			Helper::load_json(j, "Rotation", props.rotation);
		}

		static void clone_entity(Entity original, Entity clone)
		{
			if (original.has_component<Transform_Component>())
			{
				clone.add_component<Transform_Component>(original.get_component<Transform_Component>());
			}
		}

		static bool is_compatible(NodeType type)
		{
			auto caps = NodeHelper::get_node_capabilities(type);
			return NodeHelper::has_capability(caps, Node_Capability::Render2D);
		}

		static const char* get_name()
		{
			return "Transform";
		}

		static void imgui_render(Entity entity);

		static Transform_Component get_world_transform(Entity entity)
		{
			Transform_Component world_transform;
			if (!entity.has_component<Transform_Component>())
				return world_transform;

			world_transform = entity.get_component<Transform_Component>();
			auto& tag = entity.get_component<Tag_Component>();

			if (tag.parent.get_id() != INVALID_ENTITY)
			{
				Transform_Component parent_world = get_world_transform(tag.parent);

				world_transform.position = parent_world.position + world_transform.position;
				world_transform.scale = parent_world.scale * world_transform.scale;
				world_transform.rotation = parent_world.rotation + world_transform.rotation;
			}
			return world_transform;
		}

		static void get_local_transform(Entity entity, const Transform_Component& world_transform)
		{
			auto& transform = entity.get_component<Transform_Component>();
			auto& tag = entity.get_component<Tag>();

			if (tag.parent.get_id() != INVALID_ENTITY)
			{
				Transform_Component parent_world = get_world_transform(tag.parent);
				transform.position = world_transform.position - parent_world.position;
				transform.scale = world_transform.scale / parent_world.scale;
				transform.rotation = world_transform.rotation - parent_world.rotation;
			}
		}
	};

	struct Script_Component
	{
		std::string path = "";
		LuaEnv env;
		LuaFunc on_create;
		LuaFunc on_update;
		LuaFunc on_destroy;
		LuaFunc on_event;

		static json save_json(Entity entity)
		{
			json j;

			auto& comp = entity.get_component<Script_Component>();
			Helper::save_json(j, "Path", comp.path);

			return j;
		}

		static void load_json(Entity entity, const json& j)
		{
			if (entity.has_component<Script_Component>())
				entity.add_component<Script_Component>();

			auto& props = entity.get_component<Script_Component>();
			Helper::load_json(j, "Path", props.path);
		}

		static void clone_entity(Entity original, Entity clone)
		{
			if (original.has_component<Script_Component>())
			{
				const auto& original_path = original.get_component<Script_Component>().path;
				Script_Component comps;
				comps.path = original_path;
				clone.add_component<Script_Component>(comps);
				load_scripts(clone);
			}
		}

		static void create(Entity entity)
		{
			if (!entity.has_component<Script_Component>() || !Engine::is_runtime())
				return;
			auto& comp = entity.get_component<Script_Component>();
			if (comp.on_create.is_valid())
			{
				comp.on_create.call();
			}
		}

		static void update(Entity entity, TimeStamp ts)
		{
			if (!entity.has_component<Script_Component>() || !Engine::is_runtime())
				return;

			auto& comp = entity.get_component<Script_Component>();

			if (comp.on_update.is_valid())
			{
				comp.on_update.call(ts.get_seconds());
			}
		}

		static void destroy(Entity entity)
		{
			if (!entity.has_component<Script_Component>() || !Engine::is_runtime())
				return;

			auto& comp = entity.get_component<Script_Component>();

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
			if (!entity.has_component<Script_Component>() || !Engine::is_runtime())
				return false;

			auto& comp = entity.get_component<Script_Component>();
			if (!comp.on_event.is_valid())
				return false;

			comp.on_event.call(e);

			return false;
		}

		static void load_scripts(Entity entity)
		{
			if (!Engine::is_runtime() || !entity.has_component<Script_Component>())
				return;

			auto& comp = entity.get_component<Script_Component>();

			auto project = Project::get_active_project();
			std::string full_path = project->get_directory() + project->get_scripts_directory() + comp.path;

			comp.env.get().set_function("get_entity", [entity]() -> Entity {
				return entity;
				});

			comp.env.get().set_function("get_children", [](ag::Entity& e, const std::string& name) -> Entity {

				if (!e.has_component<Tag_Component>())
					return {};

				auto& tag = e.get_component<Tag_Component>();
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
	
		static bool is_compatible(NodeType type)
		{
			return true;
		}

		static const char* get_name()
		{
			return "Script";
		}
	
		static void imgui_render(Entity entity);
	};


	struct Render2D_Component
	{
		vec2u size = { 100, 100 };
		Color color = Color::White;

		static json save_json(Entity entity)
		{
			json j;
			auto& props = entity.get_component<Render2D_Component>();
			Helper::save_json(j, "Size", props.size);
			Helper::save_json(j, "Color", props.color);

			return j;
		}

		static void load_json(Entity entity, const json& j)
		{
			if (!entity.has_component<Render2D_Component>())
				entity.add_component<Render2D_Component>();

			auto& props = entity.get_component<Render2D_Component >();
			Helper::load_json(j, "Size", props.size);
			Helper::load_json(j, "Color", props.color);
		}

		static void clone_entity(Entity original, Entity clone)
		{
			if (original.has_component<Render2D_Component>())
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
			return "Render2D";
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
			return "Border";
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
			return "Corner";
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
			return "UI";
		}

		static void imgui_render(Entity entity);
	};

	struct Texture_Component
	{
		std::string path;
		AG_ref<Texture2D> texture;

		static json save_json(Entity entity)
		{
			json j;

			auto& props = entity.get_component<Texture_Component>();
			Helper::save_json(j, "Path", props.path);

			return j;
		}

		static void load_json(Entity entity, const json& j)
		{
			if (!entity.has_component<Texture_Component>())
				entity.add_component<Texture_Component>();

			auto& props = entity.get_component<Texture_Component>();
			Helper::load_json(j, "Path", props.path);

			props.texture = NodeHelper::load_texture(props.path);
		}

		static void clone_entity(Entity original, Entity clone)
		{
			if (original.has_component<Texture_Component>())
			{
				clone.add_component<Texture_Component>(original.get_component<Texture_Component>());
			}
		}

		static bool is_compatible(NodeType type)
		{
			auto caps = NodeHelper::get_node_capabilities(type);
			return NodeHelper::has_capability(caps, Node_Capability::Texture2D);
		}

		static const char* get_name()
		{
			return "Texture";
		}

		static void imgui_render(Entity entity);
	};

	struct TextureRect_Component
	{
		uint_rect rect;

		static json save_json(Entity entity)
		{
			json j;

			auto& props = entity.get_component<TextureRect_Component>();
			Helper::save_json(j, "Rect", props.rect);

			return j;
		}

		static void load_json(Entity entity, const json& j)
		{
			if (!entity.has_component<TextureRect_Component>())
				entity.add_component<TextureRect_Component>();

			auto& props = entity.get_component<TextureRect_Component>();
			Helper::load_json(j, "Rect", props.rect);

		}

		static void clone_entity(Entity original, Entity clone)
		{
			if (original.has_component<TextureRect_Component>())
			{
				clone.add_component<TextureRect_Component>(original.get_component<TextureRect_Component>());
			}
		}

		static bool is_compatible(NodeType type)
		{
			auto caps = NodeHelper::get_node_capabilities(type);
			return NodeHelper::has_capability(caps, Node_Capability::Texture2D);
		}

		static const char* get_name()
		{
			return "TextureRect";
		}

		static void imgui_render(Entity entity);
	};

	struct TextureFlip_Component
	{
		bool horizontal = false;
		bool vertical = false;

		static json save_json(Entity entity)
		{
			json j;

			auto& props = entity.get_component<TextureFlip_Component>();
			Helper::save_json(j, "Horizontal", props.horizontal);
			Helper::save_json(j, "Vertical", props.vertical);

			return j;
		}

		static void load_json(Entity entity, const json& j)
		{
			if (!entity.has_component<TextureFlip_Component>())
				entity.add_component<TextureFlip_Component>();

			auto& props = entity.get_component<TextureFlip_Component>();
			Helper::load_json(j, "Horizontal", props.horizontal);
			Helper::load_json(j, "Vertical", props.vertical);

		}

		static void clone_entity(Entity original, Entity clone)
		{
			if (original.has_component<TextureFlip_Component>())
			{
				clone.add_component<TextureFlip_Component>(original.get_component<TextureFlip_Component>());
			}
		}

		static bool is_compatible(NodeType type)
		{
			auto caps = NodeHelper::get_node_capabilities(type);
			return NodeHelper::has_capability(caps, Node_Capability::Texture2D);
		}

		static const char* get_name()
		{
			return "TextureFlip";
		}

		static void imgui_render(Entity entity);
	};







	struct Text
	{
		std::string text;
		float font_size = 48;
		RenderMode mode = RenderMode::World;
		Color text_color = Color::White;
		vec2f starting_pos;

		static vec2f calc_text_size(const Text& text, const vec2f& s)
		{
			vec2f scale;
			scale = s * (text.font_size / TextLoader::font.em_size);
			vec2f size = { 0, 0 };
			float line_height = (TextLoader::font.ascender - TextLoader::font.descender) * TextLoader::font.em_size * scale.y;
			size.y = line_height;
			float width = 0.0f;

			for (char c : text.text)
			{
				if (c == '\n')
				{
					size.x = std::max(size.x, width);
					width = 0.0f;
					size.y += line_height;
					continue;
				}
				auto it = TextLoader::font.glyphs.find(c);
				if (it != TextLoader::font.glyphs.end())
				{
					width += it->second.advance * scale.x;
				}
			}
			size.x = std::max(size.x, width);
			return size;
		}

		static vec2f center_text(const Text& text, const Transform_Component& transform)
		{
			vec2f size = calc_text_size(text, transform.scale);
			return transform.position - (size * 0.5f);
		}
	};

}