#include <Panels/ScenePanel.hpp>
#include <Application/EditorLayer.hpp>
#include <Node/NodeProperties.hpp>
#include <UI/UI.hpp>

namespace ag
{
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
		//NodeFactory::properties_map[NodeType::TextNode] = TextNode::show_properties;
		//NodeFactory::properties_map[NodeType::Button] = ButtonNode::show_properties;
		//NodeFactory::properties_map[NodeType::TextureButton] = TextureButton::show_properties;

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
		UI::draw_menu_bar();
		UI::content_browser();

		/*ImGui::Begin("Scene");
		draw_scene_top_panel();
		ImGui::Spacing();

		auto view = m_scene->m_registry.view<Tag_Component>();
		for (auto entityID : view)
		{
			Entity entity(entityID);
			auto& tag = entity.get_component<Tag_Component>();
			if (tag.parent.get_id() == INVALID_ENTITY)
			{
				draw_node_hierarchy(entity, 0);
				ImGui::Spacing();

			}
		}
		ImGui::End();*/
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
					if (UI::draw_tilemap_selector(m_selected_entity, m_tile_id))
					{
						m_tile_id = tile_id;
						AERO_CORE_INFO("True");
						m_tile_id.print();
					}
					//TileMapNodeFeatures::texture_selector_gui(props.texture, m_texture_rect);
					//TileMapNodeFeatures::register_tile(m_selected_entity);

					/*m_is_texture_selected = (texture_selector(props.texture, props.size, m_texture_rect));
					if (m_is_texture_selected)
					{
						auto& props = m_selected_entity.get_component<TileMapNode::TileMapProp>();
						props.ghost_sprite.texture_rect = m_texture_rect;
						props.display_ghost = true;
					}*/
				}
			}
			else if (tag.node_type == NodeType::Sprite)
			{
				if (m_selected_entity.has_component<TextureRect_Component>())
				{
					auto& rects = m_selected_entity.get_component<TextureRect_Component>();
					auto& sizes = m_selected_entity.get_component<Render2D_Component>();
					auto& texture = m_selected_entity.get_component<Texture_Component>().texture;

					uint_rect texture_rect;
					if (texture)
					{
						if (UI::texture_selector(m_selected_entity, texture_rect))
						{
							rects.rect = texture_rect;
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

		// Footer with selected object info and create button
		ImGui::BeginChild("FooterArea", ImVec2(0, 80), true);
		{
			auto it = NodeFactory::nodes.find(state.selected_prefab);

			if (it != NodeFactory::nodes.end()) {
				ImGui::Columns(2, "##footer_columns", false);
				ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() - 120);

				ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "Selected:");
				ImGui::SameLine();
				ImGui::Text("%s", it->second.c_str());

				// Right: Create button
				ImGui::NextColumn();

				bool can_create = NodeFactory::create_map.find(state.selected_prefab) !=
					NodeFactory::create_map.end();

				ImGui::BeginDisabled(!can_create);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));

				if (ImGui::Button("CREATE", ImVec2(100, 0))) {
					create_selected_object(state.selected_prefab);

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
			m_selected_entity = entity;
		}

		// Double click - focus entity in viewport
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
		}

		if (ImGui::MenuItem("Duplicate", "Ctrl+D"))
		{
			duplicate_entity();
		}

		if (ImGui::MenuItem("Delete", "Del"))
		{
			delete_entity();
		}

		if (ImGui::MenuItem("Make Root", "Ctrl+Shift+R"))
		{
			make_root_entity();
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("Create Child")) {

			if (ImGui::MenuItem("Empty")) {
				//create_child_entity(entity, "Empty", NodeType::Empty);
			}
			if (ImGui::MenuItem("Cube")) {
				//create_child_entity(entity, "Cube", NodeType::Cube);
			}
			if (ImGui::MenuItem("Light")) {
				//create_child_entity(entity, "Light", NodeType::PointLight);
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
		if (entity.has_component<Tag_Component>()) {
			auto& vis = entity.get_component<Tag_Component>();
			if (ImGui::MenuItem(vis.visible ? "Hide" : "Show")) {
				vis.visible = !vis.visible;
			}
		}

		if (entity.has_component<Tag_Component>()) {
			auto& lock = entity.get_component<Tag_Component>();
			if (ImGui::MenuItem(lock.locked ? "Unlock" : "Lock")) {
				lock.locked = !lock.locked;
			}
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Properties", "Alt+Enter")) {
			// Show properties panel
		}
	}

	void ScenePanel::draw_hierarchy_context_menu(Entity* entity) {
		if (ImGui::MenuItem("Create Empty")) {
			/*Entity new_entity = m_scene->create_entity("Empty", NodeType::Empty);
			m_selected_entity = new_entity;*/
		}

		if (ImGui::MenuItem("Create From Prefab...")) {
			// Open prefab browser
		}

		ImGui::Separator();

		//has_clipboard_entity()
		if (ImGui::MenuItem("Paste", "Ctrl+V", false, false)) {
			//paste_entity();
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

		if (tooltip && ImGui::IsItemHovered()) {
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
			entity.get_component<Tag_Component>().locked) {
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
		case NodeType::TextNode: return "ICON_FA_FONT";
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
		case NodeType::TextNode:
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

	//void ScenePanel::draw_create_object()
	//{
	//	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
	//	ImGui::Begin("Create Objects", &m_show_create_panel, window_flags);
	//	static NodeType selectedPrefab = NodeType::Rectangle;
	//	NodeType newSelected = selectedPrefab;

	//	for (auto& [type, name] : NodeFactory::nodes)
	//	{
	//		if (ImGui::Selectable(name.c_str(), selectedPrefab == type))
	//			newSelected = type;
	//		ImGui::Spacing();
	//		ImGui::Spacing();
	//	}
	//	selectedPrefab = newSelected;
	//	auto it = NodeFactory::nodes.find(selectedPrefab);
	//	if (it != NodeFactory::nodes.end() && NodeFactory::create_map.find(selectedPrefab) != NodeFactory::create_map.end())
	//	{
	//		if (ImGui::Button("Create Entity"))
	//		{
	//			Entity newEntity = m_scene->create_entity(it->second, selectedPrefab);

	//			if (m_selected_entity)
	//			{
	//				auto& tag = m_selected_entity.get_component<Tag_Component>();
	//				auto& new_e_tag = newEntity.get_component<Tag_Component>();

	//				new_e_tag.parent = m_selected_entity;
	//				tag.children.push_back(newEntity);
	//			}

	//			m_selected_entity = newEntity;
	//			m_show_create_panel = false;
	//		}
	//	}
	//	ImGui::End();
	//}

	void ScenePanel::draw_selected_text()
	{
		if (m_selected_entity.get_id() == INVALID_ENTITY)
			return;

		if (m_selected_entity.has_component<Transform_Component>())
		{
			auto& name = m_selected_entity.get_component<Tag_Component>().name;
			auto trans = Transform_Component::get_world_transform(m_selected_entity);
			Transform_Component transform;
			transform.position = Math::world_to_screen(trans.position, EditorLayer::get().get_float_rect(), EditorLayer::get().get_viewport_size());
			Text text;
			text.text = name;
			text.text_color = Color(220, 220, 220);
			text.font_size = 18;
			text.mode = RenderMode::Screen;
			Renderer2D::draw_text(text, transform);
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
				Renderer2D::draw_rectangle(rect, transform);
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
				Renderer2D::draw_circle(circle, transform);
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
				Renderer2D::draw_rectangle(rect, transform);
				break;
			}
			}

		}
	}

	void ScenePanel::update_transform_settings()
	{
		if (!m_selected_entity || !m_selected_entity.has_component<Transform_Component>())
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
				m_initial_transform.scale = m_selected_entity.get_component<Transform_Component>().scale;
				reset_transform_setting();
			}
			else if (Mouse::is_mouse_pressed(Button::ButtonRight))
			{
				auto& scale = m_selected_entity.get_component<Transform_Component>().scale;
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
				m_initial_transform.rotation = m_selected_entity.get_component<Transform_Component>().rotation;
				reset_transform_setting();
			}
			else if (Mouse::is_mouse_pressed(Button::ButtonRight))
			{
				auto& rotation = m_selected_entity.get_component<Transform_Component>().rotation;
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
				m_initial_transform.position = m_selected_entity.get_component<Transform_Component>().position;
				reset_transform_setting();
			}
			else if (Mouse::is_mouse_pressed(Button::ButtonRight))
			{
				auto& position = m_selected_entity.get_component<Transform_Component>().position;
				position = m_initial_transform.position;
				reset_transform_setting();
			}
			return;
		}
		default: AERO_CORE_ERROR("No sucn Transform_Component Setting!"); break;
		}
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

			if (!m_selected_entity.has_component<Script_Component>())
			{
				Script_Component comp;
				comp.path = script_path;
				m_selected_entity.add_component<Script_Component>(comp);
			}
			else
			{
				auto& comp = m_selected_entity.get_component<Script_Component>();
				comp.path = script_path;
			}
		}
	}


	void ScenePanel::duplicate_entity()
	{
		if (!m_selected_entity || m_selected_entity.get_id() == INVALID_ENTITY)
			return;

		auto& parent = m_selected_entity.get_component<Tag_Component>().parent;
		auto new_entity = m_scene->duplicate_entity(m_selected_entity, parent);

		m_selected_entity = new_entity;
	}
	void ScenePanel::delete_entity()
	{
		m_scene->destroy_entity(m_selected_entity);
		m_selected_entity = Entity();
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
			// Duplicate Entity
			if (e.get_key_code() == Key::D)
			{
				duplicate_entity();
			}

			// Make Root Entity
			if (e.get_key_code() == Key::R && shift)
			{
				make_root_entity();
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
			delete_entity();
			break;
		}

		}
		return false;
	}

	bool ScenePanel::on_mouse_pressed(MouseButtonPressedEvent& e)
	{
		if (!m_selected_entity)
			return false;

		if (m_selected_entity.has_component<Tag_Component>() && EditorLayer::get().is_viewport_hovered())
		{
			const auto& tag = m_selected_entity.get_component<Tag_Component>();
			if (tag.node_type == NodeType::TileMap)
			{
				if (!m_selected_entity.has_component<Tile_Component>() || !m_selected_entity.has_component<TileSet_Component>())
					return false;
				auto& props = m_selected_entity.get_component<Tile_Component>();
				auto& tile_set = m_selected_entity.get_component<TileSet_Component>();

				vec2f mouse_position = EditorLayer::get().get_viewport_mouse_position();
				vec2i tile_pos = {
					(int)std::floor((mouse_position.x - props.offset.x) / props.size.x),
					(int)std::floor((mouse_position.y - props.offset.y) / props.size.y)
				};

				if (e.get_mouse_button() == Button::ButtonLeft)
				{
					tile_set.placed_tiles[tile_pos] = m_tile_id;
				}
				else if (e.get_mouse_button() == Button::ButtonRight)
				{
					if (tile_set.placed_tiles.contains(tile_pos))
					{
						auto it = tile_set.placed_tiles.find(tile_pos);
						if (it != tile_set.placed_tiles.end())
						{
							tile_set.placed_tiles.erase(it);
						}
					}
				}
			}
		}


		return false;
	}
}
