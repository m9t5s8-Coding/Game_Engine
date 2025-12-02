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

		std::string scene_path = "D:/Aero/Test/Scenes/tik.aeroscene";

		m_scene = SaveScene::load_scene(scene_path);
		Scene::set_active_scene(m_scene);

		auto entities = m_scene->get_view<CameraComponent::CameraProps>();
		for (auto entityID : entities)
		{
			Entity entity(entityID);
			auto& props = entity.get_component< CameraComponent::CameraProps>();
			
			auto& view = m_view_controller->get_view();
			vec2f view_size = props.view_size * props.zoom;

			m_view_controller = ViewController::create(view_size, props.view_center);
		}
	}
}
