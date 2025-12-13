#include <Panels/ScenePanel.hpp>
#include <Application/EditorLayer.hpp>
#include <Node/TileMapNodeFeatures.hpp>

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
		update_tilemap();


		m_last_mouse_position = m_current_mouse_position;


	}

	void ScenePanel::on_event(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(AERO_BIND_EVENT_FN(ScenePanel::on_key_pressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(AERO_BIND_EVENT_FN(ScenePanel::on_mouse_pressed));
	}

	void ScenePanel::set_selected_entity(Entity entity)
	{
		m_selected_entity = entity;
	}

	void ScenePanel::on_imgui_render()
	{
		ImGui::Begin("Scene");
		draw_scene_top_panel();
		ImGui::Spacing();

		auto view = m_scene->m_registry.view<Tag>();
		for (auto entityID : view)
		{
			Entity entity(entityID);
			auto& tag = entity.get_component<Tag>();
			if (tag.parent.get_id() == INVALID_ENTITY)
			{
				draw_node_hierarchy(entity, 0);
				ImGui::Spacing();

			}
		}
		ImGui::End();


		ImGui::Begin("Properties");
		if (m_selected_entity)
			draw_properties_panel();
		ImGui::End();

		if (m_show_create_panel)
			draw_create_object();

		if (m_selected_entity)
		{
			auto& tag = m_selected_entity.get_component<Tag>();
			if (tag.node_type == NodeType::TileMap)
			{
				if (m_selected_entity.has_component<TileMapNode::TileMapProp>())
				{
					auto& props = m_selected_entity.get_component<TileMapNode::TileMapProp>();
					//TileMapNodeFeatures::texture_selector_gui(props.texture, m_texture_rect);
					TileMapNodeFeatures::register_tile(m_selected_entity);

					/*m_is_texture_selected = (texture_selector(props.texture, props.size, m_texture_rect));
					if (m_is_texture_selected)
					{
						auto& props = m_selected_entity.get_component<TileMapNode::TileMapProp>();
						props.ghost_sprite.texture_rect = m_texture_rect;
						props.display_ghost = true;
					}*/
				}
			}
		}
	}

	void ScenePanel::draw_node_hierarchy(Entity entity, int level)
	{
		auto& tag = entity.get_component<Tag>();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		if (m_selected_entity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (tag.children.empty())
			flags |= ImGuiTreeNodeFlags_Leaf;

		ImGui::Indent(level * 10.0f);

		// Draw the tree node
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entity.get_id(), flags, "%s", tag.tag.c_str());

		// Selection
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			if (m_selected_entity != entity)
			{
				m_move_flag = false;
				m_scale_flag = false;
				m_rotate_flag = false;

				if (m_selected_entity)
				{
					if (m_selected_entity.has_component<Tag>())
					{
						auto& tag = m_selected_entity.get_component<Tag>();
						if (tag.node_type == NodeType::TileMap)
						{
							auto& props = m_selected_entity.get_component<TileMapNode::TileMapProp>();
							props.display_ghost = false;
						}
					}
				}

			}
			m_selected_entity = entity;
		}
		// Draw children recursively
		if (opened)
		{
			for (auto& child : tag.children)
			{
				draw_node_hierarchy(child, level + 1);
			}


			ImGui::TreePop();
		}

		ImGui::Unindent(level * 10.0f);
	}

	void ScenePanel::draw_scene_top_panel()
	{
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
			ImGui::BeginChild("Toolbar", ImVec2(0, 40), false, flags);
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 5));

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



				ImGui::SameLine(0, 10);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 0));
				ImGui::SetWindowFontScale(1.5f);
				if (ImGui::Button("+", ImVec2(30, 30)))
				{
					m_show_create_panel = true;
				}
				ImGui::SameLine(0, 5.0f);
				if (ImGui::Button("s", ImVec2(30, 30)))
				{
					add_scripts();
				}
				ImGui::SetWindowFontScale(1.0f);
				ImGui::PopStyleVar();

				ImGui::PopStyleColor(4);
				ImGui::PopStyleVar(2);
			}
			ImGui::EndChild();
			ImGui::PopStyleVar();
		}
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

				if (m_selected_entity)
				{
					auto& tag = m_selected_entity.get_component<Tag>();
					auto& new_e_tag = newEntity.get_component<Tag>();

					new_e_tag.parent = m_selected_entity;
					tag.children.push_back(newEntity);
				}

				m_selected_entity = newEntity;
				m_show_create_panel = false;
			}
		}
		ImGui::End();
	}

	void ScenePanel::draw_selected_text()
	{
		if (m_selected_entity.get_id() == INVALID_ENTITY)
			return;

		if (m_selected_entity.has_component<Transform>())
		{
			auto& name = m_selected_entity.get_component<Tag>().tag;
			auto trans = Transform::get_world_transform(m_selected_entity);
			Transform transform;
			transform.position = Math::world_to_screen(trans.position, EditorLayer::get().get_float_rect(), EditorLayer::get().get_viewport_size());
			Text text;
			text.text = name;
			text.text_color = Color(220, 220, 220);
			text.font_size = 18;
			text.mode = RenderMode::Screen;
			Renderer2D::draw_text(text, transform);
		}
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
			m_delta = { 0, 0 };
		}

		vec2f delta = m_current_mouse_position - m_last_mouse_position;
		m_delta += delta;

		switch (m_current_transform_axis)
		{
		case ag::TransformAxis::None:
		{
			vec2f int_part = { std::floor(m_delta.x), std::floor(m_delta.y) };
			position += int_part;

			m_delta.x -= int_part.x;
			m_delta.y -= int_part.y;
			break;
		}
		case ag::TransformAxis::X:
		{
			float int_x = std::floor(m_delta.x);
			position.x += int_x;

			m_delta.x -= int_x;
			break;
		}
		case ag::TransformAxis::Y:
		{
			float int_y = std::floor(m_delta.y);
			position.y += int_y;

			m_delta.y -= int_y;
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

		rotation += Math::angle_betn_3points(m_last_mouse_position, position, m_current_mouse_position);

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


		switch (m_current_transform_axis)
		{
		case TransformAxis::None:
		{
			transform.scale *= scale_ratio;
			break;
		}
		case TransformAxis::X:
		{
			transform.scale.x *= scale_ratio;
			break;
		}
		case TransformAxis::Y:
		{
			transform.scale.y *= scale_ratio;
			break;
		}
		}

		transform.scale.x = std::max(0.01f, transform.scale.x);
		transform.scale.y = std::max(0.01f, transform.scale.y);
	}

	void ScenePanel::add_scripts()
	{
		if (!m_selected_entity)
			return;

		auto full_path = FileDialogs::open_file("Lua Scripts(*.lua)\0 * .lua\0All Files(*.*)\0 * .*\0");
		if (!full_path.empty())
		{
			auto project = Project::get_active_project();
			Helper::normalize_path(full_path);

			std::string project_dir = project->get_directory();
			std::string script_dir = project->get_scripts_directory();

			std::string base_path = project_dir + script_dir + "/";

			std::string relative_path = full_path;
			if (relative_path.find(base_path) == 0)
				relative_path = relative_path.substr(base_path.size());

			Helper::normalize_path(relative_path);

			std::filesystem::path p(full_path);
			std::string script_path = "/" + relative_path;

			if (!m_selected_entity.has_component<ScriptComponent>())
			{
				ScriptComponent comp;
				comp.script_path = script_path;
				m_selected_entity.add_component<ScriptComponent>(comp);
			}
			else
			{
				auto& comp = m_selected_entity.get_component<ScriptComponent>();
				comp.script_path = script_path;
			}
		}
	}

	void ScenePanel::reset_transform_setting()
	{
		m_current_transform_setting = TransformSetting::None;
		m_current_transform_axis = TransformAxis::None;
		m_move_flag = false;
		m_scale_flag = false;
		m_rotate_flag = false;
		m_delta = { 0, 0 };
	}

	void ScenePanel::update_tilemap()
	{
		if (!m_selected_entity)
			return;

		if (m_selected_entity.has_component<Tag>())
		{
			const auto& tag = m_selected_entity.get_component<Tag>();
			if (tag.node_type == NodeType::TileMap)
			{
				auto& props = m_selected_entity.get_component<TileMapNode::TileMapProp>();
				if (EditorLayer::get().is_viewport_hovered())
				{
					if (props.display_ghost)
					{
						vec2f mouse_position = EditorLayer::get().get_viewport_mouse_position();
						vec2i tile_pos = {
							(int)std::floor((mouse_position.x - props.offset.x) / props.size.x),
							(int)std::floor((mouse_position.y - props.offset.y) / props.size.y)
						};

						props.ghost_sprite.texture_rect = m_texture_rect;
						props.ghost_sprite_position = tile_pos;
						props.ghost_sprite.size = props.size;
					}
				}
				else
				{
					props.ghost_sprite.size = { 0, 0 };
				}
			}
		}
	}

	bool ScenePanel::texture_selector(const AG_ref<Texture2D>& texture, const vec2u& tile_size, uint_rect& texture_rect)
	{
		bool is_selected = false;
		ImGui::Begin("Texture Selector");
		{
			ImVec2 image_size(texture->get_size().x, texture->get_size().y);
			ImGui::Image((void*)texture->get_texture_id(), image_size);

			ImVec2 image_pos = ImGui::GetItemRectMin();
			ImVec2 mouse_pos = ImGui::GetMousePos();
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				int relX = (int)(mouse_pos.x - image_pos.x);
				int relY = (int)(mouse_pos.y - image_pos.y);

				int tile_x = relX / tile_size.x;
				int tile_y = relY / tile_size.y;

				texture_rect = uint_rect(tile_x * tile_size.x, tile_y * tile_size.y, tile_size);

				is_selected = true;
			}
		}
		ImGui::End();
		return is_selected;
	}

	bool ScenePanel::on_key_pressed(KeyPressedEvent& e)
	{
		bool control = Keyboard::is_key_pressed(Key::LeftControl) || Keyboard::is_key_pressed(Key::RightControl);
		bool shift = Keyboard::is_key_pressed(Key::LeftShift) || Keyboard::is_key_pressed(Key::RightShift);
		if (control)
		{
			if (e.get_key_code() == Key::D && m_selected_entity)
			{
				auto new_entity = m_scene->duplicate_entity(m_selected_entity);
				
				m_selected_entity = new_entity;


			}
			return false;
		}

		//Transformation Setting
		if (!m_selected_entity)
			return false;
		switch (e.get_key_code())
		{
		case Key::G: reset_transform_setting(); m_current_transform_setting = TransformSetting::Move; break;
		case Key::S: reset_transform_setting(); m_current_transform_setting = TransformSetting::Scale; break;
		case Key::R: reset_transform_setting(); m_current_transform_setting = TransformSetting::Rotate; break;
		case Key::X: m_current_transform_axis = TransformAxis::X; break;
		case Key::Y: m_current_transform_axis = TransformAxis::Y; break;
		case Key::Escape:
			if (m_current_transform_setting != TransformSetting::None)
				reset_transform_setting();
			break;

		case Key::Delete:
		{
			m_scene->destroy_entity(m_selected_entity);
			m_selected_entity = Entity();
			break;
		}

		}
		return false;
	}

	bool ScenePanel::on_mouse_pressed(MouseButtonPressedEvent& e)
	{
		if (!m_selected_entity)
			return false;

		if (m_selected_entity.has_component<Tag>() && EditorLayer::get().is_viewport_hovered())
		{
			const auto& tag = m_selected_entity.get_component<Tag>();
			if (tag.node_type == NodeType::TileMap)
			{
				auto& props = m_selected_entity.get_component<TileMapNode::TileMapProp>();
				if (props.display_ghost)
				{
					vec2f mouse_position = EditorLayer::get().get_viewport_mouse_position();
					vec2i tile_pos = {
						(int)std::floor((mouse_position.x - props.offset.x) / props.size.x),
						(int)std::floor((mouse_position.y - props.offset.y) / props.size.y)
					};

					if (e.get_mouse_button() == Button::ButtonLeft)
					{
						TileMapNode::Tile tile;
						//tile.position = tile_pos;
						//tile.texture_rect = m_texture_rect;
						props.tiles[tile_pos] = tile;
					}
					else if (e.get_mouse_button() == Button::ButtonRight)
					{
						if (props.tiles.contains(tile_pos))
						{
							auto it = props.tiles.find(tile_pos);
							if (it != props.tiles.end())
							{
								props.tiles.erase(it);
							}
						}
					}

				}
			}
		}


		return false;
	}
}
