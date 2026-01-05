#pragma once

#include <imgui.h>
#include <string>
#include <type_traits>
#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Core/Application.hpp>
#include <UI/Icons.hpp>


namespace ag::UI
{
	

	struct Panels
	{
		bool properties_panel = true;
		bool texture_selector = false;
		bool scene_panel = true;
		bool tilemap_selector = false;
		bool console_panel = true;
		bool animation_selector = false;
		bool tilemap_register = false;
		bool auto_tiling_register = false;
		bool save_unsave_panel = false;
	};

	inline static Panels show_panels;


	struct All_PopUp
	{
		bool new_scene;
		bool open_scene;
		bool project_setting;
	};











	struct PopUpModel
	{
		std::function<void()> draw_content;
		std::function<void()> on_close;
		std::function<void()> on_confirm;
		std::function<void()> on_cancel;
		std::string id;
		std::string name;
		std::string confirm_name;
		std::string cancel_name;
		vec2f window_size = { Application::get().get_window().get_size().x * 0.7f,Application::get().get_window().get_size().x * 0.8f };
	};

	struct Selector_Window
	{

	};

	struct Window_Props
	{
		bool window_open = true;
		vec2f current_mouse_pos;
		vec2f last_mouse_pos;
		bool is_dragging = false;
	};

	enum class FileFilter {
		All,
		Images,
		Scripts,
		Assets
	};


	void draw_texture(Entity entity);



	void draw_animation(Entity entity);
	void draw_tilemap_register(Entity entity);
	void draw_autotiling_register(Entity entity);

	bool texture_selector(Entity entity, uint_rect& texture_rect);

	void content_browser();
	bool is_right_file(const std::filesystem::path& path);
	bool is_image(const std::filesystem::path& path);
	void draw_folder_node(const std::filesystem::path& directory);

	void draw_script_selector(Entity entity);
	void draw_create_script_model(bool& show_model, Entity entity);

	bool draw_tilemap_selector(Entity entity, vec2u& id, std::string& set_name, bool& use_autotile);

	void custom_popup(PopUpModel& model);

	void custom_popup(const std::string& popup_id, const std::string& popup_name, std::function<void()> draw_content, std::function<void()> close);



	void draw_menu_bar();

	void run_current_scene();





	


	void draw_console();
}
