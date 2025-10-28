#include <Sandbox2D.hpp>

Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D")
{
}

void Sandbox2D::on_attach()
{
  ag::vec2u window_size = ag::Application::get().get_window().get_size();
  ag::vec2f view_center(0, 0);
  m_view_controller = ag::AG_cref<ag::ViewController>(window_size, view_center);

  m_texture = ag::Texture2D::create("assets/textures/tiles.png");
  ag::Renderer2D::set_texture(m_texture);

}

void Sandbox2D::on_detach()
{
}

void Sandbox2D::on_update(ag::TimeStamp ts)
{
  m_view_controller->on_update(ts);

  ag::RenderCommand::set_clear_color(ag::Color(42, 42, 42));
  ag::RenderCommand::clear();

  ag::Renderer2D::begin_scene(m_view_controller->get_view());
  ag::Renderer2D::end_scene();
}

void Sandbox2D::on_imgui_render()
{

}

void Sandbox2D::on_event(ag::Event &e)
{
  m_view_controller->on_event(e);
}
