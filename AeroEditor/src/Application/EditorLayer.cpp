#include <Application/EditorLayer.hpp>
#include <algorithm>

namespace ag
{
	EditorLayer *EditorLayer::s_instance = nullptr;

	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
		s_instance = this;
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
		{
			Application::set_mouse_position(m_viewport_mouse_pos);
		}
		
		m_framebuffer->bind();
		RenderCommand::set_clear_color(ag::Color(42, 42, 42));
		RenderCommand::clear();

		m_view_controller->on_update(ts);
		m_panel->on_update();

		
		Renderer2D::begin_scene(m_view_controller->get_view(), m_viewport_size);
		editor_things();

		m_scene->on_update(ts);

		Text text;
		Transform trans;
		text.text = m_text_buffer;
		text.text_color = Color::White;
		text.mode = RenderMode::World;
		trans.position = { 0, 0 };
		Renderer2D::draw_text(text, trans);
	
		
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
			

			uint32_t texture_ID = m_framebuffer->get_colorattachment_id();
			ImGui::Image((void*)(intptr_t)texture_ID, viewport_size, ImVec2(0, 1), ImVec2(1, 0));
			{
				ImVec2 img_pos = ImGui::GetItemRectMin();
				ImVec2 mouse = ImGui::GetMousePos();
				m_viewport_hovered = ImGui::IsItemHovered();
				ImGuiIO& io = ImGui::GetIO();

				m_viewport_mouse_pos = { mouse.x - img_pos.x, mouse.y - img_pos.y };

				//if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
				//{
				//	if (m_viewport_mouse_pos.x < 0)
				//		m_viewport_mouse_pos.x = m_viewport_size.x - 1;
				//	else if (m_viewport_mouse_pos.x > m_viewport_size.x)
				//		m_viewport_mouse_pos.x = 1;

				//	// vertical wrap
				//	if (m_viewport_mouse_pos.y < 0)
				//		m_viewport_mouse_pos.y = m_viewport_size.y - 1;
				//	else if (m_viewport_mouse_pos.y > m_viewport_size.y)
				//		m_viewport_mouse_pos.y = 1;

				//	// Update actual ImGui mouse
				//	io.MousePos.x = img_pos.x + m_viewport_mouse_pos.x;
				//	io.MousePos.y = img_pos.y + m_viewport_mouse_pos.y;

				//}


				m_view_controller->set_viewport_mouse(m_viewport_mouse_pos);
				m_viewport_mouse_pos = Math::screen_to_world(m_viewport_mouse_pos, m_view_controller->get_view().get_float_rect(), m_viewport_size);
				
				m_panel->set_current_mouse_position(m_viewport_mouse_pos);

				
			}
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
		dispatcher.Dispatch<TextInputEvent>(AERO_BIND_EVENT_FN(EditorLayer::on_text_input));
		m_view_controller->on_event(e);
		m_panel->on_event(e);
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

			if (e.get_key_code() == Key::V)
			{
				auto text = Application::get().get_window().get_clipboard_string();
				m_text_buffer += text;
			}
			
			

			return false;
		}

		if (e.get_key_code() == Key::Backspace && !m_text_buffer.empty())
		{
			m_text_buffer.pop_back();
		}

		if (e.get_key_code() == Key::Enter)
		{
			m_text_buffer += '\n';
		}

		if (e.get_key_code() == Key::Tab)
		{
			m_text_buffer += "    ";
		}
		return false;
	}

	bool EditorLayer::on_text_input(TextInputEvent& e)
	{
		m_text_buffer += e.get_character();
		return true;
	}

	void EditorLayer::editor_things()
	{
		Rectangle x_axis, y_axis;
		Transform x_axis_transform, y_axis_transform;
		x_axis.fill_color = Color(255, 107, 107, 200);
		y_axis.fill_color = Color(78, 205, 196, 200);

		x_axis.size =  vec2f(m_viewport_size.x, 1);
		y_axis.size =  vec2f(1, m_viewport_size.y);

		x_axis.mode = RenderMode::Screen;
		y_axis.mode = RenderMode::Screen;

		const auto& view = m_view_controller->get_view();
		// x axis and y axis
		{
			x_axis_transform.position = vec2i( view.get_center().x, 0 );
			y_axis_transform.position = vec2i( 0, view.get_center().y );
			x_axis_transform.position = Math::world_to_screen(x_axis_transform.position, view.get_float_rect(), m_viewport_size);
			y_axis_transform.position = Math::world_to_screen(y_axis_transform.position, view.get_float_rect(), m_viewport_size);
			Renderer2D::draw_rectangle(y_axis, y_axis_transform);
			Renderer2D::draw_rectangle(x_axis, x_axis_transform);
		}
		
		//Transformation axix
		{
			Entity e = m_panel->get_selected_entity();
			if (e)
			{
				TransformSetting t_setting = m_panel->get_transform_setting();
				
				switch (t_setting)
				{
				case ag::TransformSetting::None:
					break;
				case ag::TransformSetting::Scale:
				{
					draw_transform_settings(e);
					break;
				}
				case ag::TransformSetting::Rotate:
				{

					break;
				}
				case ag::TransformSetting::Move:
				{
					draw_transform_settings(e);
					break;
				}
				default:
					break;
				}
				
			}
		}

		// Camera
		{
			auto scene = Scene::get_active_scene();
			const auto& entities = scene->m_registry.view<Tag>();
			for (auto& entityID : entities)
			{
				Entity entity(entityID);
				auto& type = entity.get_component<Tag>().node_type;

				if (type != NodeType::Camera)
					continue;

				if (entity.has_component<CameraComponent::CameraProps>())
				{
					auto& props = entity.get_component<CameraComponent::CameraProps>();

					vec2f top_left = props.view_center - (props.view_size * props.zoom) / 2;
					

					vec2f bottom_right = props.view_center + (props.view_size * props.zoom) / 2;
					

					vec2f top_right = { bottom_right.x, top_left.y };
					

					vec2f bottom_left = { top_left.x, bottom_right.y };
					

					top_left = Math::world_to_screen(top_left, view.get_float_rect(), m_viewport_size);
					top_right = Math::world_to_screen(top_right, view.get_float_rect(), m_viewport_size);
					bottom_left = Math::world_to_screen(bottom_left, view.get_float_rect(), m_viewport_size);
					bottom_right = Math::world_to_screen(bottom_right, view.get_float_rect(), m_viewport_size);

					vec2f size;
					size.x = top_right.x - top_left.x;
					size.y = bottom_right.y - top_right.y;

					Rectangle width, height;

					width.size = { size.x, 1.0f };
					height.size = { 1.0f, size.y };

					width.fill_color = Color(120, 120, 120, 255);
					height.fill_color = Color(120, 120, 120, 255);

					width.mode = RenderMode::Screen;
					height.mode = RenderMode::Screen;

					Transform transform;
					

					// Top
					transform.position = Math::mid_point(top_left , top_right);
					Renderer2D::draw_rectangle(width, transform);

					// Left
					transform.position = Math::mid_point(top_left , bottom_left);
					Renderer2D::draw_rectangle(height, transform);

					// Bottom
					transform.position = Math::mid_point(bottom_left, bottom_right);
					Renderer2D::draw_rectangle(width, transform);

					// Right
					transform.position = Math::mid_point(bottom_right, top_right);
					Renderer2D::draw_rectangle(height, transform);

				}
			}
		}

	}

	void EditorLayer::draw_transform_settings(Entity e)
	{
		const auto& view = m_view_controller->get_view();
		TransformAxis t_axis = m_panel->get_transform_axis();

		Rectangle x_axis, y_axis;
		Transform x_axis_transform, y_axis_transform;

		// Axis visual properties
		x_axis.fill_color = Color(255, 107, 107, 180);
		y_axis.fill_color = Color(78, 205, 196, 180);

		x_axis.size = vec2f(m_viewport_size.x, 1);
		y_axis.size = vec2f(1, m_viewport_size.y);

		x_axis.mode = RenderMode::Screen;
		y_axis.mode = RenderMode::Screen;

		if (!e.has_component<Transform>())
			return;

		const auto& transform = e.get_component<Transform>();

		vec2i x_pos = vec2i( view.get_center().x, transform.position.y );
		vec2i y_pos = vec2i( transform.position.x, view.get_center().y );
		 
		x_axis_transform.position = Math::world_to_screen(x_pos, view.get_float_rect(), m_viewport_size);
		y_axis_transform.position = Math::world_to_screen(y_pos, view.get_float_rect(), m_viewport_size);

		if (t_axis == TransformAxis::None || t_axis == TransformAxis::X)
			Renderer2D::draw_rectangle(x_axis, x_axis_transform);

		if (t_axis == TransformAxis::None || t_axis == TransformAxis::Y)
			Renderer2D::draw_rectangle(y_axis, y_axis_transform);
	}
}
