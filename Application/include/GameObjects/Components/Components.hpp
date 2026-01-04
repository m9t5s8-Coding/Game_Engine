#pragma once

#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Scene/SceneComponent.hpp>

namespace ag
{
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

		static void add_component(Entity entity);
		static void remove_component(Entity entity);
		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);
		static void load_children(Entity entity);
		static void clone_entity(Entity original, Entity duplicate, Entity parent);
		static bool is_compatible(NodeType type);
		static const char* get_name();
		static void imgui_render(Entity entity);
	};

	struct Transform_Component
	{
		vec2f position;
		vec2f scale = { 1.0f, 1.0f };
		float rotation = 0.0f;

		static void add_component(Entity entity);

		static void remove_component(Entity entity);

		static json save_json(Entity entity);

		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static bool is_compatible(NodeType type);

		static const char* get_name();

		static void imgui_render(Entity entity);

		static Transform_Component get_world_transform(Entity entity);

		static void get_local_transform(Entity entity, const Transform_Component& world_transform);
	};

	struct Script_Component
	{
		std::string path = "";
		LuaEnv env;
		LuaFunc on_create;
		LuaFunc on_update;
		LuaFunc on_destroy;
		LuaFunc on_event;

		static void add_component(Entity entity);

		static void remove_component(Entity entity);

		static json save_json(Entity entity);

		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static void create(Entity entity);

		static void update(Entity entity, TimeStamp ts);

		static void destroy(Entity entity);

		static bool event(Entity entity, Event& e);

		static void load_scripts(Entity entity);

		static bool is_compatible(NodeType type);

		static const char* get_name();

		static void imgui_render(Entity entity);
	};

	struct Render2D_Component
	{
		vec2u size = { 100, 100 };
		Color color = Color::White;

		static void add_component(Entity entity);

		static void remove_component(Entity entity);


		static json save_json(Entity entity);

		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static bool is_compatible(NodeType type);

		static const char* get_name();

		static void imgui_render(Entity entity);
	};

	struct Border_Component
	{
		float thickness;
		Color color;

		static void add_component(Entity entity);

		static void remove_component(Entity entity);

		static json save_json(Entity entity);

		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static bool is_compatible(NodeType type);

		static const char* get_name();

		static void imgui_render(Entity entity);
	};

	struct Corner_Component
	{
		float corner;
		bool uniform = true;

		static void add_component(Entity entity);

		static void remove_component(Entity entity);

		static json save_json(Entity entity);

		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static bool is_compatible(NodeType type);

		static const char* get_name();

		static void imgui_render(Entity entity);
	};

	struct UI_Component
	{
		RenderMode mode = RenderMode::World;

		static void add_component(Entity entity);

		static void remove_component(Entity entity);


		static json save_json(Entity entity);

		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static bool is_compatible(NodeType type);

		static const char* get_name();

		static void imgui_render(Entity entity);
	};

	struct Texture_Component
	{
		std::string path;
		AG_ref<Texture2D> texture;

		static void add_component(Entity entity);

		static void remove_component(Entity entity);

		static json save_json(Entity entity);

		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static bool is_compatible(NodeType type);

		static const char* get_name();

		static void imgui_render(Entity entity);
	};

	struct TextureRect_Component
	{
		uint_rect rect;

		static void add_component(Entity entity);

		static void remove_component(Entity entity);

		static json save_json(Entity entity);

		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static bool is_compatible(NodeType type);

		static const char* get_name();

		static void imgui_render(Entity entity);
	};

	struct TextureFlip_Component
	{
		bool horizontal = false;
		bool vertical = false;

		static void add_component(Entity entity);

		static void remove_component(Entity entity);

		static json save_json(Entity entity);

		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static bool is_compatible(NodeType type);

		static const char* get_name();

		static void imgui_render(Entity entity);
	};

	struct Camera_Component
	{
		vec2f size = { 1280.0f, 720.0f };
		vec2f center;

		static void add_component(Entity entity);

		static void remove_component(Entity entity);

		static json save_json(Entity entity);

		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static bool is_compatible(NodeType type);

		static const char* get_name();

		static void imgui_render(Entity entity);
	};

	struct Window_Component
	{
		vec2f size = { 1280.0f, 720.0f };

		static void add_component(Entity entity);

		static void remove_component(Entity entity);

		static json save_json(Entity entity);

		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static bool is_compatible(NodeType type);

		static const char* get_name();

		static void imgui_render(Entity entity);
	};

	struct Frame
	{
		uint_rect frame_rect;
	};

	struct Animation
	{
		std::string name;
		float fps = 0;
		bool loop = true;
		bool ping_pong = false;
		std::vector<Frame> frames;

		static json save_json(const Animation& animation);

		static Animation load_json(const json& j);
	};

	struct Animation_Component
	{
		std::unordered_map<std::string, Animation> animations;
		std::string current_animation = "";
		int current_frame = 0;
		bool playing = true;
		float timer = 0.0f;
		bool current_animation_completed = false;
		uint_rect rect;

		static void add_component(Entity entity);

		static void remove_component(Entity entity);

		static json save_json(Entity entity);

		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static void update(Entity entity, TimeStamp ts);

		static const char* get_name();

		static bool play_animation(Entity entity, const std::string& name);

		static void imgui_render(Entity entity);
	};



	struct Tile_Defination
	{
		uint_rect texture_rect;
		bool is_solid = true;

		static json save_json(const Tile_Defination& def);

		static void load_json(Tile_Defination& def, const json& j);
	};

	struct Tile_Component
	{
		vec2f size = { 32, 32 };
		vec2f offset;

		static void add_component(Entity entity);

		static void remove_component(Entity entity);

		static json save_json(Entity entity);

		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static const char* get_name();

		static void imgui_render(Entity entity);
	};

	struct TileSet_Component
	{
		std::unordered_map<vec2u, Tile_Defination, vec2_hash<AG_uint>> tile_definitions;
		std::unordered_map<vec2i, vec2u, vec2_hash<int>> placed_tiles;
		bool is_tile_registered = false;
		bool tile_changed = false;

		b2Body* body = nullptr;

		static void add_component(Entity entity);

		static void remove_component(Entity entity);

		static json save_json(Entity entity);

		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static void update(Entity entity);

		static void draw(Entity entity);

		static const char* get_name();

		static bool is_compatible(NodeType type);

		static void imgui_render(Entity entity);

		static void create_body(Entity entity);
	};



	enum class ShapeType
	{
		Rectangle = 0,
		Circle = 1
	};
	enum class BodyType
	{
		Dynamic = 0,
		Static = 1,
		Kinematic = 2
	};

	struct CollisionShape_Component
	{
		ShapeType shape_type = ShapeType::Rectangle;

		vec2f size = { 100, 100 };
		float radius = 50.0f;

		int group = 1;
		bool collide_with[5] = { true, true, true, true, true };

		static void add_component(Entity entity);

		static void remove_component(Entity entity);

		static json save_json(Entity entity);

		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static const char* get_name();

		static bool is_compatible(NodeType type);


		static void imgui_render(Entity entity);

	};


	struct PhysicsBody_Component
	{
		b2Body* body = nullptr;
		BodyType body_type = BodyType::Dynamic;

		bool rotation = false;

		static void add_component(Entity entity);

		static void remove_component(Entity entity);


		static json save_json(Entity entity);

		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static void delete_entity(Entity entity);

		static void update_entity(Entity entity);

		static const char* get_name();

		static bool is_compatible(NodeType type);

		static void create_body(Entity entity);

		static void imgui_render(Entity entity);

	};
	struct Text
	{
		std::string text;
		float font_size = 48;
		RenderMode mode = RenderMode::World;
		Color text_color = Color::White;
		vec2f starting_pos;

		static vec2f calc_text_size(const Text& text, const vec2f& s);

		static vec2f center_text(const Text& text, const Transform_Component& transform);
	};

}
