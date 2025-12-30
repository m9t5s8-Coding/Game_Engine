#pragma once

#include <imgui.h>
#include <string>
#include <type_traits>
#include <Math/Math.hpp>

class Texture;
class Entity;
namespace ag::UI
{
	struct MenuState {
		bool show_new_scene_dialog = false;
		bool show_save_as_dialog = false;
		bool show_about_dialog = false;
		bool show_shortcuts_dialog = false;
		bool show_project_settings = false;
		bool show_editor_settings = false;
		std::string last_opened_scene;
		std::chrono::steady_clock::time_point last_run_time;
		bool is_running = false;
	};

	struct Selector {
		bool selecting = false;
		bool has_selection = false;
		bool dragging = false;
		bool panning = false;
		vec2f select_start, select_end;
		vec2f drag_start, drag_current;
		vec2f pan_offset;
		float zoom = 1.0f;
		float zoom_speed = 0.3f;
		float min_zoom = 0.1f;
		float max_zoom = 10.0f;
		bool show_grid = true;
		int grid_size = 32;
		bool show_pixel_grid = false;
		bool show_safe_area = false;
		uint_rect safe_area = { {50, 50}, {0, 0} }; // Will be updated
		bool lock_aspect_ratio = false;
		float aspect_ratio = 1.0f;
		vec2f last_valid_selection_start;
		vec2f last_valid_selection_end;
		std::string status_message;
		float status_timer = 0.0f;
	};

	void draw_texture(Entity entity);
	void draw_animation(Entity entity);
	void draw_tilemap_register(Entity entity);

	void custom_popup(const std::string& popup_id,const std::string& popup_name, std::function<void()> draw_content, std::function<void()> close);

	void draw_menu_bar();

	void run_current_scene();

	void handle_dialogs(MenuState& state);

	void add_shortcut_row(const char* action, const char* shortcut);



	bool texture_selector(const AG_ref<Texture>& texture, uint_rect& texture_rect);

	void draw_grid(ImDrawList* draw_list, const vec2f& image_min, const vec2f& image_max,
		const vec2f& tex_size, float zoom, int grid_size, bool pixel_grid);

	void draw_safe_area(ImDrawList* draw_list, const vec2f& image_min, const vec2f& image_max,
		const vec2f& tex_size, float zoom, const uint_rect& safe_area);

	void handle_mouse_interactions(const vec2f& image_min, const vec2f& image_max,
		const vec2f& tex_size, Selector& state);

	void draw_selection(ImDrawList* draw_list, Selector& state, const vec2f& image_min,
		const vec2f& image_max, const vec2f& tex_size);

	void draw_info_overlay(ImDrawList* draw_list, const vec2f& image_min,
		const vec2f& tex_size, Selector& state, const vec2f& available_size);

	bool convert_selection_to_rect(Selector& state, const vec2f& image_min, const vec2f& image_max,
		const vec2f& tex_size, uint_rect& texture_rect);

	void render_status_bar(Selector& state);

	void render_selection_info(const uint_rect& texture_rect);


	void draw_console();
}
