#pragma once

#include <Aero.hpp>
#include <iostream>

#include <imgui.h>
#include <Panels/ScenePanel.hpp>
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

		ag::vec2f get_viewport_mouse_position() { return m_viewport_mouse_pos; }

		bool is_viewport_hovered() const { return m_viewport_hovered; }

		static EditorLayer& get() { return *s_instance; }

		bool on_key_pressed(KeyPressedEvent& e);
	private:
		AG_ref<ViewController> m_view_controller;
		AG_ref<FrameBuffer> m_framebuffer;
		AG_ref<Scene> m_scene;
    AG_ref<ScenePanel> m_panel;
		std::unordered_map<std::string, AG_ref<Scene>> m_scenes;

		vec2i m_viewport_size;
		vec2f m_viewport_mouse_pos;
		bool m_viewport_hovered = false;

		static EditorLayer *s_instance;

	private:
		void editor_things();
		void draw_transform_settings(Entity e);
	};

}
