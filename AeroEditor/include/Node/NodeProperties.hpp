#pragma once

#include <Scene/Entity.hpp>

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


		static void tag(Entity entity);
		static void transform(Entity entity);
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
		static bool draw_component_node(const std::string& name, Entity entity,
			std::function<void(T&)> draw_content,
			bool can_remove = false)
		{
			if (!entity.has_component<T>()) return false;

			ImGui::PushID(name.c_str());

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));
			ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 12.0f);

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
				ImGuiTreeNodeFlags_Framed |
				ImGuiTreeNodeFlags_SpanAvailWidth |
				ImGuiTreeNodeFlags_AllowItemOverlap;

			bool open = ImGui::TreeNodeEx(name.c_str(), flags);
			bool remove = false;

			if (can_remove)
			{
				ImGui::SameLine(ImGui::GetContentRegionAvail().x - 25.0f);

				// Style the remove button
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.25f, 0.29f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.67f));
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

				if (ImGui::Button(" X ", ImVec2(50, 28)))
				{
					ImGui::OpenPopup("ConfirmRemove##Component");
				}

				ImGui::PopStyleVar(2);
				ImGui::PopStyleColor(3);

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
				entity.remove_component<T>();
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
				entity.add_component<T>();
			}
		}

	};
}
