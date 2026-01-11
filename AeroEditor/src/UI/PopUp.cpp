#include <UI/PopUp.hpp>
#include <Aero.hpp>

namespace ag
{
	void Texture_PopUp::draw_popup(PopUpModel& model, Extra_Settings& extra_settings, Entity entity)
	{
		if (model.window_size.x == 0 || model.window_size.y == 0)
		{
			model.window_size = { Application::get().get_window().get_size().x * 0.7f, Application::get().get_window().get_size().y * 0.8f };
		}

		ImGui::OpenPopup(model.id.c_str());
		static PopUpSettings s_settings;

		ImGuiWindowFlags flag = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImVec4 original_dim_color = ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg];
		ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(ImVec2(center.x - model.window_size.x * 0.5f, center.y - model.window_size.y * 0.5f), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(model.window_size.to_imvec2(), ImGuiCond_Appearing);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0.0f, 0.0f, 0.0f, 0.85f));

		
		if (ImGui::BeginPopupModal(model.id.c_str(), NULL, flag))
		{
			ImVec2 window_pos = ImGui::GetWindowPos();
			ImVec2 window_size = ImGui::GetWindowSize();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			float title_bar_height = 40.0f;
			ImU32 title_bar_color = IM_COL32(50, 50, 50, 255);
			draw_list->AddRectFilled(
				window_pos,
				ImVec2(window_pos.x + window_size.x, window_pos.y + title_bar_height),
				title_bar_color
			);

			ImGui::SetWindowFontScale(1.1f);
			ImGui::SetCursorPos(ImVec2(10, (title_bar_height - ImGui::GetFontSize()) * 0.5f));
			ImGui::TextColored(ImVec4(1, 1, 1, 1), model.name.c_str());

			float close_button_size = title_bar_height - 10;
			ImGui::SetCursorPos(ImVec2(
				window_size.x - close_button_size - 5,
				(title_bar_height - close_button_size) * 0.5f
			));


			ImGui::SetCursorPos(ImVec2(window_size.x - 60, 0));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));

			if (ImGui::Button(" X ", ImVec2(60, 40)))
			{
				//model.on_close();
			}
			ImGui::SetWindowFontScale(1.0f);
			ImGui::PopStyleColor(3);

			ImGui::SetCursorPosY(title_bar_height + 5);
			ImGui::Dummy(ImVec2(0.0f, 0.0f));

			{
				draw_content(model, s_settings, extra_settings, entity);
			}

			ImGui::EndPopup();
		}

		ImGui::PopStyleColor(2);

		ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg] = original_dim_color;
	}


	void Texture_PopUp::draw_content(PopUpModel& model, PopUpSettings& settings, Extra_Settings& extra_settings, Entity entity)
	{
		auto& texture = entity.get_component<Texture_Component>().texture;
		vec2f texture_size = texture->get_size();
		ImTextureID texture_id = (ImTextureID)(intptr_t)texture->get_texture_id();

		float available_width = ImGui::GetContentRegionAvail().x;
		float left_width = available_width * 0.65f - 5.0f;
		float right_width = available_width * 0.35f - 5.0f;

		ImGui::BeginChild("TextureColumn", ImVec2(left_width, 0), true);
		{
			ImGui::Text("Texture Preview");
			ImGui::Separator();

			ImGui::BeginChild("TextureContainer", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			{
				if (settings.view_size.x == 0 || settings.view_size.y == 0)
				{
					settings.view_size = ImGui::GetContentRegionAvail();
				}

				vec2f mouse_screen = ImGui::GetMousePos();
				vec2f child_pos = ImGui::GetWindowPos();
				vec2f child_size = ImGui::GetWindowSize();

				settings.current_mouse_pos = mouse_screen - child_pos;


				vec2f container = ImGui::GetContentRegionAvail();
				vec2f container_start_pos = ImGui::GetCursorScreenPos();
				bool is_hovering_container = ImGui::IsMouseHoveringRect(container_start_pos.to_imvec2(), (container_start_pos + container).to_imvec2());

				vec2f image_pos = vec2f(0, 0) - texture_size * 0.5f;
				vec2f padding = { 10.0f, 10.0f };

				if (is_hovering_container && ImGui::IsMouseClicked(2))
				{
					settings.is_dragging = true;
					settings.last_mouse_pos = settings.current_mouse_pos;
				}

				if (settings.is_dragging)
				{
					if (ImGui::IsMouseDown(2))
					{
						vec2f start = Math::screen_to_world(settings.last_mouse_pos, Math::get_float_rect(settings.view_size, settings.view_center), container);
						vec2f end = Math::screen_to_world(settings.current_mouse_pos, Math::get_float_rect(settings.view_size, settings.view_center), container);
						vec2f delta = start - end;
						settings.view_center += delta;
						settings.last_mouse_pos = settings.current_mouse_pos;
					}
					else
					{
						settings.is_dragging = false;
					}
				}

				if (is_hovering_container && ImGui::GetIO().MouseWheel != 0)
				{
					float scale_factor = (ImGui::GetIO().MouseWheel > 0) ? 0.9f : 1.1f;
					vec2f world_before_zoom = Math::screen_to_world(settings.current_mouse_pos, Math::get_float_rect(settings.view_size, settings.view_center), container);
					settings.view_size *= scale_factor;
					if (settings.view_size.x < texture_size.x || settings.view_size.y < texture_size.y)
					{
						vec2f world_after_zoom = Math::screen_to_world(settings.current_mouse_pos, Math::get_float_rect(settings.view_size, settings.view_center), container);
						vec2f offset = world_before_zoom - world_after_zoom;
						settings.view_center += offset;
					}
					else
					{
						settings.view_center = vec2f();
					}
				}

				{
					vec2f half_view = settings.view_size * 0.5f;
					if (settings.view_size.x < texture_size.x + 2 * padding.x)
					{
						float min_x = image_pos.x - padding.x + half_view.x;
						float max_x = image_pos.x + texture_size.x + padding.x - half_view.x;
						settings.view_center.x = std::clamp(settings.view_center.x, min_x, max_x);
					}
					else
					{
						settings.view_center.x = image_pos.x + texture_size.x * 0.5f;
					}

					if (settings.view_size.y < texture_size.y + 2 * padding.y)
					{
						float min_y = image_pos.y - padding.y + half_view.y;
						float max_y = image_pos.y + texture_size.y + padding.y - half_view.y;
						settings.view_center.y = std::clamp(settings.view_center.y, min_y, max_y);
					}
					else
					{
						settings.view_center.y = image_pos.y + texture_size.y * 0.5f;
					}
				}

				
				vec2f screen_pos = Math::world_to_screen(image_pos, Math::get_float_rect(settings.view_size, settings.view_center), container);
				vec2f screen_size = Math::world_size_to_screen_size(texture_size, settings.view_size, container);
				ImGui::SetCursorPos(screen_pos.to_imvec2());
				vec2f window_pos = ImGui::GetWindowPos();
				screen_pos += window_pos;
				ImDrawList* draw_list = ImGui::GetWindowDrawList();

				vec2f padding_size = Math::world_size_to_screen_size(padding, settings.view_size, container);
				draw_list->AddRectFilled(
					(screen_pos - padding_size).to_imvec2(),
					(screen_pos + screen_size + padding_size).to_imvec2(),
					IM_COL32(50, 50, 50, 200)
				);

				ImGui::Image(texture_id, screen_size.to_imvec2());

				{
					vec2i line_count;
					vec2f tile_size;
					if (extra_settings.use_size)
					{
						line_count = texture_size / extra_settings.size;
						tile_size = extra_settings.size;
					}
					else
					{
						line_count = extra_settings.size;
						tile_size = texture_size / extra_settings.size;
					}
					if (extra_settings.draw_lines)
						draw_lines(screen_pos, screen_size, tile_size,line_count, texture_size);


					vec2f button_size = Math::world_size_to_screen_size(tile_size, settings.view_size, container);
					if (extra_settings.create_buttons)
						create_buttons(screen_pos, screen_size, tile_size, line_count, texture_size, button_size);

				}



				ImGui::EndChild();
			}
			ImGui::EndChild();
		}
		ImGui::SameLine(0.0f, 10.0f);
		ImGui::BeginChild("Controls", ImVec2(right_width, 0), true);
		{

			ImGui::EndChild();
		}


	}

	void Texture_PopUp::draw_lines(const vec2f& screen_pos, const vec2f& screen_size, const vec2f& size, const vec2i& line_count, const vec2i& texture_size)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		for (int x = 0; x <= line_count.x; x++)
		{// Vertical
			float px = screen_pos.x + x * size.x * (screen_size.x / texture_size.x);
			draw_list->AddLine(ImVec2(px, screen_pos.y),
				ImVec2(px, screen_pos.y + screen_size.y),
				IM_COL32(255, 255, 255, 100), 2.0f);
		}

		for (int y = 0; y <= line_count.y; y++)
		{// Horizontal
			float py = screen_pos.y + y * size.y * (screen_size.y / texture_size.y);
			draw_list->AddLine(
				ImVec2(screen_pos.x, py),
				ImVec2(screen_pos.x + screen_size.x, py),
				IM_COL32(255, 255, 255, 100), 2.0f);
		}
	}

	void Texture_PopUp::create_buttons(const vec2f& screen_pos, const vec2f& screen_size, const vec2i& size,const vec2i& line_count, const vec2i& texture_size, const vec2f& button_size)
	{
		for (int x = 0; x < line_count.x; x++)
		{
			for (int y = 0; y < line_count.y; y++)
			{
				vec2u tile_id = { (AG_uint)x, (AG_uint)y };
				if (ImGui::InvisibleButton("##Cell", button_size.to_imvec2()))
				{

				}

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();

					ImGui::EndTooltip();
				}
			}
		}

	}



	void Create_Open_Popup::draw_popup(PopUpModel& model)
	{
		if (model.window_size.x == 0 || model.window_size.y == 0)
		{
			model.window_size = { Application::get().get_window().get_size().x * 0.7f, Application::get().get_window().get_size().y * 0.8f };
		}

		ImGui::OpenPopup(model.id.c_str());

		ImGuiWindowFlags flag = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImVec4 original_dim_color = ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg];
		ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(ImVec2(center.x - model.window_size.x * 0.5f, center.y - model.window_size.y * 0.5f), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(model.window_size.to_imvec2(), ImGuiCond_Appearing);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0.0f, 0.0f, 0.0f, 0.85f));


		if (ImGui::BeginPopupModal(model.id.c_str(), NULL, flag))
		{
			ImVec2 window_pos = ImGui::GetWindowPos();
			ImVec2 window_size = ImGui::GetWindowSize();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			float title_bar_height = 40.0f;
			ImU32 title_bar_color = IM_COL32(50, 50, 50, 255);
			draw_list->AddRectFilled(
				window_pos,
				ImVec2(window_pos.x + window_size.x, window_pos.y + title_bar_height),
				title_bar_color
			);

			ImGui::SetWindowFontScale(1.1f);
			ImGui::SetCursorPos(ImVec2(10, (title_bar_height - ImGui::GetFontSize()) * 0.5f));
			ImGui::TextColored(ImVec4(1, 1, 1, 1), model.name.c_str());

			float close_button_size = title_bar_height - 10;
			ImGui::SetCursorPos(ImVec2(
				window_size.x - close_button_size - 5,
				(title_bar_height - close_button_size) * 0.5f
			));


			ImGui::SetCursorPos(ImVec2(window_size.x - 60, 0));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));

			if (ImGui::Button(" X ", ImVec2(60, 40)))
			{
				model.on_close();
			}
			ImGui::SetWindowFontScale(1.0f);
			ImGui::PopStyleColor(3);

			ImGui::SetCursorPosY(title_bar_height + 5);
			ImGui::Dummy(ImVec2(0.0f, 0.0f));

			{
				model.draw_content();
			}

			ImGui::EndPopup();
		}

		ImGui::PopStyleColor(2);

		ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg] = original_dim_color;
	}

}