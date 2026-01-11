#pragma once

#include <Scene/Entity.hpp>
#include <functional>

namespace ag
{
	struct Component_Info
	{
		const char* name;
		bool (*is_compatible)(NodeType);
		std::function<void(Entity)> create;
		std::function<void(Entity)> imgui_render;
	};

	class NodeProperties
	{
	public:
		static void register_components();

		static void add_component(Entity entity);

		static void animated_sprite_2D(Entity entity);
		static void button_2D(Entity entity);
		static void camera_2D(Entity entity);
		static void character_body_2D(Entity entity);
		static void circle_2D(Entity entity);
		static void rectangle_2D(Entity entity);
		static void scene_2D(Entity entity);
		static void sprite_2D(Entity entity);
		static void text_2D(Entity entity);
		static void texture_button_2D(Entity entity);
		static void tilemap_2D(Entity entity);


		template<typename T>
		inline static bool draw_component_node(const std::string& name, Entity entity,
			std::function<void(T&)> draw_content,
			bool can_remove = false)
		{
			if (!entity.has_component<T>()) return false;

			ImGui::PushID(name.c_str());


			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));
			ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 12.0f);

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
				ImGuiTreeNodeFlags_Framed |
				ImGuiTreeNodeFlags_SpanAvailWidth |
				ImGuiTreeNodeFlags_AllowItemOverlap;

			float start_x = ImGui::GetCursorPosX();
			float available_width = ImGui::GetContentRegionAvail().x;

			bool open = ImGui::TreeNodeEx(name.c_str(), flags);
			bool remove = false;

			if (can_remove)
			{
				float button_width = 26.0f;
				float button_x = start_x + available_width - button_width - ImGui::GetStyle().FramePadding.x * 2;

				// Save cursor position
				float saved_cursor_y = ImGui::GetCursorPosY();
				float saved_cursor_x = ImGui::GetCursorPosX();

				// Draw a dummy at the button position first to extend boundaries
				ImGui::SetCursorPosX(button_x);
				ImGui::SetCursorPosY(saved_cursor_y - ImGui::GetFrameHeight() - ImGui::GetStyle().ItemSpacing.y);
				ImGui::Dummy(ImVec2(button_width * 2.0f, 28));

				// Now set cursor back for the actual button
				ImGui::SetCursorPosX(button_x);
				ImGui::SetCursorPosY(saved_cursor_y - ImGui::GetFrameHeight() - ImGui::GetStyle().ItemSpacing.y);

				// Style the remove button
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.25f, 0.29f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.67f));
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));

				if (ImGui::Button(" X ", ImVec2(button_width * 2.0f, 28)))
				{
					ImGui::OpenPopup("ConfirmRemove##Component");
				}
				ImGui::Dummy(ImVec2(0.0f, 0.0f));
				ImGui::PopStyleVar();
				ImGui::PopStyleColor(3);

				// Restore cursor position for tree content
				ImGui::SetCursorPosY(saved_cursor_y);
				ImGui::SetCursorPosX(saved_cursor_x);

				// Confirmation popup
				if (ImGui::BeginPopup("ConfirmRemove##Component"))
				{
					ImGui::Text("Remove component!");
					ImGui::Dummy(ImVec2(0, 3));
					ImGui::Text("%s", name.c_str());
					ImGui::Separator();

					if (ImGui::Button("Yes", ImVec2(50, 0)))
					{
						remove = true;
						ImGui::CloseCurrentPopup();
					}

					ImGui::SameLine();

					if (ImGui::Button("No", ImVec2(50, 0)))
					{
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}
			}

			if (open)
			{
				auto& component = entity.get_component<T>();
				draw_content(component);
				ImGui::TreePop();
			}

			ImGui::PopStyleVar(3);
			ImGui::PopID();

			if (remove)
			{
				T::remove_component(entity);
				Scene::get_active_scene()->set_save_required();
			}

			return open;
		}



	private:


		inline static std::vector<Component_Info> comps;

		static void draw_added_components(Entity entity);

		template<typename T>
		static void add(Entity entity)
		{
			if (!entity.has_component<T>())
			{
				T::add_component(entity);

				Scene::get_active_scene()->set_save_required();

			}
		}

	};
}
