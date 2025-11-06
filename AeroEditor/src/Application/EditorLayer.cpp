#include <Application/EditorLayer.hpp>
#include <algorithm>

namespace ag
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{

	}

	void EditorLayer::on_attach()
	{
		ag::vec2u window_size = Application::get().get_window().get_size();
		ag::vec2f view_center;
		m_view_controller = ag::AG_cref<ViewController>(window_size, view_center);

		FrameBufferSpecification spec;
		spec.size = window_size;


		m_framebuffer = FrameBuffer::create(spec);

		AERO_CORE_INFO("Scene name:{0}", Scene::get_active_scene()->get_name());
		m_scene = Scene::get_active_scene();

		if (!m_scene)
		{
			AERO_CORE_INFO("Invalid Scene");
		}
		m_panel = AG_cref<ScenePanel>(m_scene);
		// todo
		m_scenes[m_scene->get_name()] = m_scene;
	}

	void EditorLayer::on_detach()
	{
	}

	void EditorLayer::on_update(TimeStamp ts)
	{

		m_framebuffer->bind();
		RenderCommand::set_clear_color(ag::Color(42, 42, 42));
		RenderCommand::clear();

		m_view_controller->on_update(ts);
		m_panel->on_update();

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


		ImGuiWindowFlags viewport_flags = ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

		ImGui::Begin("ViewPort", nullptr, viewport_flags);
		{
			
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
				ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
				ImGui::BeginChild("Toolbar", ImVec2(0, 40), false, flags);
				{
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 5));

					ImVec4 text_color = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
					ImVec4 bg_color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
					ImVec4 bg_hovered = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);
					ImVec4 active_scene = ImVec4(0.2f, 0.1f, 0.2f, 1.0f);

					ImGui::PushStyleColor(ImGuiCol_Button, bg_color);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bg_hovered);
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, bg_hovered);
					ImGui::PushStyleColor(ImGuiCol_Text, text_color);

					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);

					bool is_first = true;
					std::string scene_to_remove;
					for (auto& [name, scene] : m_scenes)
					{
						if (!is_first)
							ImGui::SameLine(0, 10);

						is_first = false;
						ImGui::PushID(name.c_str());
						ImGui::BeginGroup();

						if(m_scene == scene)
							ImGui::PushStyleColor(ImGuiCol_Button, active_scene);
						else
							ImGui::PushStyleColor(ImGuiCol_Button, bg_color);

						if (ImGui::Button( name.c_str(), ImVec2(0, 30)))
						{
							m_scene = scene;
							m_scene->set_active_scene(scene);
							m_panel->set_scene(scene);
						}
						ImGui::PopStyleColor();
						ImGui::SameLine(0, 1);
						if (ImGui::Button("x", ImVec2(0, 30)))
						{
							// cross button
							scene_to_remove = name;
						}
						ImGui::EndGroup();
						ImGui::PopID();
					}

					if (!scene_to_remove.empty())
					{
						m_scenes.erase(scene_to_remove);
					}
					
					
					ImGui::SameLine(0, 10);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 0));
					ImGui::SetWindowFontScale(1.5f);
					if (ImGui::Button("+", ImVec2(0, 30)))
					{
						auto full_path = FileDialogs::save_file("AeroScene Files (*.aeroscene)\0*.aeroscene\0All Files (*.*)\0*.*\0");
						if (!full_path.empty())
						{
							auto project = Project::get_active_project();
							Helper::normalize_path(full_path);

							std::string project_dir = project->get_directory();
							std::string scene_dir = project->get_scene_directory();

							std::string base_path = project_dir + scene_dir + "/";

							std::string relative_path = full_path;
							if (relative_path.find(base_path) == 0)
								relative_path = relative_path.substr(base_path.size());

							Helper::normalize_path(relative_path);

							std::filesystem::path p(full_path);
							std::string scene_name = p.stem().string();
							std::string scene_path = "/" + relative_path;

							m_scene = Scene::create(scene_name, scene_path);
							SaveScene::save_scene(m_scene, full_path);
							Scene::set_active_scene(m_scene);

							m_scenes[scene_name] = m_scene;
							m_panel->set_scene(m_scene);
						}
					}
					ImGui::SetWindowFontScale(1.0f);
					ImGui::PopStyleVar();

					ImGui::PopStyleColor(4);
					ImGui::PopStyleVar(2);
				}
				ImGui::EndChild();
				ImGui::PopStyleVar();
			}
			
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
		ImGui::PopStyleVar(2);

		m_panel->on_imgui_render();

		ImGui::End();
	}

	void EditorLayer::on_event(ag::Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(AERO_BIND_EVENT_FN(EditorLayer::on_key_pressed));
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
		/*mouse_in_viewport.x = std::clamp(mouse_in_viewport.x, 0.0f, viewport_size.x);
		mouse_in_viewport.y = std::clamp(mouse_in_viewport.y, 0.0f, viewport_size.y);*/

		mouse_in_viewport.y -= 35;
		return vec2f(mouse_in_viewport.x, mouse_in_viewport.y);
	}

	bool EditorLayer::on_key_pressed(KeyPressedEvent& e)
	{
		bool control = Keyboard::is_key_pressed(Key::LeftControl) || Keyboard::is_key_pressed(Key::RightControl);
		bool shift = Keyboard::is_key_pressed(Key::LeftShift) || Keyboard::is_key_pressed(Key::RightShift);
		if (control)
		{
			if (e.get_key_code() == Key::S)
			{
				auto project = Project::get_active_project();
				std::string scene_path = project->get_directory()  + project->get_scene_directory()  + m_scene->get_directory();
				SaveScene::save_scene(m_scene, scene_path);
			}

			if (e.get_key_code() == Key::O)
			{
				auto full_path = FileDialogs::open_file("AeroScene Files (*.aeroscene)\0*.aeroscene\0All Files (*.*)\0*.*\0");
				Helper::normalize_path(full_path);
				m_scene = SaveScene::load_scene(full_path);
				m_scenes[m_scene->get_name()] = m_scene;
				m_panel->set_scene(m_scene);
			}

			return false;
		}
		return false;
	}
}
