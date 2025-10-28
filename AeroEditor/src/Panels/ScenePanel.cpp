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
			Entity entity{ entityID, m_scene.get() };
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
			((m_selected_entity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", tag.c_str());

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
				m_selected_entity = newEntity;
				m_show_create_panel = false;
			}
		}
		ImGui::End();
	}

	void ScenePanel::update_transform_settings()
	{
		if(m_selected_entity && m_selected_entity.has_component<Transform>())
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
				m_current_transform_setting = TransformSetting::None;
				m_current_transform_axix = TransformAxix::None;
			}
			return;
		}
		case ag::TransformSetting::Rotate:
		{
			rotate_transform_setting();
			if (Mouse::is_mouse_pressed(Button::ButtonLeft))
			{
				m_current_transform_setting = TransformSetting::None;
				m_current_transform_axix = TransformAxix::None;
			}
			return;
		}
		case ag::TransformSetting::Move:
		{
			move_transform_setting();
			if (Mouse::is_mouse_pressed(Button::ButtonLeft))
			{
				m_current_transform_setting = TransformSetting::None;
				m_current_transform_axix = TransformAxix::None;
			}
			return;
		}
		default: AERO_CORE_ERROR("No sucn Transform Setting!"); break;
		}
	}
	void ScenePanel::move_transform_setting()
	{
		auto& position = m_selected_entity.get_component<Transform>().position;
		switch (m_current_transform_axix)
		{
			case ag::TransformAxix::None:
			{
				position = m_current_mouse_position;
				break;
			}
			case ag::TransformAxix::X:
			{
				position.x = m_current_mouse_position.x;
				break;
			}
			case ag::TransformAxix::Y:
			{
				position.y = m_current_mouse_position.y;
				break;
			}
		}
	}
	void ScenePanel::rotate_transform_setting()
	{
		auto& rotation = m_selected_entity.get_component<Transform>().rotation;
		auto& position = m_selected_entity.get_component<Transform>().position;
		rotation = Math::angle_betn_points(position, m_current_mouse_position);
	}
	void ScenePanel::scale_transform_setting()
	{
		auto& scale = m_selected_entity.get_component<Transform>().scale;
	}

	bool ScenePanel::on_key_pressed(KeyPressedEvent& e)
	{
		if (!m_selected_entity)
			return false;

		if (e.get_key_code() == Key::G)
			m_current_transform_setting = TransformSetting::Move;

		if (e.get_key_code() == Key::S)
			m_current_transform_setting = TransformSetting::Scale;

		if (e.get_key_code() == Key::R)
			m_current_transform_setting = TransformSetting::Rotate;

		if (e.get_key_code() == Key::X)
			m_current_transform_axix = TransformAxix::X;

		if (e.get_key_code() == Key::Y)
			m_current_transform_axix = TransformAxix::Y;

		//TODO
		//if (e.get_key_code() == Key::Delete)
			//m_selected_entity.delete_entity();

		if (m_current_transform_setting != TransformSetting::None)
		{
			if (e.get_key_code() == Key::Escape)
			{
				m_current_transform_setting = TransformSetting::None;
				m_current_transform_axix = TransformAxix::None;
			}
		}

		return false;
	}
	
}
