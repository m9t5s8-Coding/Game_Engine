#pragma once

#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Scene/SceneComponent.hpp>
#include <string>
#include <Audio/AudioManager.hpp>


namespace ag
{

#define COMPONENT_NAME(T) #T


	template <typename T>
	struct Base_Component
	{
		static void add_component(Entity entity)
		{
			if (!entity.has_component<T>())
				entity.add_component<T>();
		}
		static void remove_component(Entity entity)
		{
			if (entity.has_component<T>())
				entity.remove_component<T>();
		}
		static json save_json(Entity entity)
		{

		}
		static void load_json(Entity entity, const json& j)
		{

		}
		static void clone_entity(Entity original, Entity clone)
		{
			if (original.has_component<T>())
				clone.add_component<T>(original.get_component<T>());
		}
		static bool is_compatible(NodeType type)
		{
			return true;
		}
		static void imgui_render(Entity entity)
		{

		}
	};



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
		static bool get_visibility(Entity entity);
		static bool get_lock(Entity entity);
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

	struct Render2D_Component : Base_Component<Render2D_Component>
	{
		vec2u size = { 100, 100 };
		Color color = Color::White;

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static bool is_compatible(NodeType type);
		static void imgui_render(Entity entity);

		static const char* get_name()
		{
			return "Render2D";
		}
	};

	struct Border_Component : Base_Component<Border_Component>
	{
		float thickness;
		Color color;

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static bool is_compatible(NodeType type);
		static void imgui_render(Entity entity);

		static const char* get_name()
		{
			return "Border";
		}
	};

	struct Corner_Component : Base_Component<Corner_Component>
	{
		float corner;
		bool uniform = true;

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static bool is_compatible(NodeType type);
		static void imgui_render(Entity entity);

		static const char* get_name()
		{
			return "Corner";
		}
	};

	struct UI_Component : Base_Component<UI_Component>
	{
		RenderMode mode = RenderMode::World;

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static bool is_compatible(NodeType type);
		static void imgui_render(Entity entity);

		static const char* get_name()
		{
			return "UI";
		}
	};

	struct Texture_Component : Base_Component<Texture_Component>
	{
		std::string path;
		AG_ref<Texture2D> texture;

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static bool is_compatible(NodeType type);
		static void imgui_render(Entity entity);

		static const char* get_name()
		{
			return "Texture";
		}
	};

	struct TextureRect_Component : Base_Component<TextureRect_Component>
	{
		uint_rect rect;

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static bool is_compatible(NodeType type);
		static void imgui_render(Entity entity, bool can_remove = true);

		static const char* get_name()
		{
			return "TextureRect";
		}
	};

	struct TextureFlip_Component : Base_Component<TextureFlip_Component>
	{
		bool horizontal = false;
		bool vertical = false;

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static bool is_compatible(NodeType type);
		static void imgui_render(Entity entity);

		static const char* get_name()
		{
			return "TextureFlip";
		}
	};

	struct Camera_Component : Base_Component<Camera_Component>
	{
		vec2f size = { 1280.0f, 720.0f };
		vec2f center;

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static bool is_compatible(NodeType type);
		static void imgui_render(Entity entity);

		static const char* get_name()
		{
			return "Camera";
		}
	};

	struct Window_Component : Base_Component<Window_Component>
	{
		vec2f size = { 1280.0f, 720.0f };

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static bool is_compatible(NodeType type);
		static void imgui_render(Entity entity);

		static const char* get_name()
		{
			return "Window";
		}
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

	struct Animation_Component : Base_Component<Animation_Component>
	{
		std::unordered_map<std::string, Animation> animations;
		std::string current_animation = "";
		int current_frame = 0;
		bool playing = true;
		float timer = 0.0f;
		bool current_animation_completed = false;
		uint_rect rect;

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static void update(Entity entity, TimeStamp ts);
		static bool play_animation(Entity entity, const std::string& name);
		static void imgui_render(Entity entity);

		static const char* get_name()
		{
			return "Animations";
		}
	};


	enum AutoTileBit : uint16_t
	{
		TL = 1 << 0,
		T = 1 << 1,
		TR = 1 << 2,
		L = 1 << 3,
		M = 1 << 4,
		R = 1 << 5,
		BL = 1 << 6,
		B = 1 << 7,
		BR = 1 << 8,
	};

	struct Tile_Defination
	{
		uint_rect texture_rect;
		bool is_solid = false;

		static json save_json(const Tile_Defination& def);

		static void load_json(Tile_Defination& def, const json& j);

		static const char* get_name()
		{
			return "Tile_Defination";
		}
	};

	struct Tile
	{
		vec2u tile_id;
		uint16_t set_id = std::numeric_limits<uint16_t>::max();
		bool use_autotile = false;

		static json save_json(const Tile& tile);

		static Tile load_json(const json& j);
	};

	struct Tile_Component : Base_Component<Tile_Component>
	{
		vec2f size = { 32, 32 };
		vec2f offset;

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static const char* get_name()
		{
			return "Tile";
		}

		static void imgui_render(Entity entity);
	};

	struct TileSet_Component : Base_Component<TileSet_Component>
	{
		std::unordered_map<vec2u, Tile_Defination, vec2_hash<AG_uint>> tile_definitions;
		std::unordered_map<vec2i, Tile, vec2_hash<int>> placed_tiles;
		bool is_tile_registered = false;
		bool tile_changed = false;
		vec2i tile_size;

		b2Body* body = nullptr;

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static void update(Entity entity);
		static void draw(Entity entity);

		static bool is_compatible(NodeType type);
		static void imgui_render(Entity entity);
		static void create_body(Entity entity);

		static const char* get_name()
		{
			return "TileSet";
		}
	};

	struct Auto_Tiles : Base_Component<Auto_Tiles>
	{
		std::unordered_map<uint16_t, vec2u> tile_bitmask;
		uint16_t set_id;

		static json save_json(const Auto_Tiles& tiles);
		static Auto_Tiles load_json(const json& j);

		static const char* get_name()
		{
			return "AutoTile";
		}
	};

	struct AutoTiling_Component : Base_Component<AutoTiling_Component>
	{
		std::unordered_map<std::string, Auto_Tiles> auto_tiles;
		uint16_t next_id;

		static void remove_component(Entity entity);

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static bool is_compatible(NodeType type);
		static void imgui_render(Entity entity);

		static const char* get_name()
		{
			return "AutoTileSet";
		}
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

	struct CollisionShape_Component : Base_Component<CollisionShape_Component>
	{
		ShapeType shape_type = ShapeType::Rectangle;

		vec2f size = { 100, 100 };
		float radius = 50.0f;

		int group = 1;
		bool collide_with[5] = { true, true, true, true, true };

		static void add_component(Entity entity);

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static void imgui_render(Entity entity);

		static const char* get_name()
		{
			return "CollisionShape";
		}

	};


	struct PhysicsBody_Component : Base_Component<PhysicsBody_Component>
	{
		b2Body* body = nullptr;
		BodyType body_type = BodyType::Dynamic;

		bool rotation = false;

		static void add_component(Entity entity);
		static void remove_component(Entity entity);

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static void clone_entity(Entity original, Entity clone);

		static void update_entity(Entity entity);
		static bool is_compatible(NodeType type);

		static void create_body(Entity entity);
		static void imgui_render(Entity entity);

		static const char* get_name()
		{
			return "PhysicsBody";
		}

	};


	enum class Text_Allignment_Horizontal
	{
		Left,
		Center,
		Right
	};
	enum class Text_Allignment_Vertical
	{
		Top,
		Center,
		Bottom
	};

	struct Text_Component : Base_Component<Text_Component>
	{
		std::string text = "AeroEngine";
		float font_size = 12.0f;
		

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static bool is_compatible(NodeType type);

		static void imgui_render(Entity entity);

		static const char* get_name()
		{
			return "Text";
		}
	};

	struct Text_Editor_State
	{
		size_t caret_index = 0;
		bool active = false;
		float blink_timer = 0.0f;

		static void add_component(Entity entity)
		{
			if (!entity.has_component<Text_Editor_State>())
			{
				Text_Editor_State state;
				state.active = false;
				if (entity.has_component<Text_Component>())
				{
					auto& props = entity.get_component<Text_Component>();
					state.caret_index = props.text.size();
				}
				entity.add_component<Text_Editor_State>(state);
			}
		}

		static void remove_component(Entity entity)
		{
			if (entity.has_component<Text_Editor_State>())
				entity.remove_component<Text_Editor_State>();
		}
	
	};

	enum FontStyle : uint8_t
	{
		dirty = 1 << 0,
		bold = 1 << 1,
		italic = 1 << 2,
		underline = 1 << 3,
	};

	struct FontStyle_Component : Base_Component<FontStyle_Component>
	{
		
		Text_Allignment_Horizontal h_allignment = Text_Allignment_Horizontal::Left;
		Text_Allignment_Vertical v_allignment = Text_Allignment_Vertical::Top;
		uint8_t style = 0;
		float line_height = 1.0f;
		vec2u bounds = { 100, 50 };
		Color color = Color::White;
		
		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static bool is_compatible(NodeType type);
		static void imgui_render(Entity entity);


		static const char* get_name()
		{
			return "FontStyle";
		}
	};

	struct Text
	{
		std::string text;
		float font_size = 48;
		RenderMode mode = RenderMode::World;
		Color text_color = Color::White;
		Text_Allignment_Horizontal h_allignment = Text_Allignment_Horizontal::Left;
		Text_Allignment_Vertical v_allignment = Text_Allignment_Vertical::Center;
		vec2f starting_pos;
		vec2f bounds;
		float line_height = 1.0f;
		vec2f padding = { 10, 10 };
		bool draw_rect = true;

		static vec2f calc_longest_size(const Text& text, const vec2f& s);
		static vec2f calc_line_text_size(const Text& text, const vec2f& s, const vec2f& longest_size, size_t start_index, size_t* break_index);
		static vec2f center_text(const Text& text, const Transform_Component& transform, const vec2f& longest_size);
		static vec2f center_single_line_text(const Text& text, const Transform_Component& transform, const vec2f& longest_size, size_t start_index, size_t* break_index);
	};





	enum Button_State : uint8_t
	{
		Normal = 1 << 0,
		Hovered = 1 << 1,
		Pressed = 1 << 2,
		Active = 1 << 3,
		Focused = 1 << 4,
		Disabled = 1 << 5
	};

	enum class Button_Visual_State : uint8_t
	{
		Normal,
		Hovered,
		Pressed,
		Focused,
		Disabled
	};

	

	struct ButtonState_Component : Base_Component<ButtonState_Component>
	{
		uint8_t button_state = static_cast<uint8_t>(Button_State::Normal);

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);
		static bool is_compatible(NodeType type);
		static const char* get_name()
		{
			return "ButtonState";
		}
	};

	struct Button_Visual
	{
		Color background = Color::White;
		Color border = Color::Black;
		Color text = Color::Black;
		float border_thickness = 1.0f;
		float corner = 5.0f;


		static json save_json(const Button_Visual& visual);
		static Button_Visual load_json(const json& j);

		static const char* get_name()
		{
			return  "ButtonVisual";
		}

		static void imgui_render(Entity entity);

		static const char* to_string(Button_Visual_State state)
		{
			switch (state)
			{
			case ag::Button_Visual_State::Normal:
				return "Normal";
			case ag::Button_Visual_State::Hovered:
				return "Hovered";
			case ag::Button_Visual_State::Pressed:
				return "Pressed";
			case ag::Button_Visual_State::Focused:
				return "Focused";
			case ag::Button_Visual_State::Disabled:
				return "Disabled";
			default:
				break;
			}
		}

		static std::vector<std::string> all_states()
		{
			std::vector<std::string> states;
			states.push_back("Normal");
			states.push_back("Hovered");
			states.push_back("Pressed");
			states.push_back("Focused");
			states.push_back("Disabled");

			return states;
		}

		static Button_Visual_State form_string(const std::string& s)
		{
			if (s == "Normal") return Button_Visual_State::Normal;
			if (s == "Hovered")    return Button_Visual_State::Hovered;
			if (s == "Pressed")  return Button_Visual_State::Pressed;
			if (s == "Focused")  return Button_Visual_State::Focused;
			if (s == "Disabled") return Button_Visual_State::Disabled;

			return Button_Visual_State::Normal;
		}
	
		static Button_Visual_State get_active_state(uint8_t mask)
		{
			if (mask & static_cast<uint8_t>(Button_State::Disabled))
				return Button_Visual_State::Disabled;

			if (mask & static_cast<uint8_t>(Button_State::Pressed) || mask & static_cast<uint8_t>(Button_State::Active))
				return Button_Visual_State::Pressed;

			if (mask & static_cast<uint8_t>(Button_State::Hovered))
				return Button_Visual_State::Hovered;

			if (mask & static_cast<uint8_t>(Button_State::Focused))
				return Button_Visual_State::Focused;

			return Button_Visual_State::Normal;
		}
	};

	struct Button_Layout
	{
		vec2u size = { 100, 20 };
		Text_Allignment_Horizontal h_allignment = Text_Allignment_Horizontal::Center;
		Text_Allignment_Vertical v_allignment = Text_Allignment_Vertical::Center;
		bool uniform = true;
		

		static json save_json(const Button_Layout& layout);
		static Button_Layout load_json(const json& j);

		static const char* get_name()
		{
			return "ButtonLayout";
		}

		static void imgui_render(Entity entity);
	};

	struct Button_Component : Base_Component<Button_Component>
	{
		Button_Visual base;
		Button_Layout layout;
		std::unordered_map<Button_Visual_State, Button_Visual> overrides;
		Button_Visual_State current_state = Button_Visual_State::Normal;


		static void add_component(Entity entity);
		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static const char* get_name()
		{
			return "Button";
		}

		static void imgui_render(Entity entity);
	};

	struct Textured_Button_Component : Base_Component<Button_Component>
	{
		uint_rect base_rect;
		std::unordered_map<Button_Visual_State, uint_rect> overrides;
		Button_Visual_State current_state = Button_Visual_State::Normal;

		static void add_component(Entity entity);
		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);

		static const char* get_name()
		{
			return "TexturedButton";
		}

		static void imgui_render(Entity entity);

	};



	struct Audio_Component : Base_Component<Audio_Component>
	{
		AG_uint audio_buffer = 0;
		AudioSource source;
		std::string path;

		static json save_json(Entity entity);
		static void load_json(Entity entity, const json& j);
		static void clone_entity(Entity original, Entity clone);
		static const char* get_name()
		{
			return "Audio";
		}
		static void imgui_render(Entity entity);
	};

}
