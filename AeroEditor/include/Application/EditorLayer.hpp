#pragma once

#include <Aero.hpp>
#include <iostream>

#include <imgui.h>
#include <Panels/ScenePanel.hpp>
#include <Panels/Exportpanel.hpp>
#include <unordered_map>

namespace ag
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void on_attach() override;
		virtual void on_detach() override;

		virtual void on_update(TimeStamp ts) override;
		virtual void on_imgui_render() override;
		virtual void on_event(Event& e) override;

		ag::vec2f get_viewport_mouse_position() const { return m_current_mouse_pos; }
		vec2f get_last_mouse_position() const { return m_last_mouse_pos; }

		bool is_viewport_hovered() const { return m_viewport_hovered; }

		static EditorLayer& get() { return *s_instance; }
	

		float_rect get_float_rect() { return m_view_controller->get_view().get_float_rect(); }
		vec2f get_viewport_size() { return m_viewport_size; }

		View& get_view() { return m_view_controller->get_view(); }

		void set_remove_scene(const std::string& remove_scene) { m_scene_to_remove = remove_scene; }
		std::string get_remove_scene() const { return m_scene_to_remove; }

		void print_scene_name(bool all_scene = false);

		void create_scene(const std::string& scene_name, AG_ref<Scene>& scene) { m_scenes[scene_name] = scene; m_panel->set_scene(scene); }
		void create_new_scene(const std::string& path);
		void create_new_script(const std::string& path);
		void open_scene();
		void open_scene(std::string& path);
		void save_scene();
		void run_runtime();
		void save_scene_as_default();
		void save_all_scene();
		void try_exit();


		void load_texture(Entity entity);


		bool on_key_pressed(KeyPressedEvent& e);

		bool on_text_input(TextInputEvent& e);

		void handle_scene_deletion(const std::string& scene_name);
		void handle_scene_deletion();


		void render_export_panel();
		void open_export_panel();

	private:
		

		AG_ref<ViewController> m_view_controller;
		AG_ref<FrameBuffer> m_framebuffer;
		AG_ref<Scene> m_scene;
    AG_ref<ScenePanel> m_panel;
		ExportPanel m_export_panel;
		std::unordered_map<std::string, AG_ref<Scene>> m_scenes;


		vec2i m_viewport_size;
		vec2f m_current_mouse_pos;
		vec2i m_mouse_position;
		vec2f m_last_mouse_pos;
		bool m_viewport_hovered = false;
		vec2f m_image_pos;

		std::string m_scene_to_remove;

		bool m_entity_selected = false;
		std::array<bool, 3> axis_constraints = { false, false };
		bool m_minimized = false;

		static EditorLayer* s_instance;

	

	private:

		void render_dockspace();
		void render_viewport_window();
		void render_toolbar();
		void render_settings();
		void render_viewport_content();
		void handle_viewport_interaction();
		void render_scene_buttons();
		void render_add_scene_button();

		void render_settings_button();
		void render_transform_tools_group();
		void render_axis_constraints_group();
		void render_paint_settings_group();

		void render_paint_settings_button();
		float calculate_paint_group_width() const;


		void render_transform_button(const char* label, const char* shortcut,TransformSetting mode, bool disabled);
		void render_axis_button(const char* label, const char* shortcut,TransformAxis axis, bool disabled);

		void render_paint_button(const char* label, const char* shortcut,TileMap_Paint_Settings mode);

		float calculate_transform_group_width() const;
		float calculate_axis_group_width() const;
		const char* axis_to_string(TransformAxis axis) const;
		void set_transform_mode(TransformSetting mode);
		void set_axis_mode(TransformAxis axis);
		void set_paint_mode(TileMap_Paint_Settings mode);

		bool begin_dockspace_window();
		void end_dockspace_window();
		bool begin_viewport_window();
		void end_viewport_window();
		void set_active_scene(AG_ref<Scene> scene);
		void update_mouse_position();


		bool on_mouse_clicked(MouseButtonPressedEvent& e);
		bool on_entity_clicked();


		void editor_things();
		void draw_transform_settings(Entity e);

		void entity_selection();
	};

}
