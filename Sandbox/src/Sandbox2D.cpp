#include <Sandbox2D.hpp>

namespace ag
{
	Sandbox2D::Sandbox2D()
		: Layer("Sandbox2D")
	{

	}


	void Sandbox2D::on_attach()
	{
		load_project_data();

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

		Renderer2D::begin_scene(m_view_controller->get_view(), Application::get().get_window().get_size());

		m_scene->on_update(ts);

		Renderer2D::end_scene();
	}

	void Sandbox2D::on_event(Event& event)
	{
		m_scene->on_event(event);
	}

	void Sandbox2D::load_project_data()
	{
		std::string project_path = "D:/Aero/Test/Test.aeroproj";
		{
			Project::load_project(project_path);
		}

		std::string scene_path = "D:/Aero/Test/Scenes/player.aeroscene";

		m_scene = SaveScene::load_scene(scene_path);
		Scene::set_active_scene(m_scene);
	}
}
