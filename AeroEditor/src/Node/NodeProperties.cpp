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

		REGISTER_COMPONENT(TileSet_Component);


		REGISTER_COMPONENT(UI_Component);
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



	void NodeProperties::tag(Entity entity)
	{
		draw_component_node<Tag>("Tag", entity,
			[](Tag& tag)
			{

				UI::draw_string("Tag", tag.tag);
				ImGui::Dummy(spacing);

				UI::draw_bool("Visible", tag.is_visible);
				ImGui::Dummy(spacing);

				UI::draw_bool("Lock", tag.locked);
			}, false);
	}
	void NodeProperties::transform(Entity entity)
	{
		draw_component_node<Transform>("Transform", entity,
			[](Transform& transform)
			{
				UI::draw_vec2("Position", transform.position, { 0, 0 });

				ImGui::Dummy(spacing);
				UI::draw_vec2("Scale", transform.scale, { 1.0f, 1.0f });

				ImGui::Dummy(spacing);
				UI::draw_value("Rotation", transform.rotation);
			}, false);
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


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.25f, 0.29f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.67f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));


		if (ImGui::Button("Add Component", ImVec2(button_width, 30)))
		{
			ImGui::OpenPopup("AddComponentModal");
		}


		ImGui::PopStyleColor(3);


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
				UI::draw_value("Thickness", props.thickness, 0.0f);

				ImGui::Dummy(spacing);
				UI::draw_color("Color", props.color);

			}, true);
	}
	void Corner_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Corner_Component>("Corner Component", entity,
			[](Corner_Component& props)
			{

				UI::draw_bool("Uniform", props.uniform);
				ImGui::Dummy(spacing);

				UI::draw_value("Corner", props.corner, 0.0f);

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
			[](Render2D_Component& props) {
				UI::draw_vec2("Size", props.size);

				ImGui::Dummy(spacing);

				UI::draw_color("Color", props.color);
			}, false);
	}
	void Texture_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Texture_Component>("Texture Component", entity,
			[entity](Texture_Component& props)
			{
				UI::draw_texture(entity);
				ImGui::Dummy(spacing);

			}, false);
	}
	void TextureRect_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<TextureRect_Component>("TextureRect Component", entity,
			[entity](TextureRect_Component& props) mutable
			{
				UI::draw_vec2("Position", props.rect.position);
				ImGui::Dummy(spacing);

				vec2u reset_value;
				if (entity.has_component<Texture_Component>())
				{
					const auto& texture = entity.get_component<Texture_Component>();
					reset_value = texture.texture->get_size();
				}
				UI::draw_vec2("Size", props.rect.size, reset_value);

			}, true);
	}
	void TextureFlip_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<TextureFlip_Component>("TextureFlip Component", entity,
			[entity](TextureFlip_Component& props) mutable
			{
				UI::draw_bool("Horizontal", props.horizontal);
				ImGui::Dummy(spacing);

				UI::draw_bool("Vertical", props.vertical);
				ImGui::Dummy(spacing);

			}, true);
	}
	void Transform_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Transform_Component>("Transform Component", entity,
			[](Transform_Component& transform)
			{
				UI::draw_vec2("Position", transform.position, { 0, 0 });

				ImGui::Dummy(spacing);
				UI::draw_vec2("Scale", transform.scale, { 1.0f, 1.0f });

				ImGui::Dummy(spacing);
				UI::draw_value("Rotation", transform.rotation);
			}, false);
	}
	void Tag_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Tag_Component>("Tag Component", entity,
			[](Tag_Component& tag)
			{
				UI::draw_string("Name", tag.name);
				ImGui::Dummy(spacing);

				UI::draw_bool("Visible", tag.visible);
				ImGui::Dummy(spacing);

				UI::draw_bool("Lock", tag.locked);
			}, false);
	}
	void Script_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Script_Component>("Scirpt Component", entity,
			[](Script_Component& props)
			{
				UI::draw_string("Name", props.path);

			}, true);
	}
	void Camera_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Camera_Component>("Camera Component", entity,
			[](Camera_Component& props)
			{
				UI::draw_vec2("Size", props.size, { 1280, 720 });

				ImGui::Dummy(spacing);
				UI::draw_vec2("Center", props.center, props.size / 2 );

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
	void Animation_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Animation_Component>("Animation Component", entity,
			[entity](Animation_Component& anim)
			{
				UI::draw_animation(entity);
			}, false);
	}
	void Tile_Component::imgui_render(Entity entity)
	{
		NodeProperties::draw_component_node<Tile_Component>("Tile Component", entity,
			[entity](Tile_Component& tile)
			{
				UI::draw_vec2("Size", tile.size, { 32, 32 });

				ImGui::Dummy(spacing);
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


		add_component(entity);
		draw_added_components(entity);
	}

	void NodeProperties::text_2D(Entity entity)
	{

	}

	void NodeProperties::texture_button_2D(Entity entity)
	{

	}

	void NodeProperties::tilemap_2D(Entity entity)
	{
		Tag_Component::imgui_render(entity);
		Texture_Component::imgui_render(entity);
		Tile_Component::imgui_render(entity);


		add_component(entity);
		draw_added_components(entity);
	}
}
