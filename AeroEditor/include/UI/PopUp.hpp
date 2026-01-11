#pragma once
#include <string>
#include <functional>
#include <Math/Math.hpp>

namespace ag
{
	class Entity;
	struct PopUpModel
	{
		std::string name;
		std::string id;
		std::string confirm_name;
		std::string close_name;
		std::function<void()> draw_content;
		std::function<void()> on_open;
		std::function<void()> on_close;
		std::function<void()> on_confirm;
		vec2f window_size;
	};
	struct PopUpSettings
	{
		vec2f view_size;
		vec2f view_center;
		vec2f current_mouse_pos;
		vec2f last_mouse_pos;
		vec2f tile_size;
		vec2f dimension;
		bool is_dragging = false;
		bool is_confirm = false;
	};

	struct Extra_Settings
	{
		vec2f size;
		bool draw_lines = true;
		bool use_size;
		bool create_buttons = true;
	};



	struct Texture_PopUp
	{
		static void draw_popup(PopUpModel& model,Extra_Settings& extra_settings, Entity entity);
		static void draw_content(PopUpModel& model, PopUpSettings& settings, Extra_Settings& extra_settings, Entity entity);
		static void draw_lines(const vec2f& screen_pos, const vec2f& screen_size, const vec2f& size, const vec2i& line_count, const vec2i& texture_size);
		static void create_buttons(const vec2f& screen_pos, const vec2f& screen_size, const vec2i& size, const vec2i& line_count, const vec2i& texture_size, const vec2f& button_size);
	};

	struct Create_Open_Popup
	{
		static void draw_popup(PopUpModel& model);
	};
}