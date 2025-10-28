#include <EditorLayer.hpp>

namespace ag
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{

	}

	void EditorLayer::on_attach()
	{
		ag::vec2u window_size = Application::get().get_window().get_size();
		ag::vec2f view_center(0, 0);
		m_view_controller = ag::AG_cref<ViewController>(window_size, view_center);

		m_texture = ag::Texture2D::create("assets/textures/tiles.png");
		Renderer2D::set_texture(m_texture);

		FrameBufferSpecification spec;
		spec.size = window_size;

		m_framebuffer = FrameBuffer::create(spec);
		m_scene = AG_cref<Scene>();
		m_panel = AG_cref<ScenePanel>(m_scene);
	}

	void EditorLayer::on_detach()
	{
	}

	void EditorLayer::on_update(TimeStamp ts)
	{
		m_view_controller->on_update(ts);
		m_panel->on_update();

		m_framebuffer->bind();
		RenderCommand::set_clear_color(ag::Color(42, 42, 42));
		RenderCommand::clear();

		Renderer2D::begin_scene(m_view_controller->get_view());
		m_scene->on_update(ts);
		Renderer2D::end_scene();
		m_framebuffer->unbind();
	}

	void EditorLayer::on_imgui_render()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


		}
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);


		ImGuiWindowFlags viewport_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("ViewPort", nullptr, viewport_flags);
		{																								 
			bool view_hovered = ImGui::IsWindowHovered();
			if (view_hovered)
				ImGui::SetWindowFocus();
			Application::get().get_imgui_layer()->block_events(!view_hovered);
			ImVec2 viewport_size = ImGui::GetContentRegionAvail();
			if (viewport_size.x > 0 && viewport_size.y > 0 &&
				(m_viewport_size.x != viewport_size.x || m_viewport_size.y != viewport_size.y))
			{
				m_viewport_size = viewport_size;
				m_framebuffer->resize(m_viewport_size);
				m_view_controller->on_resize(m_viewport_size);
				m_view_controller->set_viewport_size(m_viewport_size);
			}
			vec2f mouse_viewport = get_imgui_viewport_mouse_position();
			vec2f current_mouse_pos = Math::screen_to_world(mouse_viewport, m_view_controller->get_view().get_float_rect(), m_viewport_size);
			m_panel->set_current_mouse_position(current_mouse_pos);
			
			m_view_controller->set_viewport_mouse(mouse_viewport);

			uint32_t texture_ID = m_framebuffer->get_colorattachment_id();
			ImGui::Image((void*)(intptr_t)texture_ID, viewport_size, ImVec2(0, 1), ImVec2(1, 0));
		}
		ImGui::End();
		ImGui::PopStyleVar();

		m_panel->on_imgui_render();

		ImGui::Begin("Test2");

		ImGui::Text("Hello World");
		ImGui::End();
		ImGui::End();
	}

	void EditorLayer::on_event(ag::Event& e)
	{
		m_view_controller->on_event(e);
		m_panel->on_event(e);
	}

	vec2f EditorLayer::get_imgui_viewport_mouse_position()
	{
		ImVec2 window_pos = ImGui::GetWindowPos();
		ImVec2 content_min = ImGui::GetWindowContentRegionMin();
		ImVec2 content_pos = ImVec2(window_pos.x + content_min.x, window_pos.y + content_min.y);

		ImVec2 mouse_pos = ImGui::GetMousePos();
		ImVec2 mouse_in_viewport = ImVec2(mouse_pos.x - content_pos.x, mouse_pos.y - content_pos.y);

		ImVec2 viewport_size = ImGui::GetContentRegionAvail();
		mouse_in_viewport.x = std::clamp(mouse_in_viewport.x, 0.0f, viewport_size.x);
		mouse_in_viewport.y = std::clamp(mouse_in_viewport.y, 0.0f, viewport_size.y);

		return vec2f(mouse_in_viewport.x, mouse_in_viewport.y);
	}
}
