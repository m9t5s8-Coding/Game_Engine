#include <Sandbox2D.hpp>

Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D")
{
}

void Sandbox2D::on_attach()
{
  ag::vec2u view_size(1280, 720);
  ag::vec2f view_center(1280 / 2, 720 / 2);
  m_view_controller.set_view(view_size, view_center);
}

void Sandbox2D::on_detach()
{
}

void Sandbox2D::on_update(ag::TimeStamp ts)
{
  m_view_controller.on_update(ts);

  ag::RenderCommand::set_clear_color(ag::Color::Red);
  ag::RenderCommand::clear();

  ag::Renderer2D::begin_scene(m_view_controller);

  ag::Rectangle rect;
  rect.size = ag::vec2u(50, 50);
  rect.scale = ag::vec2f(1, 1);

  int rows = 100;
  int cols = 100;
  float spacing = 0.0f;

  for (int y = 0; y < rows; y++)
  {
    for (int x = 0; x < cols; x++)
    {
      if ((x + y) % 2 == 0)
        rect.color = ag::Color::Black;
      else
        rect.color = ag::Color::White;
      rect.position = ag::vec2f(
          x * (rect.size.x + spacing) + rect.size.x / 2.0f,
          y * (rect.size.y + spacing) + rect.size.y / 2.0f);

      ag::Renderer2D::draw_rect(rect);
    }
  }

  ag::Renderer2D::end_scene();
}

void Sandbox2D::on_imgui_render()
{
}

void Sandbox2D::on_event(ag::Event &e)
{
  m_view_controller.on_event(e);
}
