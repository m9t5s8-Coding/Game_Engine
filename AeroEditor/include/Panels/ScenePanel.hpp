#pragma once

#include <Aero.hpp>


namespace ag
{
	using EntityID = uint32_t;

	const float MOVE_THRESHOLD = 1.0f;

	enum class TransformSetting
	{
		None, Scale, Rotate, Move
	};

	enum class TransformAxis
	{
		None, X, Y
	};

	enum class TileMap_Paint_Settings
	{
		None, Paint, Line, Rectangle, Fill
	};
	enum class TileMap_Settings
	{
		Paint, Eraser
	};

	struct CreatePanelState {
		NodeType selected_prefab = NodeType::Rectangle;
		std::string search_filter;
		bool show_categories = true;
		bool show_3d_objects = false;
		bool show_lights = false;
		bool show_ui = true;
		bool show_primitives = true;
		ImVec2 icon_size = { 200.0f, 35.0f };
		ImVec2 scroll_position;
		std::unordered_map<NodeType, bool> favorites;
		std::chrono::steady_clock::time_point last_created_time;
	};

	struct HierarchyState {
		Entity dragged_entity;
		Entity drop_target;
		ImGuiID drag_drop_id = 0;
		enum DropPosition { None, Before, After, Into } drop_position = None;
		bool show_filter = false;
		std::string filter_text;
		bool show_only_selected = false;
		bool flatten_hierarchy = false;
		bool show_icons = true;
		bool auto_expand_to_selection = false;
		std::unordered_set<EntityID> expanded_nodes;
		std::unordered_set<EntityID> filtered_out_entities;
		float indent_size = 10.0f;
		ImVec4 default_text_color = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
		ImVec4 selected_color = ImVec4(0.26f, 0.59f, 0.98f, 0.3f);
		ImVec4 disabled_color = ImVec4(0.5f, 0.5f, 0.5f, 0.5f);
		ImVec4 prefab_color = ImVec4(0.8f, 0.6f, 0.2f, 1.0f);
	};

	class ScenePanel
	{
	public:
		ScenePanel(const AG_ref<Scene>& scene);
		ScenePanel() = default;
		void set_scene(const AG_ref<Scene>& scene);

		~ScenePanel() = default;

		void on_imgui_render();
		void on_event(Event& e);
		void on_update();


		TransformSetting get_transform_setting() const { return m_current_transform_setting; }
		void set_transform_setting(TransformSetting setting) { reset_transform_setting(); m_current_transform_setting = setting; }

		TransformAxis get_transform_axis() const { return m_current_transform_axis; }
		void set_transform_axis(TransformAxis axis) { m_current_transform_axis = axis; }

		TileMap_Paint_Settings get_paint_settings() const { return m_paint_settings; }
		void set_paint_settings(TileMap_Paint_Settings settings) { m_paint_settings = settings; }


		void is_mouse_inside_window(const bool inside_window) { m_mouse_inside_window = inside_window; }

		bool has_selected_entity() const { if (m_selected_entity && m_selected_entity.get_id() != INVALID_ENTITY) { return true; } return false; }
		bool selected_has_transform() { if (has_selected_entity()) { return m_selected_entity.has_component<Transform_Component>(); } }

		Entity get_selected_entity() const { return m_selected_entity; }
		void set_selected_entity(Entity entity);

		std::string categorize_node_type(NodeType type);
		bool should_show_category(const std::string& category, const CreatePanelState& state);
		void draw_object_button(NodeType type, const std::string& name, CreatePanelState& state);
		void draw_object_list_item(NodeType type, const std::string& name, CreatePanelState& state);
		void create_selected_object(NodeType type);
		bool string_contains_case_insensitive(const std::string& str, const std::string& substr);

		void draw_scene_hierarchy();

		void draw_node_hierarchy(Entity entity, int level = 0);
		void draw_properties_panel();
		void draw_create_object();
		void draw_selected_text();
		void draw_collision_shapes();

		void set_current_mouse_position(const vec2f& position) { m_current_mouse_position = position; }
		void update_transform_settings();
		void move_transform_setting();
		void rotate_transform_setting();
		void scale_transform_setting();
		void reset_transform_setting();



		void update_tilemap();

		bool texture_selector(const AG_ref<Texture2D>& texture, const vec2u& tile_size, uint_rect& texture_rect);


		void draw_scene_top_panel();

		bool on_key_pressed(KeyPressedEvent& e);
		bool on_mouse_pressed(MouseButtonPressedEvent& e);
		void tile_map_draw();


		vec2u get_tile_id() const { return m_tile_id; }
		void draw_tilemap_ghosts();

		bool on_entity_clicked();
		void draw_selection_box();

	private:
		void draw_entity_node(Entity entity, int level);
		void draw_hierarchy_toolbar();
		void draw_hierarchy_filter();
		void handle_entity_interactions(Entity entity);
		void draw_entity_context_menu(Entity entity);
		void draw_hierarchy_context_menu(Entity* entity);
		bool draw_toolbar_button(const char* icon, const char* tooltip);
		void push_entity_style(Entity entity, bool is_selected);
		std::string get_icon_for_entity(Entity entity);

		void select_entity(Entity entity);
		void update_filter();
		void expand_all_nodes();
		void collapse_all_nodes();
		void reparent_entity(EntityID child_id, EntityID new_parent_id);


		void duplicate_entity();
		void delete_entity();
		void make_root_entity();


		void paint_eraser_tiles_helper(TileSet_Component& tile_set, const vec2i& pos);
		void paint_eraser_tiles_helper(const vec2i& pos);
		void paint_tiles(TileSet_Component& tile_set, const vec2i& pos);
		void erase_tiles(TileSet_Component& tile_set, const vec2i& pos);

		void update_neighbour(TileSet_Component& tile_set, const vec2i& pos);

		uint16_t calculate_bitmask(TileSet_Component& tile_set, const vec2i& pos);
		uint16_t resolve_mask(const std::unordered_map<uint16_t, vec2u>& table, uint16_t mask);
		uint16_t remove_lowest_priority_bit(uint16_t mask);
		uint16_t normalize_autotile_mask(uint16_t mask);
		uint16_t get_set_id(const std::string& set_name);

		
		bool check_if_clicked(Entity entity);
		
	private:
		AG_ref<Scene> m_scene;
		Entity m_selected_entity;
		bool m_show_create_panel = false;

		HierarchyState m_hierarchy_state;

		vec2f m_last_mouse_position;
		vec2f m_current_mouse_position;
		bool m_mouse_inside_window;
		vec2f m_delta;

		uint_rect m_texture_rect;
		bool m_is_texture_selected;
		vec2u m_tile_id;

		bool m_move_flag = false;
		bool m_scale_flag = false;
		bool m_rotate_flag = false;

		Transform_Component m_initial_transform;
		TransformSetting m_current_transform_setting = TransformSetting::None;
		TransformAxis m_current_transform_axis = TransformAxis::None;

		TileMap_Paint_Settings m_paint_settings = TileMap_Paint_Settings::Paint;
		TileMap_Settings m_settings = TileMap_Settings::Paint , m_previous_settings = TileMap_Settings::Paint;
		std::unordered_map<vec2i, vec2u, vec2_hash<int>> temp_tiles;
		std::string m_active_set;
		bool m_use_auto_tile = false;
	};
}
