#include <Panels/ScenePanel.hpp>
#include <Application/EditorLayer.hpp>
#include <Node/NodeProperties.hpp>
#include <UI/UI.hpp>
#include <queue>

namespace ag
{
	static const uint8_t bit_lookup[3][3] =
	{
			{ TL,  T,  TR },
			{ L,   0,  R  },
			{ BL,  B,  BR }
	};

	namespace Icons {
		constexpr const char* PLUS = "[+]";
		constexpr const char* SEARCH = "[S]";
		constexpr const char* FILTER = "[F]";
		constexpr const char* TRASH = "[X]";
		constexpr const char* CLONE = "[C]";
		constexpr const char* EYE = "[O]";
		constexpr const char* EYE_SLASH = "[0]";
		constexpr const char* LOCK = "[L]";
		constexpr const char* UNLOCK = "[U]";

		constexpr const char* EXPAND_ALL = "[>]";
		constexpr const char* COLLAPSE_ALL = "[v]";
		constexpr const char* EXPAND = "[>]";
		constexpr const char* COLLAPSE = "[v]";
	}


	ScenePanel::ScenePanel(const AG_ref<Scene>& scene)
	{
		set_scene(scene);

		NodeProperties::register_components();

		NodeFactory::properties_map[NodeType::Rectangle] = NodeProperties::rectangle_2D;
		NodeFactory::properties_map[NodeType::Circle] = NodeProperties::circle_2D;
		NodeFactory::properties_map[NodeType::Sprite] = NodeProperties::sprite_2D;
		NodeFactory::properties_map[NodeType::AnimatedSprite2D] = NodeProperties::animated_sprite_2D;
		NodeFactory::properties_map[NodeType::Camera] = NodeProperties::camera_2D;
		NodeFactory::properties_map[NodeType::TileMap] = NodeProperties::tilemap_2D;
		NodeFactory::properties_map[NodeType::Scene2D] = NodeProperties::scene_2D;
		NodeFactory::properties_map[NodeType::Text] = NodeProperties::text_2D;
		NodeFactory::properties_map[NodeType::Button] = NodeProperties::button_2D;
		NodeFactory::properties_map[NodeType::TextureButton] = NodeProperties::texture_button_2D;
		NodeFactory::properties_map[NodeType::Audio] = NodeProperties::audio_2D;

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


		if (has_selected_entity())
		{
			if (m_selected_entity.has_component<Tag_Component>() && EditorLayer::get().is_viewport_hovered())
			{
				const auto& tag = m_selected_entity.get_component<Tag_Component>();
				if (tag.node_type == NodeType::TileMap)
					tile_map_draw();
			}
			if (m_selected_entity.has_component<Text_Component>())
			{
				if (!m_selected_entity.has_component<Text_Editor_State>())
					Text_Editor_State::add_component(m_selected_entity);
			}
		}
		m_last_mouse_position = m_current_mouse_position;
	}

	void ScenePanel::on_event(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<TextInputEvent>(AERO_BIND_EVENT_FN(ScenePanel::on_text_input));
		dispatcher.Dispatch<KeyPressedEvent>(AERO_BIND_EVENT_FN(ScenePanel::on_key_pressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(AERO_BIND_EVENT_FN(ScenePanel::on_mouse_pressed));
	}

	void ScenePanel::set_selected_entity(Entity entity)
	{
		if (has_selected_entity() && m_selected_entity.has_component<Text_Editor_State>())
		{
			auto& state = m_selected_entity.get_component<Text_Editor_State>();
			state.active = false;
		}
		if (m_selected_entity.get_id() == entity.get_id() && has_selected_entity())
		{
			if (entity && entity.has_component<Text_Editor_State>())
			{
				auto& state = entity.get_component<Text_Editor_State>();
				state.active = true;
			}
		}


		m_selected_entity = entity;
	}

	void ScenePanel::on_imgui_render()
	{
		UI::draw_menu_bar();
		UI::content_browser();
		UI::popup_functions();


		draw_scene_hierarchy();


		draw_properties_panel();

		if (m_show_create_panel)
			draw_create_object();

		if (m_selected_entity)
		{
			auto& tag = m_selected_entity.get_component<Tag_Component>();
			if (tag.node_type == NodeType::TileMap)
			{
				if (m_selected_entity.has_component<Tile_Component>())
				{
					auto& props = m_selected_entity.get_component<Tile_Component>();
					vec2u tile_id = m_tile_id;
					if (UI::draw_tilemap_selector(m_selected_entity, m_tile_id, m_active_set, m_use_auto_tile))
					{
						m_tile_id = tile_id;
					}
				}
			}
			if (m_selected_entity.has_component<TextureRect_Component>())
			{
				auto& rects = m_selected_entity.get_component<TextureRect_Component>();
				auto& texture = m_selected_entity.get_component<Texture_Component>().texture;

				uint_rect texture_rect;
				if (texture)
				{
					if (UI::texture_selector(m_selected_entity, texture_rect))
					{
						rects.rect = texture_rect;
						if (m_selected_entity.has_component<Textured_Button_Component>())
						{
							auto& comps = m_selected_entity.get_component<Textured_Button_Component>();
							comps.base_rect = texture_rect;
							comps.overrides[comps.current_state] = texture_rect;
						}
						if (m_selected_entity.has_component<Render2D_Component>())
						{
							auto& sizes = m_selected_entity.get_component<Render2D_Component>();
							sizes.size = texture_rect.size;
						}
					}
				}
			}
		}

		UI::draw_console();
	}

	void ScenePanel::draw_node_hierarchy(Entity entity, int level)
	{
		auto& tag = entity.get_component<Tag_Component>();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		if (m_selected_entity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (tag.children.empty())
			flags |= ImGuiTreeNodeFlags_Leaf;

		ImGui::Indent(level * 10.0f);

		// Draw the tree node
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entity.get_id(), flags, "%s", tag.name.c_str());

		// Selection
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			if (m_selected_entity != entity)
			{
				m_move_flag = false;
				m_scale_flag = false;
				m_rotate_flag = false;
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

	}


	void ScenePanel::draw_create_object() {
		if (!m_show_create_panel) return;

		static CreatePanelState state;

		ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoCollapse;
		//ImGuiWindowFlags_AlwaysAutoResize;

		ImGui::SetNextWindowSize(ImVec2(350, 500), ImGuiCond_FirstUseEver);
		ImGui::Begin("Create Objects", &m_show_create_panel, window_flags);

		// Header with search
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 8));
		{
			ImGui::BeginGroup();

			// Search bar
			ImGui::SetNextItemWidth(-1);
			ImGui::InputTextWithHint("##search", "Search objects...",
				state.search_filter.data(), ImGuiInputTextFlags_AutoSelectAll);

			// Quick filter buttons
			ImGui::BeginGroup();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 3));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));

			if (ImGui::Button("All", ImVec2(80, 0))) {
				state.show_3d_objects = state.show_lights =
					state.show_ui = state.show_primitives = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("UI", ImVec2(80, 0))) {
				state.show_ui = !state.show_ui;
			}

			ImGui::PopStyleVar(2);
			ImGui::EndGroup();

			ImGui::EndGroup();
		}
		ImGui::PopStyleVar();

		ImGui::Separator();

		// Content area
		ImGui::BeginChild("ContentArea", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 1.5f));
		{
			// Group objects by category
			std::unordered_map<std::string, std::vector<std::pair<NodeType, std::string>>> categorized_nodes;

			for (const auto& [type, name] : NodeFactory::nodes) {
				std::string category = categorize_node_type(type);
				categorized_nodes[category].emplace_back(type, name);
			}

			// Display categories
			for (const auto& [category, nodes] : categorized_nodes) {

				if (!should_show_category(category, state)) continue;

				// Filter by search
				std::vector<std::pair<NodeType, std::string>> filtered_nodes;
				for (const auto& [type, name] : nodes) {
					if (state.search_filter.empty() ||
						string_contains_case_insensitive(name, state.search_filter)) {
						filtered_nodes.emplace_back(type, name);
					}
				}

				if (filtered_nodes.empty()) continue;

				// Category header
				bool category_open = ImGui::CollapsingHeader(
					fmt::format("{} ({})", category, filtered_nodes.size()).c_str(),
					ImGuiTreeNodeFlags_DefaultOpen);

				if (category_open) {
					ImGui::Indent(10.0f);

					if (state.show_categories)
					{
						for (const auto& [type, name] : filtered_nodes)
						{
							draw_object_button(type, name, state);
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
						}
					}
					else {

						for (const auto& [type, name] : filtered_nodes) {
							draw_object_list_item(type, name, state);
						}
					}

					ImGui::Unindent(10.0f);
					ImGui::Spacing();
				}
			}
		}
		ImGui::EndChild();

		ImGui::Separator();

		ImGui::BeginChild("FooterArea", ImVec2(0, 80), true);
		{
			auto it = NodeFactory::nodes.find(state.selected_prefab);

			if (it != NodeFactory::nodes.end()) {
				ImGui::Columns(2, "##footer_columns", false);
				ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() - 120);

				ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "Selected:");
				ImGui::SameLine();
				ImGui::Text("%s", it->second.c_str());

				ImGui::NextColumn();

				bool can_create = NodeFactory::create_map.find(state.selected_prefab) !=
					NodeFactory::create_map.end();

				ImGui::BeginDisabled(!can_create);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));

				if (ImGui::Button("CREATE", ImVec2(100, 0)))
				{
					create_selected_object(state.selected_prefab);
					m_scene->set_save_required(true);
				}

				ImGui::PopStyleColor(3);
				ImGui::EndDisabled();

				ImGui::Columns(1);

				// Status message
				auto now = std::chrono::steady_clock::now();
				auto time_since_creation = std::chrono::duration_cast<std::chrono::milliseconds>(
					now - state.last_created_time).count();

				if (time_since_creation < 1500) {
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ Created successfully");
				}
			}
			else {
				// Centered "no selection" message
				float text_width = ImGui::CalcTextSize("Select an object to begin").x;
				ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_width) * 0.5f);
				ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.5f - ImGui::GetTextLineHeight() * 0.5f);
				ImGui::TextDisabled("Select an object to begin");
			}
		}
		ImGui::EndChild();
		ImGui::End();
	}



	void ScenePanel::draw_scene_hierarchy() {
		if (!m_scene) return;

		ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar;

		ImGui::Begin("Scene", nullptr, window_flags);

		//Tool Bar the Bar above the Scene
		draw_hierarchy_toolbar();

		ImGui::Separator();


		if (m_hierarchy_state.show_filter) {
			draw_hierarchy_filter();
		}

		// Content area

		const char* child_name = "HierarchyContent";
		ImGui::BeginChild(child_name, ImVec2(0, 0), true);
		{
			// Get all root entities (no parent)
			auto view = m_scene->m_registry.view<Tag_Component>();
			std::vector<Entity> root_entities;

			for (auto entity_id : view) {
				Entity entity(entity_id);
				auto& tag = entity.get_component<Tag_Component>();
				if (!tag.parent || tag.parent.get_id() == INVALID_ENTITY) {
					root_entities.push_back(entity);
				}
			}

			// Draw root entities
			for (auto& entity : root_entities) {
				draw_entity_node(entity, 0);
			}

			// Draw unparented entities that might have been filtered
			if (!m_hierarchy_state.filter_text.empty()) {
				//draw_filtered_entities();
			}

			// Handle drag and drop target
			//handle_drag_drop_target();

			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_selected_entity = Entity{};
				m_move_flag = false;
				m_scale_flag = false;
				m_rotate_flag = false;
			}
		}
		ImGui::EndChild();

		if (ImGui::BeginPopupContextWindow("HierarchyContext", ImGuiPopupFlags_MouseButtonRight)) {
			draw_hierarchy_context_menu(nullptr);
			ImGui::EndPopup();
		}

		ImGui::End();
	}

	//Draw Entity Node
	void ScenePanel::draw_entity_node(Entity entity, int level) {
		if (!entity || !entity.has_component<Tag_Component>()) return;

		auto& tag = entity.get_component<Tag_Component>();
		EntityID entity_id = entity.get_id();

		// Skip filtered out entities
		if (m_hierarchy_state.filtered_out_entities.find(entity_id) !=
			m_hierarchy_state.filtered_out_entities.end()) {
			return;
		}

		// Apply filter
		if (!m_hierarchy_state.filter_text.empty()) {
			std::string entity_name_lower = tag.name;
			std::transform(entity_name_lower.begin(), entity_name_lower.end(),
				entity_name_lower.begin(), ::tolower);
			std::string filter_lower = m_hierarchy_state.filter_text;
			std::transform(filter_lower.begin(), filter_lower.end(),
				filter_lower.begin(), ::tolower);

			if (entity_name_lower.find(filter_lower) == std::string::npos) {
				m_hierarchy_state.filtered_out_entities.insert(entity_id);
				return;
			}
		}

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_FramePadding;

		bool is_selected = (m_selected_entity == entity);
		bool is_expanded = m_hierarchy_state.expanded_nodes.find(entity_id) !=
			m_hierarchy_state.expanded_nodes.end();

		if (is_selected) {
			flags |= ImGuiTreeNodeFlags_Selected;
			if (m_hierarchy_state.auto_expand_to_selection && !is_expanded) {
				flags |= ImGuiTreeNodeFlags_DefaultOpen;
			}
		}

		if (tag.children.empty()) {
			flags |= ImGuiTreeNodeFlags_Leaf;
		}

		// Push unique ID for this node
		ImGui::PushID(static_cast<int>(entity_id));


		ImGui::Indent(level * m_hierarchy_state.indent_size);
		push_entity_style(entity, is_selected);

		// Tree node
		bool node_open = ImGui::TreeNodeEx("##node", flags, "%s", tag.name.c_str());

		// Handle interactions
		handle_entity_interactions(entity);

		bool should_start_drag = ImGui::IsItemActive() &&
			ImGui::IsMouseDragging(ImGuiMouseButton_Left);

		if (should_start_drag)
		{
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				m_hierarchy_state.dragged_entity = entity;
				ImGui::SetDragDropPayload("ENTITY_NODE", &entity_id, sizeof(EntityID));
				ImGui::Text("%s", tag.name.c_str());
				ImGui::EndDragDropSource();
			}
		}
		else if (m_hierarchy_state.dragged_entity == entity)
		{
			if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				m_hierarchy_state.dragged_entity = Entity{};
			}
		}


		// Drag and drop target
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_NODE"))
			{
				EntityID dragged_id = *(const EntityID*)payload->Data;
				if (dragged_id != entity_id)
				{
					reparent_entity(dragged_id, entity_id);
				}
			}
			ImGui::EndDragDropTarget();
		}

		// Pop style
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();

		// Draw children if node is open
		if (node_open)
		{
			m_hierarchy_state.expanded_nodes.insert(entity_id);

			for (auto& child : tag.children)
			{
				draw_entity_node(child, level + 1);
			}

			ImGui::TreePop();
		}
		else
		{
			m_hierarchy_state.expanded_nodes.erase(entity_id);
		}

		ImGui::Unindent(level * m_hierarchy_state.indent_size);

		ImGui::PopID();
	}

	//Draw the tool bar
	void ScenePanel::draw_hierarchy_toolbar() {
		ImGui::BeginChild("HierarchyToolbar", ImVec2(0, 50), false);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));


			if (draw_toolbar_button(Icons::PLUS, "Create new object"))
			{
				m_show_create_panel = true;
			}

			ImGui::SameLine();


			ImGui::BeginDisabled(!m_selected_entity);
			if (draw_toolbar_button(Icons::TRASH, "Delete selected")) {
				//delete_selected_entity();
			}
			ImGui::EndDisabled();

			ImGui::SameLine();

			// Duplicate button
			ImGui::BeginDisabled(!m_selected_entity);
			if (draw_toolbar_button(Icons::CLONE, "Duplicate selected")) {
				//duplicate_selected_entity();
			}
			ImGui::EndDisabled();

			ImGui::SameLine();

			// Filter toggle
			if (draw_toolbar_button(
				m_hierarchy_state.show_filter ? Icons::FILTER : Icons::SEARCH,
				"Toggle filter")) {
				m_hierarchy_state.show_filter = !m_hierarchy_state.show_filter;
			}

			ImGui::SameLine();

			// Expand all
			if (draw_toolbar_button(Icons::EXPAND_ALL, "Expand all")) {
				expand_all_nodes();
			}

			ImGui::SameLine();

			// Collapse all
			if (draw_toolbar_button(Icons::COLLAPSE_ALL, "Collapse all")) {
				collapse_all_nodes();
			}

			// Right-aligned buttons
			ImGui::SameLine(ImGui::GetWindowWidth() - 150);

			// Visibility toggle
			if (m_selected_entity && m_selected_entity.has_component<Tag_Component>()) {
				auto& vis = m_selected_entity.get_component<Tag_Component>();
				if (draw_toolbar_button(
					vis.visible ? Icons::EYE : Icons::EYE_SLASH,
					vis.visible ? "Hide object" : "Show object")) {
					vis.visible = !vis.visible;
				}
			}

			ImGui::SameLine();

			// Lock toggle
			if (m_selected_entity && m_selected_entity.has_component<Tag_Component>()) {
				auto& lock = m_selected_entity.get_component<Tag_Component>();
				if (draw_toolbar_button(
					lock.locked ? Icons::LOCK : Icons::UNLOCK,
					lock.locked ? "Unlock object" : "Lock object")) {
					lock.locked = !lock.locked;
				}
			}

			ImGui::PopStyleVar(2);
		}
		ImGui::EndChild();
	}

	//Draw Filter Box
	void ScenePanel::draw_hierarchy_filter() {
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));

		ImGui::BeginChild("FilterArea", ImVec2(0, 40), false);
		{
			// Filter input
			ImGui::SetNextItemWidth(-1);
			if (ImGui::InputTextWithHint("##Filter", "Type to filter...",
				m_hierarchy_state.filter_text.data(),
				ImGuiInputTextFlags_AutoSelectAll)) {
				update_filter();
			}

			// Quick filter buttons
			if (!m_hierarchy_state.filter_text.empty()) {
				ImGui::SameLine();
				if (ImGui::Button("Clear")) {
					m_hierarchy_state.filter_text.clear();
					m_hierarchy_state.filtered_out_entities.clear();
				}
			}
		}
		ImGui::EndChild();

		ImGui::PopStyleVar(2);
	}

	//Handle Interaction
	void ScenePanel::handle_entity_interactions(Entity entity) {
		// Left click (select entity)
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
			select_entity(entity);
		}

		// Right click (context menu)
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
			ImGui::OpenPopup("EntityContextMenu");
			set_selected_entity(entity);
		}


		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			//focus_entity_in_viewport(entity);
		}

		// Context menu
		if (ImGui::BeginPopup("EntityContextMenu")) {
			draw_entity_context_menu(entity);
			ImGui::EndPopup();
		}
	}

	void ScenePanel::draw_entity_context_menu(Entity entity) {
		if (ImGui::MenuItem("Rename", "F2"))
		{
			//start_renaming_entity(entity);
			m_scene->set_save_required(true);
		}

		if (ImGui::MenuItem("Duplicate", "Ctrl+D"))
		{
			duplicate_entity();
			m_scene->set_save_required(true);
		}

		if (ImGui::MenuItem("Delete", "Del"))
		{
			delete_entity();
			m_scene->set_save_required(true);
		}

		if (ImGui::MenuItem("Make Root", "Ctrl+Shift+R"))
		{
			make_root_entity();
			m_scene->set_save_required(true);
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("Create Child")) {

			if (ImGui::MenuItem("Empty"))
			{
				//create_child_entity(entity, "Empty", NodeType::Empty);
				m_scene->set_save_required(true);
			}
			if (ImGui::MenuItem("Cube"))
			{
				//create_child_entity(entity, "Cube", NodeType::Cube);
				m_scene->set_save_required(true);
			}
			if (ImGui::MenuItem("Light"))
			{
				//create_child_entity(entity, "Light", NodeType::PointLight);
				m_scene->set_save_required(true);
			}
			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Copy Path")) {
			//copy_entity_path(entity);
		}

		if (ImGui::MenuItem("Find in Project")) {
			//find_entity_in_project(entity);
		}

		ImGui::Separator();

		// Component toggles
		if (entity.has_component<Tag_Component>())
		{
			auto& vis = entity.get_component<Tag_Component>();
			if (ImGui::MenuItem(vis.visible ? "Hide" : "Show"))
			{
				vis.visible = !vis.visible;
				m_scene->set_save_required(true);
			}
		}

		if (entity.has_component<Tag_Component>())
		{
			auto& lock = entity.get_component<Tag_Component>();
			if (ImGui::MenuItem(lock.locked ? "Unlock" : "Lock"))
			{
				lock.locked = !lock.locked;
				m_scene->set_save_required(true);
			}
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Properties", "Alt+Enter"))
		{
			// Show properties panel
		}
	}

	void ScenePanel::draw_hierarchy_context_menu(Entity* entity) {
		if (ImGui::MenuItem("Create Empty")) {
			/*Entity new_entity = m_scene->create_entity("Empty", NodeType::Empty);
			m_selected_entity = new_entity;*/
			m_scene->set_save_required(true);
		}

		if (ImGui::MenuItem("Create From Prefab...")) {
			// Open prefab browser
			m_scene->set_save_required(true);
		}

		ImGui::Separator();

		//has_clipboard_entity()
		if (ImGui::MenuItem("Paste", "Ctrl+V", false, false))
		{
			//paste_entity();
			m_scene->set_save_required(true);
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Expand All")) {
			expand_all_nodes();

		}

		if (ImGui::MenuItem("Collapse All")) {
			collapse_all_nodes();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Show Icons")) {
			m_hierarchy_state.show_icons = !m_hierarchy_state.show_icons;
		}

		if (ImGui::MenuItem("Auto-expand to Selection")) {
			m_hierarchy_state.auto_expand_to_selection = !m_hierarchy_state.auto_expand_to_selection;
		}
	}

	// Drawing Each Tool Bar
	bool ScenePanel::draw_toolbar_button(const char* icon, const char* tooltip) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 0.8f));

		ImGui::SetWindowFontScale(1.2f);
		bool clicked = ImGui::Button(icon, ImVec2(40, 40));
		ImGui::SetWindowFontScale(1.0f);
		ImGui::PopStyleColor(3);

		if (tooltip && ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("%s", tooltip);
		}

		return clicked;
	}

	void ScenePanel::push_entity_style(Entity entity, bool is_selected) {
		// Default styles
		ImVec4 text_color = m_hierarchy_state.default_text_color;
		ImVec4 bg_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		float frame_rounding = 2.0f;

		// Selected entity
		if (is_selected) {
			bg_color = m_hierarchy_state.selected_color;
		}

		// Disabled entity
		if (entity.has_component<Tag_Component>() &&
			!entity.get_component<Tag_Component>().visible) {
			text_color = m_hierarchy_state.disabled_color;
		}

		// Prefab instance
		/*if (entity.has_component<PrefabInstance>()) {
			text_color = m_hierarchy_state.prefab_color;
		}*/

		// Locked entity
		if (entity.has_component<Tag_Component>() &&
			entity.get_component<Tag_Component>().locked)
		{
			text_color = m_hierarchy_state.disabled_color;
		}

		ImGui::PushStyleColor(ImGuiCol_Text, text_color);
		ImGui::PushStyleColor(ImGuiCol_Header, bg_color);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, bg_color);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, frame_rounding);
	}

	std::string ScenePanel::get_icon_for_entity(Entity entity) {
		if (!entity.has_component<Tag_Component>()) return "?";

		auto& tag = entity.get_component<Tag_Component>();

		switch (tag.node_type) {
			//case NodeType::Empty: return ICON_FA_CUBE;
		case NodeType::Camera: return "ICON_FA_VIDEO";
			//case NodeType::Light: return ICON_FA_LIGHTBULB;
			//case NodeType::Mesh: return ICON_FA_CUBES;
		case NodeType::Sprite: return "ICON_FA_IMAGE";
		case NodeType::Text: return "ICON_FA_FONT";
		case NodeType::Button: return "ICON_FA_MOUSE_POINTER";
			//case NodeType::AudioSource: return ICON_FA_VOLUME_UP;
			//case NodeType::ParticleSystem: return ICON_FA_FIRE;
			//case NodeType::RigidBody: return ICON_FA_WEIGHT;
			//case NodeType::Collider: return ICON_FA_SHIELD_ALT;
			//case NodeType::Script: return ICON_FA_CODE;
			//case NodeType::Canvas: return ICON_FA_LAYER_GROUP;
		case NodeType::TileMap: return "ICON_FA_TH";
		default: return "ICON_FA_CUBE";
		}
	}

	void ScenePanel::select_entity(Entity entity) {
		m_selected_entity = entity;
		m_move_flag = m_scale_flag = m_rotate_flag = false;

		//// Handle tilemap specific logic
		//if (m_selected_entity.has_component<Tag_Component>() &&
		//	m_selected_entity.get_component<Tag_Component>().node_type == NodeType::TileMap) {
		//	auto& props = m_selected_entity.get_component<TileMapNode::TileMapProp>();
		//	props.display_ghost = true;
		//}

		// Notify selection change
		//on_entity_selected(entity);
	}

	void ScenePanel::update_filter() {
		m_hierarchy_state.filtered_out_entities.clear();
		if (m_hierarchy_state.filter_text.empty()) return;

		// Filter will be applied in draw_entity_node
	}

	void ScenePanel::expand_all_nodes() {
		auto view = m_scene->m_registry.view<Tag_Component>();
		for (auto entity_id : view) {
			m_hierarchy_state.expanded_nodes.insert(static_cast<EntityID>(entity_id));
		}
	}

	void ScenePanel::collapse_all_nodes()
	{
		m_hierarchy_state.expanded_nodes.clear();
	}

	void ScenePanel::reparent_entity(EntityID child_id, EntityID new_parent_id)
	{
		Entity child(static_cast<entt::entity>(child_id));
		Entity new_parent(static_cast<entt::entity>(new_parent_id));

		m_hierarchy_state.dragged_entity = Entity{};
		m_hierarchy_state.drop_target = Entity{};

		if (!child || !new_parent) return;

		auto& child_tag = child.get_component<Tag_Component>();
		auto& parent_tag = new_parent.get_component<Tag_Component>();

		Transform_Component child_transform;
		if (child.has_component<Transform_Component>())
			child_transform = Transform_Component::get_world_transform(child);


		if (child_tag.parent)
		{
			auto& old_parent_tag = child_tag.parent.get_component<Tag_Component>();
			auto it = std::find(old_parent_tag.children.begin(),
				old_parent_tag.children.end(), child);
			if (it != old_parent_tag.children.end())
			{
				old_parent_tag.children.erase(it);
			}
		}
		child_tag.parent = new_parent;
		parent_tag.children.push_back(child);


		if (child.has_component<Transform_Component>())
		{
			Transform_Component::get_local_transform(child, child_transform);
		}


		AERO_INFO("Reparented {} to {}",
			child_tag.name, parent_tag.name);
	}

	void ScenePanel::make_root_entity()
	{
		if (!m_selected_entity && m_selected_entity.get_id() == INVALID_ENTITY)
			return;

		auto& tag = m_selected_entity.get_component<Tag_Component>();
		auto child_transform = Transform_Component::get_world_transform(m_selected_entity);

		if (tag.parent && tag.parent.get_id() != INVALID_ENTITY)
		{
			auto& parent_tag = tag.parent.get_component<Tag_Component>();
			auto it = std::find(parent_tag.children.begin(),
				parent_tag.children.end(), m_selected_entity);
			if (it != parent_tag.children.end())
			{
				parent_tag.children.erase(it);
			}
			tag.parent = Entity{};
		}

		auto& transform = m_selected_entity.get_component<Transform_Component>();
		transform = child_transform;
	}





	// Helper functions
	std::string ScenePanel::categorize_node_type(NodeType type) {
		switch (type) {
		case NodeType::Rectangle:
		case NodeType::Circle:
		case NodeType::Sprite:
		case NodeType::AnimatedSprite2D:
		case NodeType::Text:
			return "Primitives";

		case NodeType::Button:
		case NodeType::TextureButton:
			return "UI";


		case NodeType::Camera:
		case NodeType::Scene2D:
			return "Components";

		default:
			return "Other";
		}
	}

	bool ScenePanel::should_show_category(const std::string& category, const CreatePanelState& state) {
		if (!state.search_filter.empty()) return true;

		if (category == "Primitives") return state.show_primitives;
		if (category == "3D Objects") return state.show_3d_objects;
		if (category == "Lights") return state.show_lights;
		if (category == "UI") return state.show_ui;
		if (category == "Empty") return true;
		if (category == "Components") return true;
		if (category == "Other") return true;

		return true;
	}

	void ScenePanel::draw_object_button(NodeType type, const std::string& name, CreatePanelState& state) {
		ImGui::PushID(static_cast<int>(type));

		const bool is_selected = (state.selected_prefab == type);

		// Button style
		ImVec4 button_color = is_selected ?
			ImVec4(0.26f, 0.59f, 0.98f, 0.4f) :
			ImVec4(0.2f, 0.2f, 0.2f, 0.3f);

		ImGui::PushStyleColor(ImGuiCol_Button, button_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 0.8f));

		// Draw button
		ImVec2 button_size = state.icon_size;

		std::string display_text = name;
		ImVec2 text_size = ImGui::CalcTextSize(name.c_str());
		const float max_text_width = button_size.x - 10.0f;
		if (text_size.x > max_text_width)
		{
			float avg_char_width = text_size.x / display_text.length();
			int max_chars = static_cast<int>(max_text_width / avg_char_width) - 3;
			if (max_chars > 3)
			{
				display_text = display_text.substr(0, max_chars) + "...";
				text_size = ImGui::CalcTextSize(display_text.c_str());
			}
		}

		if (ImGui::Button(display_text.c_str(), button_size)) {
			state.selected_prefab = type;
		}

		ImGui::PopStyleColor(3);


		if (is_selected) {
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			ImVec2 rect_min = ImGui::GetItemRectMin();
			ImVec2 rect_max = ImGui::GetItemRectMax();

			draw_list->AddRect(rect_min, rect_max,
				IM_COL32(100, 200, 255, 255),
				4.0f, 0, 2.0f);
		}


		// Context menu
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Select")) {
				state.selected_prefab = type;
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Create Now")) {
				create_selected_object(type);
				m_show_create_panel = false;
			}
			ImGui::EndPopup();
		}

		// Tooltip
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("%s", name.c_str());
			ImGui::TextDisabled("Click to select, Right-click for options");
			ImGui::EndTooltip();
		}

		ImGui::PopID();
	}

	void ScenePanel::draw_object_list_item(NodeType type, const std::string& name, CreatePanelState& state) {
		const bool is_selected = (state.selected_prefab == type);

		ImGui::PushID(static_cast<int>(type));

		// Selectable item
		if (ImGui::Selectable(name.c_str(), is_selected)) {
			state.selected_prefab = type;
		}

		// Context menu
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Create")) {
				create_selected_object(type);
				m_show_create_panel = false;
			}
			ImGui::EndPopup();
		}

		// Drag and drop support
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("NODE_TYPE", &type, sizeof(NodeType));
			ImGui::Text("Create %s", name.c_str());
			ImGui::EndDragDropSource();
		}

		ImGui::PopID();
	}

	void ScenePanel::create_selected_object(NodeType type)
	{
		auto it = NodeFactory::nodes.find(type);
		auto create_it = NodeFactory::create_map.find(type);

		if (it == NodeFactory::nodes.end() || create_it == NodeFactory::create_map.end()) {
			AERO_WARN("Cannot create object: type {} not registered", static_cast<int>(type));
			return;
		}

		// Create the entity
		Entity new_entity = m_scene->create_entity(it->second, type);
		// Parent to selected entity if one is selected
		if (m_selected_entity)
		{
			auto& selected_tag = m_selected_entity.get_component<Tag_Component>();
			auto& new_tag = new_entity.get_component<Tag_Component>();
			new_tag.parent = m_selected_entity;
			selected_tag.children.push_back(new_entity);
		}

		// Select the new entity
		m_selected_entity = new_entity;
		m_show_create_panel = false;
	}

	bool ScenePanel::string_contains_case_insensitive(const std::string& str, const std::string& substr) {
		if (substr.empty()) return true;

		auto it = std::search(
			str.begin(), str.end(),
			substr.begin(), substr.end(),
			[](char ch1, char ch2) {
				return std::tolower(ch1) == std::tolower(ch2);
			}
		);
		return it != str.end();
	}







	void ScenePanel::draw_properties_panel()
	{
		ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoScrollbar);
		if (m_selected_entity)
		{
			const auto& tag = m_selected_entity.get_component<Tag_Component>();
			auto it = NodeFactory::properties_map.find(tag.node_type);
			if (it != NodeFactory::properties_map.end())
				it->second(m_selected_entity);
		}
		ImGui::End();
	}

	void ScenePanel::draw_selected_text()
	{
		if (m_selected_entity.get_id() == INVALID_ENTITY)
			return;

		if (m_selected_entity.has_component<Transform_Component>())
		{
			int entity_id = (int)(m_selected_entity.get_id());
			auto& name = m_selected_entity.get_component<Tag_Component>().name;
			auto trans = Transform_Component::get_world_transform(m_selected_entity);
			Transform_Component transform;
			transform.position = Math::world_to_screen(trans.position, EditorLayer::get().get_float_rect(), EditorLayer::get().get_viewport_size());
			Text text;
			text.text = name;
			text.text_color = Color(220, 220, 220);
			text.font_size = 18;
			text.mode = RenderMode::Screen;
			Renderer2D::draw_text(text, transform, entity_id);
		}
	}

	void ScenePanel::draw_collision_shapes()
	{
		const auto& view = m_scene->get_view<CollisionShape_Component>();
		const auto& camera = EditorLayer::get().get_view();
		auto viewport_size = EditorLayer::get().get_viewport_size();
		Rectangle rect;
		Circle circle;
		for (auto id : view)
		{
			Entity e(id);
			int entity_id = (int)(e.get_id());
			auto& shapes = e.get_component<CollisionShape_Component>();
			auto transform = Transform_Component::get_world_transform(e);
			transform.position = Math::world_to_screen(transform.position, camera.get_float_rect(), viewport_size);
			switch (shapes.shape_type)
			{
			case ShapeType::Rectangle:
			{
				auto rect_size = Math::world_size_to_screen_size(shapes.size, camera.get_size(), viewport_size);
				rect.size = rect_size;
				rect.mode = RenderMode::Screen;
				rect.fill_color = Color(80, 180, 255, 40);
				rect.border_color = Color(30, 140, 230, 220);
				rect.border_thickness = -3.0f;
				Renderer2D::draw_rectangle(rect, transform, entity_id);
				break;
			}
			case ShapeType::Circle:
			{
				auto circle_size = Math::world_size_to_screen_size({ shapes.radius * 2,shapes.radius * 2 }, camera.get_size(), viewport_size);
				circle.size = circle_size;
				rect.mode = RenderMode::Screen;
				circle.fill_color = Color(80, 180, 255, 40);
				circle.border_color = Color(30, 140, 230, 220);
				circle.border_thickness = -3.0f;
				Renderer2D::draw_circle(circle, transform, entity_id);
				break;
			}
			default:
			{
				auto rect_size = Math::world_size_to_screen_size(shapes.size, camera.get_size(), viewport_size);
				rect.size = rect_size;
				rect.mode = RenderMode::Screen;
				rect.fill_color = Color(80, 180, 255, 40);
				rect.border_color = Color(30, 140, 230, 220);
				rect.border_thickness = -3.0f;
				Renderer2D::draw_rectangle(rect, transform, entity_id);
				break;
			}
			}

		}
	}

	void ScenePanel::update_transform_settings()
	{
		if (!m_selected_entity || !m_selected_entity.has_component<Transform_Component>() || !EditorLayer::get().is_viewport_hovered())
			return;

		static bool was_left_pressed = false;
		static bool was_right_pressed = false;

		bool is_left_pressed = Mouse::is_mouse_pressed(Button::ButtonLeft);
		bool is_right_pressed = Mouse::is_mouse_pressed(Button::ButtonRight);

		bool left_clicked = !was_left_pressed && is_left_pressed;
		bool right_clicked = !was_right_pressed && is_right_pressed;

		bool left_released = was_left_pressed && !is_left_pressed;
		bool right_released = was_right_pressed && !is_right_pressed;

		switch (m_current_transform_setting)
		{
		case ag::TransformSetting::None:
		{
			break;
		}
		case ag::TransformSetting::Scale:
		{
			if (is_left_pressed)
			{
				scale_transform_setting();
				m_scene->set_save_required(true);
			}
			else if (left_released)
			{
				m_initial_transform.scale = m_selected_entity.get_component<Transform_Component>().scale;
				reset_transform_setting();
			}

			if (right_clicked)
			{
				auto& scale = m_selected_entity.get_component<Transform_Component>().scale;
				scale = m_initial_transform.scale;
				reset_transform_setting();
			}
			break;
		}
		case ag::TransformSetting::Rotate:
		{
			if (is_left_pressed)
			{
				rotate_transform_setting();
				m_scene->set_save_required(true);
			}
			else if (left_released)
			{
				reset_transform_setting();
				m_initial_transform.rotation = m_selected_entity.get_component<Transform_Component>().rotation;
			}

			if (right_clicked)
			{
				auto& rotation = m_selected_entity.get_component<Transform_Component>().rotation;
				rotation = m_initial_transform.rotation;
				reset_transform_setting();
			}
			break;
		}
		case ag::TransformSetting::Move:
		{
			if (is_left_pressed)
			{
				move_transform_setting();
				m_scene->set_save_required(true);
			}
			else if (left_released)
			{
				m_initial_transform.position = m_selected_entity.get_component<Transform_Component>().position;
				reset_transform_setting();
			}

			if (right_clicked)
			{
				auto& position = m_selected_entity.get_component<Transform_Component>().position;
				position = m_initial_transform.position;
				reset_transform_setting();
			}
			break;
		}
		default: AERO_CORE_ERROR("No sucn Transform_Component Setting!"); break;
		}
		was_left_pressed = is_left_pressed;
		was_right_pressed = is_right_pressed;
	}
	void ScenePanel::move_transform_setting()
	{
		auto& position = m_selected_entity.get_component<Transform_Component>().position;
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
		auto& rotation = m_selected_entity.get_component<Transform_Component>().rotation;
		auto position = Transform_Component::get_world_transform(m_selected_entity).position;

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
		auto& transform = m_selected_entity.get_component<Transform_Component>();
		auto position = Transform_Component::get_world_transform(m_selected_entity).position;

		if (!m_scale_flag)
		{
			m_initial_transform.scale = transform.scale;
			m_last_mouse_position = m_current_mouse_position;
			m_scale_flag = true;
		}

		float initial_distance = (m_last_mouse_position - position).length();
		float current_distance = (m_current_mouse_position - position).length();

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

	void ScenePanel::draw_selection_box()
	{
		if (!m_selected_entity)
			return;

		if (m_selected_entity.get_id() == INVALID_ENTITY)
			return;

		if (m_selected_entity.has_component<Render2D_Component>() && m_selected_entity.has_component<Transform_Component>())
		{
			const auto& render2d = m_selected_entity.get_component<Render2D_Component>();
			Transform_Component transform = Transform_Component::get_world_transform(m_selected_entity);

			int entity_id = (int)(m_selected_entity.get_id());

			Rectangle rect;
			rect.size = render2d.size;
			rect.fill_color = Color(100, 100, 250, 80);

			Renderer2D::draw_rectangle(rect, transform, entity_id);
		}
	}


	void ScenePanel::duplicate_entity()
	{
		if (!m_selected_entity || m_selected_entity.get_id() == INVALID_ENTITY)
			return;

		auto& parent = m_selected_entity.get_component<Tag_Component>().parent;
		auto new_entity = m_scene->duplicate_entity(m_selected_entity, parent);

		m_selected_entity = new_entity;
		m_scene->set_save_required(true);
	}
	void ScenePanel::delete_entity()
	{
		m_scene->destroy_entity(m_selected_entity);
		m_selected_entity = Entity();
		m_scene->set_save_required(true);
	}

	void ScenePanel::reset_transform_setting()
	{
		m_current_transform_setting = TransformSetting::None;
		m_current_transform_axis = TransformAxis::None;
		m_move_flag = false;
		m_scale_flag = false;
		m_rotate_flag = false;
		m_delta = { 0, 0 };
		m_scene->set_save_required(true);
	}

	void ScenePanel::update_tilemap()
	{
		if (!m_selected_entity)
			return;

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
		if (!has_selected_entity())
			return false;

		bool control = Keyboard::is_key_pressed(Key::LeftControl) || Keyboard::is_key_pressed(Key::RightControl);
		bool shift = Keyboard::is_key_pressed(Key::LeftShift) || Keyboard::is_key_pressed(Key::RightShift);

		// Typing text
		if (m_selected_entity.has_component<Text_Editor_State>() && m_selected_entity.get_component<Text_Editor_State>().active &&
			m_selected_entity.has_component<Text_Component>())
		{
			auto& state = m_selected_entity.get_component<Text_Editor_State>();
			auto& props = m_selected_entity.get_component<Text_Component>();

			if (e.get_key_code() == Key::Left && state.caret_index > 0)
			{
				state.caret_index--;
				state.blink_timer = 0.0f;
				return true;
			}
			else if (e.get_key_code() == Key::Right && state.caret_index < (props.text.size()))
			{
				state.caret_index++;
				state.blink_timer = 0.0f;
				return true;
			}
			else if (e.get_key_code() == Key::Backspace)
			{
				if (control)
				{
					if (state.caret_index == 0)
						return true;

					size_t delete_start = state.caret_index;
					while (delete_start > 0 && props.text[delete_start - 1] == ' ')
						delete_start--;

					while (delete_start > 0 && props.text[delete_start - 1] != ' ')
						delete_start--;

					props.text.erase(props.text.begin() + delete_start, props.text.begin() + state.caret_index);

					state.caret_index = delete_start;
				}
				if (state.caret_index > 0)
				{
					props.text.erase(props.text.begin() + state.caret_index - 1);
					state.blink_timer = 0.0f;
					state.caret_index--;
				}
				return true;
			}
			else if (e.get_key_code() == Key::Enter)
			{
				props.text.insert(props.text.begin() + state.caret_index, '\n');
				state.blink_timer = 0.0f;
				state.caret_index++;
				return true;
			}
			else if (e.get_key_code() == Key::Tab)
			{
				props.text.insert(props.text.begin() + state.caret_index, '\t');
				state.blink_timer = 0.0f;
				state.caret_index++;
				return true;
			}
			else if (e.get_key_code() == Key::Escape)
			{
				state.blink_timer = 0.0f;
				state.active = false;
				return true;
			}
			else if (e.get_key_code() == Key::V)
			{
				if (control)
				{
					std::string clipboard_text = Application::get().get_window().get_clipboard_string();
					props.text.insert(props.text.begin() + state.caret_index, clipboard_text.begin(), clipboard_text.end());
					state.blink_timer = 0.0f;
					state.caret_index += clipboard_text.size();
				}
			}
			return false;
		}






		if (control)
		{

			if (e.get_key_code() == Key::D)
			{
				duplicate_entity();
				m_scene->set_save_required(true);
				return true;
			}

			// Make Root Entity
			if (e.get_key_code() == Key::R && shift)
			{
				make_root_entity();
				m_scene->set_save_required();
				return true;
			}
			return false;
		}

		switch (e.get_key_code())
		{
		case Key::G: reset_transform_setting(); m_current_transform_setting = TransformSetting::Move; return true;
		case Key::S: reset_transform_setting(); m_current_transform_setting = TransformSetting::Scale; return true;
		case Key::R: reset_transform_setting(); m_current_transform_setting = TransformSetting::Rotate; return true;
		case Key::X: m_current_transform_axis = TransformAxis::X; return true;
		case Key::Y: m_current_transform_axis = TransformAxis::Y; return true;
		case Key::Escape:
		{
			if (m_current_transform_setting != TransformSetting::None)
				reset_transform_setting();
			return true;
		}

		case Key::Delete:
		{
			if (!m_selected_entity.has_component<Text_Editor_State>())
			{
				delete_entity();
				m_scene->set_save_required();
				return true;
			}
		}

		}

		return false;
	}

	bool ScenePanel::on_mouse_pressed(MouseButtonPressedEvent& e)
	{
		return false;
	}

	bool ScenePanel::on_text_input(TextInputEvent& e)
	{
		if (!has_selected_entity())
			return false;

		bool control = Keyboard::is_key_pressed(Key::LeftControl) || Keyboard::is_key_pressed(Key::RightControl);
		if (control)
			return false;
		if (m_selected_entity.has_component<Text_Component>() && m_selected_entity.get_component<Text_Editor_State>().active && m_selected_entity.has_component<Text_Editor_State>())
		{
			auto& state = m_selected_entity.get_component<Text_Editor_State>();
			auto& text = m_selected_entity.get_component<Text_Component>();

			text.text.insert(text.text.begin() + state.caret_index, e.get_character());

			state.caret_index++;
			state.blink_timer = 0.0f;
			return true;
		}
	}





	bool ScenePanel::on_entity_clicked()
	{
		m_scene->m_registry.sort<Tag_Component>([](const Tag_Component& a, const Tag_Component& b)
			{ return a.index > b.index; });

		auto view = m_scene->m_registry.view<Tag_Component>();
		for (auto entityID : view)
		{
			Entity entity(entityID);
			auto& tag = entity.get_component<Tag_Component>();
			if (tag.parent.get_id() != INVALID_ENTITY)
				continue;

			for (auto e = tag.children.rbegin(); e != tag.children.rend(); ++e)
			{
				if (check_if_clicked(*e))
					return true;
			}

			if (check_if_clicked(entity))
				return true;
		}

		return false;
	}

	bool ScenePanel::check_if_clicked(Entity entity)
	{
		if (!entity.has_component<Transform_Component>())
			return false;

		if (!entity.has_component<Render2D_Component>())
			return false;

		auto& transform = entity.get_component<Transform_Component>();
		auto& render2d = entity.get_component<Render2D_Component>();

		vec2f position = transform.position - (render2d.size * transform.scale) * 0.5f;
		vec2f size = render2d.size * transform.scale;
		float_rect rect = { position, size };

		vec2f mouse_pos = EditorLayer::get().get_viewport_mouse_position();

		if (rect.contains(mouse_pos))
		{
			m_selected_entity = entity;
			AERO_CORE_INFO("Contains Entity");
			return true;
		}
		else
		{
			AERO_CORE_INFO("Doesnot Contains Entity");
			return false;
		}
	}

	void ScenePanel::tile_map_draw()
	{
		if (!m_selected_entity.has_component<Tile_Component>() || !m_selected_entity.has_component<TileSet_Component>())
			return;

		static bool dragging = false;
		static vec2i start_tile;
		static vec2i previous_tile = { std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };

		bool is_left = Mouse::is_mouse_pressed(Button::ButtonLeft);
		bool is_right = Mouse::is_mouse_pressed(Button::ButtonRight);


		auto& tile_set = m_selected_entity.get_component<TileSet_Component>();

		if ((!is_left && !is_right) || (is_left && is_right))
		{
			start_tile = vec2i();
			if (dragging)
			{
				for (const auto& [position, id] : temp_tiles)
				{
					paint_eraser_tiles_helper(tile_set, position);
				}
			}
			dragging = false;
			temp_tiles.clear();
			previous_tile = { std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };
			return;
		}

		auto& props = m_selected_entity.get_component<Tile_Component>();

		vec2f current_mouse = EditorLayer::get().get_viewport_mouse_position();

		vec2i current_tile = {
			(int)std::floor((current_mouse.x - props.offset.x) / props.size.x),
			(int)std::floor((current_mouse.y - props.offset.y) / props.size.y)
		};

		if (is_left && !is_right)
		{
			m_settings = TileMap_Settings::Paint;
			if (!dragging && (m_paint_settings == TileMap_Paint_Settings::Rectangle || m_paint_settings == TileMap_Paint_Settings::Line || m_paint_settings == TileMap_Paint_Settings::Fill))
			{
				start_tile = current_tile;
			}
			dragging = true;

		}
		else if (is_right && !is_left)
		{
			m_settings = TileMap_Settings::Eraser;
			if (!dragging && (m_paint_settings == TileMap_Paint_Settings::Rectangle || m_paint_settings == TileMap_Paint_Settings::Line || m_paint_settings == TileMap_Paint_Settings::Fill))
			{
				start_tile = current_tile;
			}
			dragging = true;
		}







		bool should_paint = true;

		if (dragging)
		{
			if (current_tile == previous_tile && m_settings == m_previous_settings)
			{
				should_paint = false;
			}
			else if (current_tile != previous_tile)
			{
				temp_tiles.clear();
			}
		}


		if (m_previous_settings != m_settings)
		{
			dragging = false;
			m_previous_settings = m_settings;
		}

		if (!should_paint)
		{
			previous_tile = current_tile;
			return;
		}


		switch (m_paint_settings)
		{
		case ag::TileMap_Paint_Settings::None:
		{
			{
				paint_eraser_tiles_helper(tile_set, current_tile);
			}
			break;
		}

		case ag::TileMap_Paint_Settings::Paint:
		{
			{
				if (previous_tile.x == std::numeric_limits<int>::max())
				{
					previous_tile = current_tile;
					paint_eraser_tiles_helper(tile_set, current_tile);
					break;
				}
				float dx = current_tile.x - previous_tile.x;
				float dy = current_tile.y - previous_tile.y;

				int steps = std::max(abs(dx), abs(dy));

				for (int i = 0; i <= steps; i++)
				{
					float t = (float)i / steps;
					int x = previous_tile.x + dx * t;
					int y = previous_tile.y + dy * t;
					paint_eraser_tiles_helper(tile_set, { x, y });
				}
				if (steps == 0)
				{
					paint_eraser_tiles_helper(tile_set, current_tile);
				}
			}
			break;
		}
		case ag::TileMap_Paint_Settings::Line:
		{
			if (dragging)
			{
				float dx = current_tile.x - start_tile.x;
				float dy = current_tile.y - start_tile.y;

				int steps = std::max(abs(dx), abs(dy));

				for (int i = 0; i <= steps; i++)
				{
					float t = (float)i / steps;
					int x = start_tile.x + dx * t;
					int y = start_tile.y + dy * t;
					paint_eraser_tiles_helper({ x, y });
				}
				if (steps == 0)
				{
					paint_eraser_tiles_helper(current_tile);
				}
			}
			break;
		}
		case ag::TileMap_Paint_Settings::Rectangle:
		{
			if (dragging)
			{
				int x_max = std::max(start_tile.x, current_tile.x);
				int y_max = std::max(start_tile.y, current_tile.y);

				int x_min = std::min(start_tile.x, current_tile.x);
				int y_min = std::min(start_tile.y, current_tile.y);

				int width = x_max - x_min;
				int height = y_max - y_min;

				for (int x = x_min; x <= x_max; x++)
				{
					paint_eraser_tiles_helper({ x, y_min });
				}
				for (int x = x_min; x <= x_max; x++)
				{
					paint_eraser_tiles_helper({ x, y_min + height });
				}
				for (int y = y_min; y <= y_max; y++)
				{
					paint_eraser_tiles_helper({ x_min, y });
				}
				for (int y = y_min; y <= y_max; y++)
				{
					paint_eraser_tiles_helper({ x_min + width, y });
				}

			}
			break;
		}
		case ag::TileMap_Paint_Settings::Fill:
		{
			if (dragging)
			{
				int x_max = std::max(start_tile.x, current_tile.x);
				int y_max = std::max(start_tile.y, current_tile.y);

				int x_min = std::min(start_tile.x, current_tile.x);
				int y_min = std::min(start_tile.y, current_tile.y);

				for (int x = x_min; x <= x_max; x++)
				{
					for (int y = y_min; y <= y_max; y++)
					{
						paint_eraser_tiles_helper({ x, y });
					}
				}
			}
			break;
		}
		default:
		{
			break;
		}
		}
		previous_tile = current_tile;
	}
	void ScenePanel::paint_eraser_tiles_helper(const vec2i& pos)
	{
		switch (m_settings)
		{
		case ag::TileMap_Settings::Paint:
		{
			temp_tiles[pos] = m_tile_id;
			return;
		}
		case ag::TileMap_Settings::Eraser:
		{
			temp_tiles[pos] = m_tile_id;
			return;
		}
		default:
			return;
		}
	}
	void ScenePanel::paint_eraser_tiles_helper(TileSet_Component& tile_set, const vec2i& pos)
	{
		switch (m_settings)
		{
		case ag::TileMap_Settings::Paint:
		{
			paint_tiles(tile_set, pos);
			m_scene->set_save_required();
			return;
		}
		case ag::TileMap_Settings::Eraser:
		{
			erase_tiles(tile_set, pos);
			m_scene->set_save_required();
			return;
		}
		default:
			return;
		}
	}
	void ScenePanel::paint_tiles(TileSet_Component& tile_set, const vec2i& pos)
	{
		if (!m_use_auto_tile)
		{
			Tile tile{};
			tile.tile_id = m_tile_id;
			tile_set.placed_tiles[pos] = tile;
			return;
		}

		auto& comps = m_selected_entity.get_component<AutoTiling_Component>();
		auto it = comps.auto_tiles.find(m_active_set);
		if (it == comps.auto_tiles.end())
			return;

		Tile& placed = tile_set.placed_tiles[pos];
		placed.set_id = it->second.set_id;
		placed.use_autotile = true;

		auto def = it->second.tile_bitmask.find(0);
		placed.tile_id = (def != it->second.tile_bitmask.end())
			? def->second
			: it->second.tile_bitmask.begin()->second;

		uint16_t raw = calculate_bitmask(tile_set, pos);
		uint16_t final_mask = resolve_mask(it->second.tile_bitmask, raw);
		auto id = it->second.tile_bitmask.find(final_mask);
		if (id != it->second.tile_bitmask.end())
		{
			if (placed.tile_id != id->second)
				placed.tile_id = id->second;
		}

		update_neighbour(tile_set, pos);
	}

	void ScenePanel::erase_tiles(TileSet_Component& tile_set, const vec2i& pos)
	{
		auto it = tile_set.placed_tiles.find(pos);
		if (it == tile_set.placed_tiles.end())
			return;


		bool was_autotile = it->second.use_autotile;
		auto set_id = it->second.set_id;

		tile_set.placed_tiles.erase(it);


		if (was_autotile)
		{
			for (int dx = -1; dx <= 1; dx++)
			{
				for (int dy = -1; dy <= 1; dy++)
				{
					if (dx == 0 && dy == 0) continue;

					vec2i neighbour_pos = { pos.x + dx, pos.y + dy };

					auto tile_it = tile_set.placed_tiles.find(neighbour_pos);
					if (tile_it == tile_set.placed_tiles.end())
						continue;

					Tile& tile = tile_it->second;
					if (tile.set_id == set_id && tile.use_autotile)
					{
						update_neighbour(tile_set, neighbour_pos);
					}
				}
			}
		}
	}

	uint16_t ScenePanel::get_set_id(const std::string& set_name)
	{
		if (m_selected_entity.has_component<AutoTiling_Component>())
		{
			auto& props = m_selected_entity.get_component<AutoTiling_Component>();

			auto it = props.auto_tiles.find(set_name);
			if (it != props.auto_tiles.end())
			{
				return it->second.set_id;
			}
		}
		return std::numeric_limits<uint16_t>::max();
	}

	uint16_t ScenePanel::normalize_autotile_mask(uint16_t mask)
	{
		const bool t = mask & T;
		const bool l = mask & L;
		const bool r = mask & R;
		const bool b = mask & B;

		// TL requires T + L
		if (!(t && l)) mask &= ~TL;

		// TR requires T + R
		if (!(t && r)) mask &= ~TR;

		// BL requires B + L
		if (!(b && l)) mask &= ~BL;

		// BR requires B + R
		if (!(b && r)) mask &= ~BR;

		return mask;
	}


	uint16_t ScenePanel::calculate_bitmask(TileSet_Component& tile_set, const vec2i& pos)
	{
		uint16_t mask = 0;
		auto active_set_id = get_set_id(m_active_set);

		if (active_set_id == std::numeric_limits<uint16_t>::max())
			return mask;

		auto check = [&](int dx, int dy, uint16_t bit) {
			vec2i neighbor_pos = { pos.x + dx, pos.y + dy };

			if (neighbor_pos.x == pos.x && neighbor_pos.y == pos.y)
			{
				mask |= bit;
				return;
			}

			auto it = tile_set.placed_tiles.find(neighbor_pos);
			if (it == tile_set.placed_tiles.end())
				return;

			const Tile& neighbor = it->second;
			if (neighbor.set_id == active_set_id && neighbor.use_autotile)
			{
				mask |= bit;
				return;
			}
			};

		// Check direct neighbors (4-way)
		check(0, -1, T);
		check(-1, 0, L);
		check(0, 0, M);
		check(1, 0, R);
		check(0, 1, B);


		if (mask & T && mask & L) check(-1, -1, TL);
		if (mask & T && mask & R) check(1, -1, TR);
		if (mask & B && mask & L) check(-1, 1, BL);
		if (mask & B && mask & R) check(1, 1, BR);


		return normalize_autotile_mask(mask);
	}

	uint16_t ScenePanel::remove_lowest_priority_bit(uint16_t mask)
	{
		static constexpr uint16_t priority[] = {
				TL, TR, BL, BR,
				T, L, R, B
		};
		for (uint16_t bit : priority)
		{
			if (mask & bit)
				return mask & ~bit;
		}

		return mask;
	}


	uint16_t ScenePanel::resolve_mask(const std::unordered_map<uint16_t, vec2u>& table, uint16_t mask)
	{
		while (!table.contains(mask))
		{
			uint16_t reduced = remove_lowest_priority_bit(mask);
			if (reduced == mask)
				break;
			mask = reduced;
		}
		return mask;
	}


	void ScenePanel::draw_tilemap_ghosts()
	{
		{
			if (has_selected_entity())
			{
				auto type = NodeHelper::get_nodetype(m_selected_entity);
				if (type == NodeType::TileMap && m_selected_entity.has_component<Texture_Component>() &&
					m_selected_entity.has_component<TileSet_Component>() && m_selected_entity.has_component<Tile_Component>())
				{
					auto& tile_set = m_selected_entity.get_component<TileSet_Component>();
					auto& props = m_selected_entity.get_component<Tile_Component>();

					if (!m_use_auto_tile)
					{
						Transform_Component trans;
						Sprite sprite;
						sprite.size = props.size;
						sprite.fill_color.a = 150;
						Renderer2D::set_texture(m_selected_entity.get_component<Texture_Component>().texture);
						{
							for (const auto& [position, id] : temp_tiles)
							{
								auto tex_it = tile_set.tile_definitions.find(id);
								if (tex_it == tile_set.tile_definitions.end())
								{
									continue;
								}
								const Tile_Defination& def = tex_it->second;
								sprite.texture_rect = def.texture_rect;

								trans.position = (position * props.size) + props.size / 2 + props.offset;

								Renderer2D::draw_sprite(sprite, trans);
							}
						}


						auto def = tile_set.tile_definitions.find(m_tile_id);
						if (def != tile_set.tile_definitions.end())
						{
							auto texture_rect = def->second.texture_rect;
							sprite.texture_rect = texture_rect;
							Transform_Component trans;
							vec2f current_mouse = EditorLayer::get().get_viewport_mouse_position();

							vec2i current_tile = {
								(int)std::floor((current_mouse.x - props.offset.x) / props.size.x),
								(int)std::floor((current_mouse.y - props.offset.y) / props.size.y)
							};

							trans.position = (current_tile * props.size) + props.size / 2 + props.offset;

							Renderer2D::draw_sprite(sprite, trans);
						}
					}
					else
					{
						Transform_Component trans;
						Rectangle rect;
						rect.size = props.size;
						rect.fill_color.a = 200;
						Renderer2D::set_texture(m_selected_entity.get_component<Texture_Component>().texture);
						{
							for (const auto& [position, id] : temp_tiles)
							{
								auto tex_it = tile_set.tile_definitions.find(id);
								if (tex_it == tile_set.tile_definitions.end())
								{
									continue;
								}
								const Tile_Defination& def = tex_it->second;

								trans.position = (position * props.size) + props.size / 2 + props.offset;

								Renderer2D::draw_rectangle(rect, trans);
							}
						}


						auto def = tile_set.tile_definitions.find(m_tile_id);
						if (def != tile_set.tile_definitions.end())
						{
							Transform_Component trans;
							vec2f current_mouse = EditorLayer::get().get_viewport_mouse_position();

							vec2i current_tile = {
								(int)std::floor((current_mouse.x - props.offset.x) / props.size.x),
								(int)std::floor((current_mouse.y - props.offset.y) / props.size.y)
							};

							trans.position = (current_tile * props.size) + props.size / 2 + props.offset;

							Renderer2D::draw_rectangle(rect, trans);
						}
					}
				}
			}
		}

	}

	void ScenePanel::update_neighbour(TileSet_Component& tile_set, const vec2i& start)
	{
		auto set_id = get_set_id(m_active_set);
		if (set_id == std::numeric_limits<uint16_t>::max()) return;

		auto& comps = m_selected_entity.get_component<AutoTiling_Component>();
		auto auto_it = comps.auto_tiles.find(m_active_set);
		if (auto_it == comps.auto_tiles.end()) return;

		auto& table = auto_it->second.tile_bitmask;

		std::queue<vec2i> q;
		std::unordered_set<vec2i, vec2_hash<int>> visited;

		q.push(start);
		for (int dx = -1; dx <= 1; dx++)
			for (int dy = -1; dy <= 1; dy++)
				if (dx || dy)
					q.push({ start.x + dx, start.y + dy });

		while (!q.empty())
		{
			vec2i pos = q.front();
			q.pop();

			if (visited.contains(pos))
				continue;
			visited.insert(pos);

			auto self_it = tile_set.placed_tiles.find(pos);
			if (self_it == tile_set.placed_tiles.end())
				continue;

			Tile& self = self_it->second;
			if (self.set_id != set_id || !self.use_autotile)
				continue;

			uint16_t raw = calculate_bitmask(tile_set, pos);
			uint16_t resolved = resolve_mask(table, raw);

			auto id_it = table.find(resolved);
			if (id_it != table.end() && self.tile_id != id_it->second)
			{
				self.tile_id = id_it->second;

				for (int dx = -1; dx <= 1; dx++)
					for (int dy = -1; dy <= 1; dy++)
						if (dx || dy)
							q.push({ pos.x + dx, pos.y + dy });
			}
		}
	}
}
