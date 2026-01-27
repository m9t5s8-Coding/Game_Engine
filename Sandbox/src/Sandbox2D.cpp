#include <Sandbox2D.hpp>

namespace ag
{
	Sandbox2D::Sandbox2D()
		: Layer("Sandbox2D")
	{

	}


	void Sandbox2D::on_attach()
	{
		auto size = Application::get().get_window().get_size();
		m_view_controller = ViewController::create(size, size / 2);
		ViewController::set_main_controller(m_view_controller);

		FrameBufferSpecification spec;
		spec.attachments = { FrameBuffer_Texture_Format::RGBA8,  FrameBuffer_Texture_Format::RED_INTEGER };

		spec.size = size;


		m_framebuffer = FrameBuffer::create(spec);


		load_project_data();
	}

	void Sandbox2D::on_detach()
	{

	}

	void Sandbox2D::on_update(ag::TimeStamp ts)
	{
		ViewController::set_mouse_position();
		m_view_controller->on_update(ts);

		m_framebuffer->bind();

		RenderCommand::set_clear_color(ag::Color(38, 45, 42));
		RenderCommand::clear();
		m_framebuffer->clear_attachment(1, -1);

		Renderer2D::begin_scene(m_view_controller->get_view(), Application::get().get_window().get_size());
		//m_scene->on_update(ts);

		Rectangle rect;
		rect.size = { 50, 50 };
		rect.fill_color = Color::Black;
		float spacing = 5.0f;
		rect.corner_radius = 20.0f;
		int shape_count = 40000;
		Transform_Component transform;
		int cols = 200;
		for (int i = 0; i < shape_count; i++)
		{
			int col = i % cols;
			int row = i / cols;

			transform.position.x = col * rect.size.x + spacing;
			transform.position.y = row * rect.size.y + spacing;

			Renderer2D::draw_rectangle(rect, transform);
		}


		Renderer2D::end_scene();

		entity_selection();

		m_framebuffer->unbind();


		Renderer2D::begin_scene(m_view_controller->get_view(), Application::get().get_window().get_size());
		Renderer2D::draw_fullscreen_quad(m_framebuffer->get_colorattachment_id());
		Renderer2D::end_scene();

	}

	void Sandbox2D::on_event(Event& event)
	{
		m_scene->on_event(event);
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseButtonPressedEvent>(AERO_BIND_EVENT_FN(Sandbox2D::on_mouse_pressed));
		dispatcher.Dispatch<WindowResizeEvent>(AERO_BIND_EVENT_FN(Sandbox2D::on_window_resize));
		dispatcher.Dispatch<WindowCloseEvent>(AERO_BIND_EVENT_FN(Sandbox2D::on_window_close));
	}

	std::string Sandbox2D::get_appdata_path()
	{
		const char* appdata = std::getenv("APPDATA");

		if (appdata)
			return std::string(appdata);
		else
			return ".";
	}


	void Sandbox2D::entity_selection()
	{
		vec2i mouse_position = Mouse::get_mouse_position();
		if (mouse_position.x >= 0 && mouse_position.y >= 0)
		{
			int pixel_data = m_framebuffer->read_pixel(1, mouse_position);
			if (pixel_data >= 0)
			{
				Entity e((entt::entity)(pixel_data));
				m_hover_entity = e;
				if (m_hover_entity.has_component<ButtonState_Component>())
				{
					auto& props = m_hover_entity.get_component<ButtonState_Component>();
					props.button_state |= Button_State::Hovered;
				}
			}
			else
			{
				m_hover_entity = Entity();
			}
		}
	}

	bool Sandbox2D::on_mouse_pressed(MouseButtonPressedEvent& e)
	{
		if (e.get_mouse_button() == Button::ButtonLeft)
		{
			if (!m_hover_entity)
				return false;

			if (m_hover_entity.has_component<ButtonState_Component>())
			{
				auto& props = m_hover_entity.get_component<ButtonState_Component>();
				props.button_state |= Button_State::Pressed;
			}
		}
		return false;
	}

	bool Sandbox2D::on_window_resize(WindowResizeEvent& e)
	{
		m_framebuffer->resize(e.get_size());
		return false;
	}

	bool Sandbox2D::on_window_close(WindowCloseEvent& e)
	{
		Application::get().m_running = false;
		return true;
	}

	void Sandbox2D::load_project_data()
	{
		std::string app_data_path = get_appdata_path();
		Helper::normalize_path(app_data_path);
		std::string app_folder = app_data_path + "/AEROEngine";
		std::string settings_path = app_folder + "/settings.json";

		json j;

		// Open Setting File For an AeroEngine
		{
			std::ifstream file(settings_path);
			if (!file.is_open())
			{
				AERO_CORE_INFO("Cannot Open File {0}", settings_path);
			}
			file >> j;
			file.close();
		}
		// Load the Project
		{
			std::string project_path;
			Helper::load_json(j["Project"], "File Path", project_path);
			{
				Project::load_project(project_path);
			}
		}
		
		std::string scene_path;
		{
			auto project = Project::get_active_project();
			auto& project_file = project->get_project_file_directory();
			std::ifstream proj_file(project_file);
			if (!proj_file.is_open())
			{
				AERO_CORE_INFO("Cannot Open File :{0}", project_file);
			}
			proj_file >> j;
			proj_file.close();
			Helper::load_json(j["Scene"], "Default Path", scene_path);
			auto& project_directory = project->get_directory();
			auto& scene_directory = project->get_scene_directory();
			scene_path = project_directory + scene_directory + scene_path;
		}


		m_scene = SaveScene::load_scene(scene_path);
		Scene::set_active_scene(m_scene);

		auto entities = m_scene->get_view<Camera_Component>();
		for (auto entityID : entities)
		{
			Entity entity(entityID);
			auto& props = entity.get_component<Camera_Component>();

			auto& view = m_view_controller->get_view();
			vec2f view_size = props.size;

			m_view_controller = ViewController::create(view_size, props.center);
			ViewController::set_main_controller(m_view_controller);
			if(entity.has_component<Window_Component>())
			{
				auto& props = entity.get_component<Window_Component>();
				Application::get().get_window().set_size(props.size);
				Application::get().get_window().center_window();
			}
		}
		
	}
}
