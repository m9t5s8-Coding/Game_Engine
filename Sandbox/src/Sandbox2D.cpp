#include <Sandbox2D.hpp>

namespace ag
{
	Sandbox2D::Sandbox2D()
		: Layer("Sandbox2D")
	{

	}


	void Sandbox2D::on_attach()
	{
		vec2f view_size = Application::get().get_window().get_size();
		vec2f view_center = view_size / 2.0f;
		m_view_controller = ViewController::create(view_size, view_center);


	}

	void Sandbox2D::on_detach()
	{

	}

	void Sandbox2D::on_update(ag::TimeStamp ts)
	{
		RenderCommand::set_clear_color(ag::Color(42, 42, 42));
		RenderCommand::clear();

		m_view_controller->on_update(ts);

		Renderer2D::begin_scene(m_view_controller->get_view());
		Rectangle rect;
		rect.size = { 100, 100 };
		rect.fill_color = Color::Red;
		rect.border_thickness = 0.0f;
		rect.border_color = Color::Black;

		Transform transform;
		transform.position = { 10, 10 };

		Renderer2D::draw_rectangle(rect, transform);
		Renderer2D::end_scene();

	}
}
