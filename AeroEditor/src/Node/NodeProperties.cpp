#include <Node/NodeProperties.hpp>
#include <Aero.hpp>
#include <UI/UI.hpp>


namespace ag
{
#define REGISTER_COMPONENT(T) \
	comps.push_back({T::get_name(), &T::is_compatible, [](Entity e) { add<T>(e); }, [](Entity e){ T::imgui_render(e); } })


	inline static ImVec2 spacing = ImVec2(0, 3);

	void NodeProperties::register_components()
	{
		comps.clear();
		REGISTER_COMPONENT(Border_Component);
		REGISTER_COMPONENT(Corner_Component);

		REGISTER_COMPONENT(TextureRect_Component);
		REGISTER_COMPONENT(TextureFlip_Component);

		REGISTER_COMPONENT(SolidSet_Component);
		REGISTER_COMPONENT(TileSet_Component);
		REGISTER_COMPONENT(AutoTiling_Component);

		REGISTER_COMPONENT(Window_Component);
		REGISTER_COMPONENT(CameraBounds_Component);
		REGISTER_COMPONENT(CameraFollow_Component);


		REGISTER_COMPONENT(Tween_Component);




		REGISTER_COMPONENT(FontStyle_Component);
		REGISTER_COMPONENT(UI_Component);

		REGISTER_COMPONENT(PhysicsBody_Component);
		REGISTER_COMPONENT(Script_Component);

	}

	void NodeProperties::draw_added_components(Entity entity)
	{
		for (auto& info : comps)
		{
			if (!info.is_compatible(NodeHelper::get_nodetype(entity)))
				continue;

			info.imgui_render(entity);
		}
	}

	void NodeProperties::add_component(Entity entity)
	{
		ImGui::Dummy(spacing);

		ImGui::Separator();
		ImGui::Spacing();

		ImVec2 available_size = ImGui::GetContentRegionAvail();
		float button_width = 200.0f;
		float button_x = (available_size.x - button_width) * 0.5f;

		ImGui::SetCursorPosX(button_x);

		if (ImGui::Button( ICON_FA_PLUS "  Add Component", ImVec2(button_width, 35)))
		{
			ImGui::OpenPopup("AddComponentModal");
		}
		if (ImGui::BeginPopup("AddComponentModal"))
		{
			for (auto& info : comps)
			{
				if (!info.is_compatible(NodeHelper::get_nodetype(entity)))
					continue;
				ImGui::Spacing();

				if (ImGui::MenuItem(info.name))
				{
					info.create(entity);
					ImGui::CloseCurrentPopup();
				}
				ImGui::Spacing();
				ImGui::Separator();
			}

			ImGui::EndPopup();
		}

		ImGui::Spacing();
		ImGui::Separator();

		ImGui::Dummy(spacing);
	}

	void Border_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Border_Component>("Border Component", entity,
			[](Border_Component& props)
			{
				UI::draw_value("Thickness", props.thickness);
				UI::draw_color("Color", props.color);
			}, true);
	}
	void Corner_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Corner_Component>("Corner Component", entity,
			[entity](Corner_Component& props) mutable
			{
				UI::draw_bool("Uniform", props.uniform);

				float max = 100.0f;
				if (entity.has_component<Render2D_Component>())
				{
					auto& size = entity.get_component<Render2D_Component>().size;
					max = std::min(size.x, size.y);
					max *= 0.5f;
				}

				UI::draw_value("Corner", props.corner, 0.0f, max);
			}, true);
	}
	void UI_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<UI_Component>("UI Component", entity,
			[](UI_Component& props)
			{
				std::vector<std::string> options;
				options.push_back("Screen");
				options.push_back("World");
				UI::draw_enum("Mode", props.mode, options);
			}, true);
	}
	void Render2D_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Render2D_Component >("Render2D Component", entity,
			[entity](Render2D_Component& props) mutable
			{
				vec2f reset_value;
				{
					if (entity.has_component<Texture_Component>() && entity.get_component<Texture_Component>().texture)
					{
						reset_value = entity.get_component<Texture_Component>().texture->get_size();
					}

					if (entity.has_component<TextureRect_Component>())
					{
						reset_value = entity.get_component<TextureRect_Component>().rect.size;
					}
				}
				UI::draw_vec2("Size", props.size, reset_value);
				UI::draw_color("Color", props.color);
			}, false);
	}
	void Texture_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Texture_Component>("Texture Component", entity,
			[entity](Texture_Component& props)
			{
				UI::draw_texture(entity);
			}, false);
	}
	void TextureRect_Component::imgui_render(Entity entity, bool can_remove)
	{
		NodeProperties::draw_component_node<TextureRect_Component>("TextureRect Component", entity,
			[entity](TextureRect_Component& props) mutable
			{
				bool changed = false;
				changed = UI::draw_vec2("Position", props.rect.position);

				vec2u reset_value;
				if (entity.has_component<Texture_Component>())
				{
					const auto& texture = entity.get_component<Texture_Component>();
					if (texture.texture)
					{
						reset_value = texture.texture->get_size();
					}
					if ((props.rect.size.x == 0 || props.rect.size.y == 0) && texture.texture)
					{
						props.rect.size = texture.texture->get_size();
					}
				}
				changed = UI::draw_vec2("Size", props.rect.size, reset_value);
			}, can_remove);
	}
	void TextureFlip_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<TextureFlip_Component>("TextureFlip Component", entity,
			[entity](TextureFlip_Component& props) mutable
			{
				UI::draw_bool("Horizontal", props.horizontal);
				UI::draw_bool("Vertical", props.vertical);

			}, true);
	}
	void Transform_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Transform_Component>("Transform Component", entity,
			[](Transform_Component& transform)
			{
				UI::draw_vec2("Position", transform.position, { 0, 0 });
				UI::draw_vec2("Scale", transform.scale, { 1.0f, 1.0f });
				UI::draw_value("Rotation", transform.rotation, 0.0f, 360.0f);
			}, false);
	}
	void Tag_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Tag_Component>("Tag Component", entity,
			[](Tag_Component& tag)
			{
				UI::draw_string("Name", tag.name);
				UI::draw_bool("Visible", tag.visible);
				UI::draw_bool("Lock", tag.locked);
			}, false);
	}
	void Script_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Script_Component>("Scirpt Component", entity,
			[entity](Script_Component& props) mutable
			{
				UI::draw_script_selector(entity);
			}, true);
	}
	
	
	void Camera_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Camera_Component>("Camera Component", entity,
			[](Camera_Component& props)
			{
				UI::draw_vec2("Size", props.size, { 1280, 720 });
				UI::draw_vec2("Center", props.center, props.size / 2);
			}, false);
	}
	void Window_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Window_Component>("Window Component", entity,
			[](Window_Component& props)
			{
				UI::draw_vec2("Size", props.size, { 1280, 720 });
			}, true);
	}
	void CameraBounds_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<CameraBounds_Component>("Camera Bounds Component", entity,
			[](CameraBounds_Component& props)
			{
				float max = std::numeric_limits<float>::max();
				float min = std::numeric_limits<float>::min();
				UI::draw_vec2("X Axis", props.x_axis, { min, max });
				UI::draw_vec2("Y Axis", props.y_axis, { min, max });

			}, true);
	}
	void CameraFollow_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<CameraFollow_Component>("Camera Follow Component", entity,
			[](CameraFollow_Component& props)
			{
				std::vector<std::string> type;
				type.push_back("None");
				type.push_back("Lock on Target");
				type.push_back("Lerp Smooth");
				type.push_back("Spring");
				type.push_back("Dead Zone");
				UI::draw_enum("Follow Type", props.type, type);

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				ImGui::Text("Target Entity:");
				std::string entity_name = "";
				if (props.target)
				{
					entity_name = props.target.get_component<Tag_Component>().name;
				}
				GUI_Button button;
				button.label = entity_name;
				button.size = ImVec2(ImGui::GetContentRegionAvail().x, 35);
				if (!entity_name.empty())
				{
					UI::draw_button(button);
				}
				else
				{
					button.label = "Drop Target Entity Here";
					UI::draw_button(button);
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_NODE"))
					{
						AG_uint dragged_id = *(const AG_uint*)payload->Data;
						Entity e(dragged_id);
						props.target = e;
					}
					ImGui::EndDragDropTarget();
				}

				// Show parameters based on selected follow type
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				switch (props.type)
				{
				case FollowType::NONE:
				{
					ImGui::TextWrapped("No camera following. Camera will remain static or controlled manually.");
					break;
				}

				case FollowType::LOCK_ON_TARGET:
				{
					ImGui::TextWrapped("Camera instantly locks onto target position with no smoothing.");
					break;
				}

				case FollowType::LERP_SMOOTH:
				{
					ImGui::Text("Lerp Smooth Settings:");
					ImGui::Spacing();

					ImGui::PushItemWidth(-1);
					ImGui::DragFloat("##LerpSpeed", &props.lerp_speed, 0.1f, 0.1f, 50.0f, "Speed: %.1f");
					ImGui::PopItemWidth();

					ImGui::TextWrapped("Speed: How fast the camera follows the target. Higher values = faster following.");
					if (ImGui::Button("Reset to Default##Lerp", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
					{
						props.lerp_speed = 5.0f;
					}
					break;
				}

				case FollowType::SPRING:
				{
					ImGui::Text("Spring Settings:");
					ImGui::Spacing();

					ImGui::PushItemWidth(-1);
					ImGui::DragFloat("##SpringStiffness", &props.spring_stiffness, 1.0f, 1.0f, 500.0f, "Stiffness: %.1f");
					ImGui::PopItemWidth();
					ImGui::TextWrapped("Stiffness: Spring strength. Higher values = tighter following.");

					ImGui::Spacing();

					ImGui::PushItemWidth(-1);
					ImGui::DragFloat("##SpringDamping", &props.spring_damping, 0.5f, 0.1f, 100.0f, "Damping: %.1f");
					ImGui::PopItemWidth();
					ImGui::TextWrapped("Damping: Reduces oscillation/bounce. Higher values = less bouncy.");

					if (ImGui::Button("Reset to Default##Spring", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
					{
						props.spring_stiffness = 150.0f;
						props.spring_damping = 20.0f;
						props.velocity = vec2f(0.0f, 0.0f);
					}
					break;
				}

				case FollowType::DEAD_ZONE:
				{
					ImGui::Text("Dead Zone Settings:");
					ImGui::Spacing();

					ImGui::PushItemWidth(-1);
					ImGui::DragFloat2("##DeadZone", &props.dead_zone.x, 1.0f, 0.0f, 1000.0f, "Size: %.0f px");
					ImGui::PopItemWidth();
					ImGui::TextWrapped("Dead Zone Size: Camera only moves when target leaves this rectangle.");

					ImGui::Spacing();

					ImGui::PushItemWidth(-1);
					ImGui::DragFloat("##DeadZoneSpeed", &props.lerp_speed, 0.1f, 0.1f, 50.0f, "Speed: %.1f");
					ImGui::PopItemWidth();
					ImGui::TextWrapped("Speed: How fast camera moves when target leaves dead zone.");

					if (ImGui::Button("Reset to Default##DeadZone", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
					{
						props.dead_zone = vec2f(100.0f, 100.0f);
						props.lerp_speed = 10.0f;
					}
					break;
				}
				}
			}, true);
	}
	
	void Animation_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Animation_Component>("Animation Component", entity,
			[entity](Animation_Component& anim)
			{
				UI::draw_animation(entity);
			}, false);
	}
	void Tween_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Tween_Component>("Tween Component", entity,
			[entity](Tween_Component& tween) mutable
			{
				std::vector<std::string> option;

				option = { "STOPPED", "PLAYING", "PAUSED", "COMPLETED" };

				UI::draw_enum("State", tween.state, option);

				option.clear();
				option = {"LINEAR", "EASE_IN", "EASE_OUT", "EASE_IN_OUT" };
				UI::draw_enum("Ease Type", tween.ease_type, option);

				option.clear();
				option = { "ONCE", "LOOP", "PING_PONG" };
				UI::draw_enum("Loop Type", tween.loop_type, option);
				

				vec2f reset_value;
				if (entity.has_component<Transform_Component>())
				{
					reset_value = Transform_Component::get_world_transform(entity).position;
				}
				UI::draw_vec2("Start", tween.start_position, reset_value);
				UI::draw_vec2("End", tween.end_position, reset_value);

				UI::draw_value("Duration", tween.duration, 0.01f, 100.0f);

				GUI_Button button;
				ImGui::BeginGroup();

				button.size.x = (ImGui::GetContentRegionAvail().x - 10) / 3.0f;
				button.size.y = 35.0f;

				button.label = ICON_FA_PLAY"  Play";
				if (UI::draw_button(button))
				{
					Tween_Component::play_tween(entity);
				}
				ImGui::SameLine(0, 5.0f);

				button.label = ICON_FA_PAUSE"  Pause";
				button.enabled = tween.state == State::PLAYING;
				if (UI::draw_button(button))
				{
					tween.state = State::PAUSED;
				}

				ImGui::SameLine(0, 5.0f);

				button.label = ICON_FA_STOP"  Stop";
				button.enabled = tween.state == State::PLAYING || tween.state == State::PAUSED;
				if (UI::draw_button(button))
				{
					tween.state = State::STOPPED;
					tween.elapsed_time = 0.0f;
					tween.reverse_direction = false;
				}
				ImGui::EndGroup();

			}, true);
	}
	
	void SolidSet_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<SolidSet_Component>("SolidSet Component", entity,
			[entity](SolidSet_Component& tile)
			{
				ImGui::Text("%d", tile.placed_tiles.size());
			}, true);
	}
	void Tile_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Tile_Component>("Tile Component", entity,
			[entity](Tile_Component& tile)
			{
				UI::draw_vec2("Size", tile.size, { 32, 32 });
				UI::draw_vec2("Offset", tile.offset, { 0, 0 });
			}, false);
	}
	void TileSet_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<TileSet_Component>("TileSet Component", entity,
			[entity](TileSet_Component& tile)
			{
				UI::draw_tilemap_register(entity);
			}, true);
	}
	
	
	void PhysicsBody_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<PhysicsBody_Component>("PhysicsBody Component", entity,
			[](PhysicsBody_Component& props)
			{
				std::vector<std::string> options;
				options.push_back("Dynamic");
				options.push_back("Static");
				options.push_back("Kinematic");

				UI::draw_enum("Body Type", props.body_type, options);
				UI::draw_bool("Rotation", props.rotation);
			}, true);
		CollisionShape_Component::imgui_render(entity);
		PhysicsMaterial_Component::imgui_render(entity);
	}
	void CollisionShape_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<CollisionShape_Component>("CollisionShape Component", entity,
			[](CollisionShape_Component& props)
			{
				std::vector<std::string> options;
				options.push_back("Rectangle");
				options.push_back("Circle");

				UI::draw_enum("Shape Type", props.shape_type, options);
				switch (props.shape_type)
				{
				case ShapeType::Rectangle:
					UI::draw_vec2("Size", props.size);
					break;
				case ShapeType::Circle:
					UI::draw_value("Radius", props.radius);
					break;
				default:
					UI::draw_vec2("Size", props.size);
					break;
				}
				ImGui::SliderInt("Group Number", &props.group, 1, 5);
				ImGui::Text("Collides With:");
				for (int i = 0; i < 5; i++)
				{
					char label[16];
					sprintf(label, "Group %d", i + 1);
					ImGui::Checkbox(label, &props.collide_with[i]);
				}
			}, false);
	}
	void PhysicsMaterial_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<PhysicsMaterial_Component>("PhysicsMaterials Component", entity,
			[](PhysicsMaterial_Component& props)
			{
				std::vector<std::string> options;

				options.push_back("Custom");
				options.push_back("Wood");
				options.push_back("Metal");
				options.push_back("Rubber");
				options.push_back("Ice");
				options.push_back("Bouncy");
				options.push_back("Stone");

				if (UI::draw_enum("Material Type", props.preset, options))
					apply_preset(props);
				
				ImGui::PushItemWidth(-1);
				if (ImGui::SliderFloat("##Density", &props.density, 0.0f, 10.0f, "Density: %.2f"))
					props.preset = MaterialPreset::Custom;

				if (ImGui::SliderFloat("##Friction", &props.friction, 0.0f, 1.0f, "Friction: %.2f"))
					props.preset = MaterialPreset::Custom;

				if (ImGui::SliderFloat("##Restitution", &props.restitution, 0.0f, 1.5f, "Restitution: %.2f"))
					props.preset = MaterialPreset::Custom;

				ImGui::PopItemWidth();

			}, false);
	}
	
	void AutoTiling_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<AutoTiling_Component>("AutoTiling Component", entity,
			[entity](AutoTiling_Component& comp) mutable
			{
				UI::draw_autotiling_register(entity);
			}, true);
	}
	void Text_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Text_Component>("Text Component", entity,
			[entity](Text_Component& comp) mutable
			{
				UI::draw_string_multiline("Description", comp.text, 2048, ImVec2(0, 150));
				UI::draw_value("Size", comp.font_size, 10.0f);
			}, false);
	}
	void FontStyle_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<FontStyle_Component>("FontStyle Component", entity,
			[entity](FontStyle_Component& props) mutable
			{
				UI::draw_color("Text Color", props.color);
				std::vector<std::string> allignment;
				allignment.push_back("Left");
				allignment.push_back("Center");
				allignment.push_back("Right");
				UI::draw_enum("Horizontal", props.h_allignment, allignment);
				allignment.clear();
				allignment.push_back("Top");
				allignment.push_back("Center");
				allignment.push_back("Bottom");
				UI::draw_enum("Vertical", props.v_allignment, allignment);
				UI::draw_value("Line Height", props.line_height, 1.0f, 5.0f);
				UI::draw_vec2("Bounds", props.bounds);
			}, true);
	}
	void Button_Visual::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Button_Component>("Button_Visual", entity,
			[entity](Button_Component& comps) mutable
			{
				bool changed = false;
				changed = UI::draw_color("Background", comps.base.background);
				changed = UI::draw_color("Border", comps.base.border);
				changed = UI::draw_color("Text", comps.base.text);
				changed = UI::draw_value("Thickness", comps.base.border_thickness);
				float size = comps.layout.size.y;
				changed = UI::draw_value("Corner", comps.base.corner, 0.0f, size * 0.5f);
				if (changed)
				{
					comps.overrides[comps.current_state] = comps.base;
				}

			}, false);
	}
	void Button_Layout::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Button_Component>("Button_Layout", entity,
			[entity](Button_Component& comps) mutable
			{
				UI::draw_vec2("Size", comps.layout.size);
				std::vector<std::string> allignment;
				allignment.push_back("Left");
				allignment.push_back("Center");
				allignment.push_back("Right");
				UI::draw_enum("Horizontal", comps.layout.h_allignment, allignment);
				allignment.clear();
				allignment.push_back("Top");
				allignment.push_back("Center");
				allignment.push_back("Bottom");
				UI::draw_enum("Vertical", comps.layout.v_allignment, allignment);
				UI::draw_bool("Uniform", comps.layout.uniform);
			}, false);
	}
	void Button_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Button_Component>("Button Component", entity,
			[entity](Button_Component& comps) mutable
			{
				std::vector<std::string> visual_states = Button_Visual::all_states();
				bool exists = false;
				if (UI::draw_enum("State", comps.current_state, visual_states))
				{
					auto it = comps.overrides.find(comps.current_state);
					if (it != comps.overrides.end())
						comps.base = it->second;
				}

				auto it = comps.overrides.find(comps.current_state);
				if (it != comps.overrides.end())
					exists = true;
				else
					exists = false;


				float total_width = ImGui::GetContentRegionAvail().x;
				float button_spacing = 10.0f;
				GUI_Button button;
				button.size = { (total_width - button_spacing) * 0.5f , 35.0f };
				button.label = "Create New";
				button.enabled = !exists;
				if (UI::draw_button(button))
				{
					comps.overrides[comps.current_state] = comps.base;
				}

				ImGui::SameLine(0, button_spacing);

				button.label = "Delete";
				button.enabled = exists;
				if (UI::draw_button(button))
				{
					if (comps.current_state != Button_Visual_State::Normal)
					{
						comps.overrides.erase(comps.current_state);
						comps.current_state = Button_Visual_State::Normal;
					}
				}
			}, false);
	}
	void Textured_Button_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Textured_Button_Component>("Texture Button Component", entity,
			[entity](Textured_Button_Component& comps) mutable
			{
				std::vector<std::string> visual_states = Button_Visual::all_states();
				bool exists = false;
				if (UI::draw_enum("State", comps.current_state, visual_states))
				{
					auto it = comps.overrides.find(comps.current_state);
					if (it != comps.overrides.end())
					{
						comps.base_rect = it->second;
						if (entity.has_component<TextureRect_Component>())
						{
							auto& props = entity.get_component<TextureRect_Component>();
							props.rect = comps.base_rect;
						}
					}
				}

				auto it = comps.overrides.find(comps.current_state);
				if (it != comps.overrides.end())
					exists = true;
				else
					exists = false;


				float total_width = ImGui::GetContentRegionAvail().x;
				float button_spacing = 10.0f;

				GUI_Button button;

				button.size = { (total_width - button_spacing) * 0.5f , 35.0f };


				button.label = "Create New";
				button.enabled = !exists;
				if (UI::draw_button(button))
				{
					comps.overrides[comps.current_state] = comps.base_rect;
				}

				ImGui::SameLine(0, button_spacing);

				button.label = "Delete";
				button.enabled = exists;
				if (UI::draw_button(button))
				{
					if (comps.current_state != Button_Visual_State::Normal)
					{
						comps.overrides.erase(comps.current_state);
						comps.current_state = Button_Visual_State::Normal;
					}
				}
			}, false);
	}
	void Audio_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Audio_Component>("Audio Component", entity,
			[entity](Audio_Component& comps) mutable
			{
				if (!comps.path.empty())
				{
					ImGui::BeginGroup();

					std::filesystem::path p(comps.path);
					std::string filename = p.filename().string();
					std::string folder = p.parent_path().string();

					if (folder.length() > 40)
					{
						folder = "..." + folder.substr(folder.length() - 37);
					}

					ImGui::TextDisabled("Loaded:");
					ImGui::SameLine();
					ImGui::TextUnformatted(filename.c_str());

					ImGui::TextDisabled("Path:");
					ImGui::SameLine();
					ImGui::TextDisabled("%s", folder.c_str());

					ImGui::EndGroup();

					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::TextUnformatted(comps.path.c_str());
						ImGui::EndTooltip();
					}

					ImGui::Spacing();
				}

				ImGui::BeginGroup();

				GUI_Button button;

				if (comps.audio_buffer == 0)
				{
					button.label = "Load Sound";
				}
				else
				{
					button.label = "Change Sound";
				}

				button.size.x = (ImGui::GetContentRegionAvail().x - 8) * 0.5f;
				button.size.y = 35.0f;

				if (UI::draw_button(button))
				{
					auto full_path = FileDialogs::open_file("Audio Files\0*.mp3;*.wav;*.ogg;*.flac\0All Files\0*.*\0");
					if (!full_path.empty())
					{
						comps.path = full_path;
						comps.audio_buffer = NodeHelper::load_sound(comps.path);
						comps.source->set_buffer(comps.audio_buffer);
					}
				}

				ImGui::SameLine(0, 4.0f);
				button.label = "Unload";
				button.enabled = !comps.path.empty();
				if (UI::draw_button(button))
				{
					comps.audio_buffer = 0;
					comps.path.clear();
					comps.source->set_buffer(0);
				}

				ImGui::EndGroup();

				if (comps.audio_buffer != 0)
				{
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Playback");

					ImGui::BeginGroup();

					bool isPlaying = comps.source->is_playing();

					button.label = isPlaying ? "Pause" : "Play";

					button.size.x = (ImGui::GetContentRegionAvail().x - 10) / 3.0f;

					if (UI::draw_button(button))
					{
						if (isPlaying)
						{
							comps.source->pause();
						}
						else
						{
							comps.source->play();
						}
					}

					ImGui::SameLine(0, 5.0f);

					button.label = "Stop";
					if (UI::draw_button(button))
					{
						comps.source->stop();
					}

					ImGui::SameLine(0, 5.0f);

					button.label = "Restart";
					if (UI::draw_button(button))
					{
						comps.source->stop();
						comps.source->play();
					}

					ImGui::EndGroup();

					ImGui::Spacing();

					ImGui::PushItemWidth(-1);
					float volume = comps.source->get_volume();
					if (ImGui::SliderFloat("##Volume", &volume, 0.0f, 1.0f, "Volume: %.2f"))
					{
						comps.source->set_volume(volume);
					}

					float pitch = comps.source->get_pitch();
					if (ImGui::SliderFloat("##Pitch", &pitch, 0.5f, 2.0f, "Pitch: %.2f"))
					{
						comps.source->set_pitch(pitch);
					}


					bool loop = comps.source->is_looping();
					if (UI::draw_bool("Loop", loop))
					{
						comps.source->set_loop(loop);
					}

					ImGui::PopItemWidth();

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::BeginGroup();
					ImGui::TextDisabled("Status:");
					ImGui::SameLine();

					if (isPlaying)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(50, 220, 50, 255));
						ImGui::Text("Playing");
						ImGui::PopStyleColor();
					}
					else if (comps.source->is_paused())
					{
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(220, 220, 50, 255));
						ImGui::Text("Paused");
						ImGui::PopStyleColor();
					}
					else
					{
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 255));
						ImGui::Text("Stopped");
						ImGui::PopStyleColor();
					}
					ImGui::EndGroup();
				}

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AUDIO_FILE"))
					{
						const char* dropped_path = (const char*)payload->Data;
						comps.path = dropped_path;
						comps.audio_buffer = NodeHelper::load_sound(comps.path);
						comps.source->set_buffer(comps.audio_buffer);
					}
					ImGui::EndDragDropTarget();
				}
			}, false);
	}
	


	void NodeProperties::animated_sprite_2D(Entity entity)
	{
		Tag_Component::imgui_render(entity);
		Transform_Component::imgui_render(entity);
		Texture_Component::imgui_render(entity);
		Animation_Component::imgui_render(entity);


		add_component(entity);
		draw_added_components(entity);
	}

	void NodeProperties::button_2D(Entity entity)
	{
		Tag_Component::imgui_render(entity);
		Transform_Component::imgui_render(entity);
		Button_Layout::imgui_render(entity);
		Text_Component::imgui_render(entity);
		Button_Component::imgui_render(entity);
		Button_Visual::imgui_render(entity);

		add_component(entity);
		draw_added_components(entity);
	}

	void NodeProperties::camera_2D(Entity entity)
	{
		Tag_Component::imgui_render(entity);
		Camera_Component::imgui_render(entity);


		add_component(entity);
		draw_added_components(entity);
	}

	void NodeProperties::character_body_2D(Entity entity)
	{

	}

	void NodeProperties::circle_2D(Entity entity)
	{
		Tag_Component::imgui_render(entity);
		Transform_Component::imgui_render(entity);
		Render2D_Component::imgui_render(entity);

		add_component(entity);
		draw_added_components(entity);
	}

	void NodeProperties::rectangle_2D(Entity entity)
	{

		Tag_Component::imgui_render(entity);
		Transform_Component::imgui_render(entity);
		Render2D_Component::imgui_render(entity);

		add_component(entity);
		draw_added_components(entity);
	}

	void NodeProperties::scene_2D(Entity entity)
	{
		Tag_Component::imgui_render(entity);
		Transform_Component::imgui_render(entity);

		add_component(entity);
		draw_added_components(entity);
	}

	void NodeProperties::sprite_2D(Entity entity)
	{
		Tag_Component::imgui_render(entity);
		Transform_Component::imgui_render(entity);
		Texture_Component::imgui_render(entity);
		Render2D_Component::imgui_render(entity);


		add_component(entity);
		draw_added_components(entity);
	}

	void NodeProperties::text_2D(Entity entity)
	{
		Tag_Component::imgui_render(entity);
		Transform_Component::imgui_render(entity);
		//Render2D_Component::imgui_render(entity);
		Text_Component::imgui_render(entity);

		add_component(entity);
		draw_added_components(entity);
	}

	void NodeProperties::texture_button_2D(Entity entity)
	{
		Tag_Component::imgui_render(entity);
		Transform_Component::imgui_render(entity);
		Texture_Component::imgui_render(entity);
		Textured_Button_Component::imgui_render(entity);
		TextureRect_Component::imgui_render(entity, false);

		add_component(entity);
		draw_added_components(entity);
	}

	void NodeProperties::tilemap_2D(Entity entity)
	{
		Tag_Component::imgui_render(entity);
		Texture_Component::imgui_render(entity);
		Tile_Component::imgui_render(entity);


		add_component(entity);
		draw_added_components(entity);
	}

	void NodeProperties::audio_2D(Entity entity)
	{
		Tag_Component::imgui_render(entity);
		Audio_Component::imgui_render(entity);

		add_component(entity);
		draw_added_components(entity);
	}

}
