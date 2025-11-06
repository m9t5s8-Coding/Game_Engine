#include <Panels/ScenePanel.hpp>

namespace ag
{
	ScenePanel::ScenePanel(const AG_ref<Scene>& scene)
	{
		set_scene(scene);
	}

	void ScenePanel::set_scene(const AG_ref<Scene>& scene)
	{
		m_scene = scene;
		m_selected_entity = {};
	}

	void ScenePanel::on_update()
	{
		update_transform_settings();

		m_last_mouse_position = m_current_mouse_position;
	}

	void ScenePanel::on_event(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(AERO_BIND_EVENT_FN(ScenePanel::on_key_pressed));
	}

	void ScenePanel::set_selected_entity(Entity entity)
	{
		m_selected_entity = entity;
	}

	void ScenePanel::on_imgui_render()
	{
		ImGui::Begin("Scene");
		if (ImGui::Button("Add Objects"))
		{
			m_show_create_panel = true;
		}
		ImGui::Spacing();

		auto view = m_scene->m_registry.view<Tag>();
		for (auto entityID : view)
		{
			Entity entity(entityID);
			draw_node_hierarchy(entity);
			ImGui::Spacing();
		}
		ImGui::End();


		ImGui::Begin("Properties");
		if (m_selected_entity)
			draw_properties_panel();
		ImGui::End();

		if (m_show_create_panel)
			draw_create_object();
	}

	void ScenePanel::draw_node_hierarchy(Entity entity)
	{
		auto& tag = entity.get_component<Tag>().tag;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));

		ImGuiTreeNodeFlags flags =
			((m_selected_entity == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_Leaf;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entity.get_id(), flags, "%s", tag.c_str());

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			m_selected_entity = entity;

		if (opened)
			ImGui::TreePop();

		ImGui::PopStyleVar();
	}

	void ScenePanel::draw_properties_panel()
	{
		auto type = m_selected_entity.get_component<Tag>().node_type;
		auto it = NodeFactory::properties_map.find(type);
		if (it != NodeFactory::properties_map.end())
			it->second(m_selected_entity);
	}

	void ScenePanel::draw_create_object()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		ImGui::Begin("Create Objects", &m_show_create_panel, window_flags);
		static NodeType selectedPrefab = NodeType::Rectangle;
		NodeType newSelected = selectedPrefab;

		for (auto& [type, name] : NodeFactory::nodes)
		{
			if (ImGui::Selectable(name.c_str(), selectedPrefab == type))
				newSelected = type;
			ImGui::Spacing();
			ImGui::Spacing();
		}
		selectedPrefab = newSelected;
		auto it = NodeFactory::nodes.find(selectedPrefab);
		if (it != NodeFactory::nodes.end() && NodeFactory::create_map.find(selectedPrefab) != NodeFactory::create_map.end())
		{
			if (ImGui::Button("Create Entity"))
			{
				Entity newEntity = m_scene->create_entity(it->second, selectedPrefab);
				/*auto& tag = newEntity.get_component<Tag>();
				newEntity.add_component<SortKey>(tag.index);*/

				m_selected_entity = newEntity;
				m_show_create_panel = false;
			}
		}
		ImGui::End();
	}

	void ScenePanel::update_transform_settings()
	{
		if (!m_selected_entity || !m_selected_entity.has_component<Transform>())
			return;

		switch (m_current_transform_setting)
		{
		case ag::TransformSetting::None:
		{
			return;
		}
		case ag::TransformSetting::Scale:
		{
			scale_transform_setting();
			if (Mouse::is_mouse_pressed(Button::ButtonLeft))
			{
				m_initial_transform.scale = m_selected_entity.get_component<Transform>().scale;
				reset_transform_setting();
			}
			else if (Mouse::is_mouse_pressed(Button::ButtonRight))
			{
				auto& scale = m_selected_entity.get_component<Transform>().scale;
				scale = m_initial_transform.scale;
				reset_transform_setting();
			}
			return;
		}
		case ag::TransformSetting::Rotate:
		{
			rotate_transform_setting();
			if (Mouse::is_mouse_pressed(Button::ButtonLeft))
			{
				m_initial_transform.rotation = m_selected_entity.get_component<Transform>().rotation;
				reset_transform_setting();
			}
			else if (Mouse::is_mouse_pressed(Button::ButtonRight))
			{
				auto& rotation = m_selected_entity.get_component<Transform>().rotation;
				rotation = m_initial_transform.rotation;
				reset_transform_setting();
			}
			return;
		}
		case ag::TransformSetting::Move:
		{
			move_transform_setting();
			if (Mouse::is_mouse_pressed(Button::ButtonLeft))
			{
				m_initial_transform.position = m_selected_entity.get_component<Transform>().position;
				reset_transform_setting();
			}
			else if (Mouse::is_mouse_pressed(Button::ButtonRight))
			{
				auto& position = m_selected_entity.get_component<Transform>().position;
				position = m_initial_transform.position;
				reset_transform_setting();
			}
			return;
		}
		default: AERO_CORE_ERROR("No sucn Transform Setting!"); break;
		}
	}
	void ScenePanel::move_transform_setting()
	{
		auto& position = m_selected_entity.get_component<Transform>().position;


		if (!m_move_flag)
		{
			m_initial_transform.position = position;
			m_last_mouse_position = m_current_mouse_position;
			m_move_flag = true;
		}

		vec2f delta = m_current_mouse_position - m_last_mouse_position;

		switch (m_current_transform_axix)
		{
		case ag::TransformAxix::None:
		{
			position += delta;
			break;
		}
		case ag::TransformAxix::X:
		{
			position.x += delta.x;
			break;
		}
		case ag::TransformAxix::Y:
		{
			position.y += delta.y;
			break;
		}
		}
	}
	void ScenePanel::rotate_transform_setting()
	{
		auto& rotation = m_selected_entity.get_component<Transform>().rotation;
		auto& position = m_selected_entity.get_component<Transform>().position;

		if (!m_rotate_flag)
		{
			m_initial_transform.rotation = rotation;
			m_last_mouse_position = m_current_mouse_position;
			m_rotate_flag = true;
		}

		vec2f delta = m_current_mouse_position - m_last_mouse_position;

		rotation += Math::angle_betn_3points(m_last_mouse_position,position, m_current_mouse_position);

		if (rotation > 360) rotation -= 360;
		if (rotation < -360) rotation += 360;
	}
	void ScenePanel::scale_transform_setting()
	{
		auto& transform = m_selected_entity.get_component<Transform>();

		if (!m_scale_flag)
		{
			m_initial_transform.scale = transform.scale;
			m_last_mouse_position = m_current_mouse_position;
			m_scale_flag = true;
		}

		float initial_distance = (m_last_mouse_position - transform.position).length();
		float current_distance = (m_current_mouse_position - transform.position).length();

		float scale_ratio = current_distance / std::max(initial_distance, 0.001f);


		switch (m_current_transform_axix)
		{
			case TransformAxix::None:
			{
				transform.scale *= scale_ratio;
				break;
			}
			case TransformAxix::X:
			{
				transform.scale.x *= scale_ratio;
				break;
			}
			case TransformAxix::Y:
			{
				transform.scale.y *= scale_ratio;
				break;
			}
		}

		transform.scale.x = std::max(0.01f, transform.scale.x);
		transform.scale.y = std::max(0.01f, transform.scale.y);
	}

	void ScenePanel::reset_transform_setting()
	{
		m_current_transform_setting = TransformSetting::None;
		m_current_transform_axix = TransformAxix::None;
		m_move_flag = true;
		m_scale_flag = true;
		m_rotate_flag = true;
	}

	bool ScenePanel::on_key_pressed(KeyPressedEvent& e)
	{
		bool control = Keyboard::is_key_pressed(Key::LeftControl) || Keyboard::is_key_pressed(Key::RightControl);
		bool shift = Keyboard::is_key_pressed(Key::LeftShift) || Keyboard::is_key_pressed(Key::RightShift);
		if (control)
		{
			if (e.get_key_code() == Key::D && m_selected_entity)
				m_selected_entity = m_scene->duplicate_entity(m_selected_entity);

			return false;
		}

		//Transformation Setting
		if (!m_selected_entity)
			return false;
		switch (e.get_key_code())
		{
		case Key::G: m_current_transform_setting = TransformSetting::Move; break;
		case Key::S: m_current_transform_setting = TransformSetting::Scale; break;
		case Key::R: m_current_transform_setting = TransformSetting::Rotate; break;
		case Key::X: m_current_transform_axix = TransformAxix::X; break;
		case Key::Y: m_current_transform_axix = TransformAxix::Y; break;
		case Key::Escape:
			if (m_current_transform_setting != TransformSetting::None)
				reset_transform_setting();
			break;
		}
		return false;
	}
}
