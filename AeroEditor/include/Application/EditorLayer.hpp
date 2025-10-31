#pragma once

#include <Aero.hpp>
#include <iostream>

#include <imgui.h>
#include <Panels/ScenePanel.hpp>

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

		ag::vec2f get_imgui_viewport_mouse_position();



		bool on_key_pressed(KeyPressedEvent& e);
	private:
		AG_ref<ViewController> m_view_controller;
		AG_ref<Texture> m_texture;
		AG_ref<FrameBuffer> m_framebuffer;
		AG_ref<Scene> m_scene;
    AG_ref<ScenePanel> m_panel;


		vec2i m_viewport_size;
	};

}
