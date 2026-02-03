#pragma once

#include <Core/Core.hpp>
#include <Core/TimeStamp.hpp>
#include <Events/Event.hpp>
#include <Math/Math.hpp>
#include <Renderer/View.hpp>

namespace ag
{
	class ViewController
	{
	public:
		ViewController();
		ViewController(const vec2f& view_size, const vec2f& view_center);
		~ViewController();

		static AG_ref<ViewController> create(const vec2f& view_size, const vec2f& view_center);

		void set_view(const vec2u& view_size, const vec2f& view_center);

		void on_update(TimeStamp ts);
		void on_event(Event& e);

		View& get_view() { return m_view; };
		const View& get_view() const { return m_view; };

		void set_size(const vec2f& size) { m_view.set_size(size); }
		void set_center(const vec2f& center) { m_view.set_center(center); }

		void set_viewport_mouse(const vec2f& mouse_pos) { m_mouse_in_viewport = mouse_pos; }
		void set_viewport_size(const vec2f& viewport_size) { m_viewport_size = viewport_size; }

		void on_resize(const vec2f& size);

		static void set_main_controller(const AG_ref<ViewController>& controller) { s_main_controller = controller; }
		static AG_ref<ViewController> get_main_controller() { return s_main_controller; }

		static float_rect get_view_floatrect() { return s_main_controller->get_view().get_float_rect(); }

		static vec2f get_mouse_position() { return s_mouse_position; }
		static void set_mouse_position();

	private:
		bool on_mouse_scroll(MouseScrolledEvent& e);
		bool on_window_resize(WindowResizeEvent& e);
		bool on_mouse_clicked(MouseButtonPressedEvent& e);
		bool on_mouse_released(MouseButtonReleasedEvent& e);
		bool on_mouse_moved(MouseMovedEvent& e);


	private:
		View m_view;

		inline static AG_ref<ViewController> s_main_controller;
		inline static vec2f s_mouse_position;

		bool m_middle_pressed = false;
		bool m_view_ismax = false;
		bool m_view_ismin = false;

		vec2f m_last_mouse_pos;
		vec2f m_last_size;

		vec2f m_mouse_in_viewport;
		vec2f m_viewport_size;
	};
}
