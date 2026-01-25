#pragma once

#include <imgui.h>
#include <string>
#include <type_traits>
#include <Math/Math.hpp>
#include <Scene/Entity.hpp>
#include <Core/Application.hpp>
#include <UI/Icons.hpp>
#include <Scene/Scene.hpp>

namespace ag
{

	struct PropertyStyle {
		ImVec4 label_color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
		ImVec4 value_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImVec4 button_color = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
		ImVec4 input_color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		float min_width = 150.0f;
		float speed = 1.0f;
		const char* format = "%.3f";
		bool show_reset_buttons = true;
		bool show_labels = true;
		bool drag_enabled = true;
		bool slider_enabled = false;
		float slider_min = 0.0f;
		float slider_max = 1.0f;
	};
	struct Panels
	{
		bool properties_panel = true;
		bool texture_selector = false;
		bool scene_panel = true;
		bool tilemap_selector = false;
		bool console_panel = true;
		bool animation_selector = false;
		bool tilemap_register = false;
		bool auto_tiling_register = false;
		bool save_changes_panel = false;
		bool create_new_scene = false;
		bool create_new_script = false;
	};
	struct ButtonColors
	{
		Color normal;
		Color hover;
		Color active;
		Color disabled;
	};
	struct ButtonTextColors
	{
		Color normal;
		Color hover;
		Color active;
		Color disabled;
	};
	struct GUI_Button
	{
		ButtonColors background;
		ButtonTextColors text;
		std::string label;
		vec2f size = { 0, 0 };
		float radius = 4.0f;
		bool enabled = true;
	};

	class UI
	{
	public:
		static void draw_texture(Entity entity);
		static void draw_window();
		static void draw_animation(Entity entity);
		static void draw_frame_selector(Entity entity, const std::string& current_animation);

		static void draw_tilemap_register(Entity entity);
		static void draw_autotiling_register(Entity entity);
		static bool texture_selector(Entity entity, uint_rect& texture_rect);
		static void content_browser();
		static bool is_right_file(const std::filesystem::path& path);
		static bool is_image(const std::filesystem::path& path);
		static void draw_folder_node(const std::filesystem::path& directory, int depth = 0);
		static void draw_script_selector(Entity entity);
		static bool draw_tilemap_selector(Entity entity, vec2u& id, std::string& set_name, bool& use_autotile);

		static void create_new_scene();
		static void create_new_script(Entity entity);
		static void save_changes();
		static void popup_functions();

		static bool draw_button(const GUI_Button& btn);
		static void test_popup(Entity entity);
		static void custom_popup(const std::string& popup_id, const std::string& popup_name, std::function<void()> draw_content, std::function<void()> close);
		static void draw_menu_bar();
		static void run_current_scene();
		static void draw_console();


		static Panels& get_uistate_panels() { return s_show_panels; }

		static void draw_title(const char* label)
		{
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::SeparatorText(label);
			ImGui::Spacing();
		}

		// Helper functions
		inline static void BeginProperty(const char* label, const char* tooltip = nullptr, bool same_line = true) {
			const float label_width = s_property_style.min_width;

			ImGui::PushID(label);

			if (s_property_style.show_labels) {
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, label_width);
				ImGui::TextColored(s_property_style.label_color, "%s", label);
				if (tooltip && ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::TextUnformatted(tooltip);
					ImGui::EndTooltip();
				}

				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
			}
		}

		inline static void EndProperty() {
			if (s_property_style.show_labels) {
				ImGui::PopItemWidth();
				ImGui::Columns(1);
			}
			ImGui::PopID();
		}

		inline static bool DrawResetButton(const char* label, const ImVec4& color) {
			const float line_height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
			const ImVec2 button_size = { line_height + 3.0f, line_height };

			ImGui::PushStyleColor(ImGuiCol_Button, color);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
				ImVec4(color.x * 1.2f, color.y * 1.2f, color.z * 1.2f, color.w));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive,
				ImVec4(color.x * 0.8f, color.y * 0.8f, color.z * 0.8f, color.w));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

			const bool clicked = ImGui::Button(label, button_size);

			ImGui::PopStyleColor(4);

			return clicked;
		}

		template<typename T>
		inline bool DrawDragScalar(const char* id, T* value, ImGuiDataType data_type,
			float speed = 1.0f, const char* format = "%.3f") {
			if (s_property_style.drag_enabled)
			{
				return ImGui::DragScalar(id, data_type, value, speed, nullptr, nullptr, format);
			}
			else
			{
				return ImGui::InputScalar(id, data_type, value, nullptr, nullptr, format);
			}
		}

		// Main property widgets
		template<typename T>
		static bool draw_vec2(const char* label, T& vec, const vec2f& reset_value = vec2f(0, 0),
			const char* tooltip = nullptr) {
			bool changed = false;
			BeginProperty(label, tooltip);

			const float total_width = ImGui::CalcItemWidth();
			const float line_height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
			const ImVec2 button_size = { line_height + 3.0f, line_height };
			const float spacing = ImGui::GetStyle().ItemSpacing.x;

			// Calculate input field width
			float input_width = total_width;
			if (s_property_style.show_reset_buttons) {
				input_width = (total_width - (button_size.x * 2.0f) - spacing * 3.0f) / 2.0f;
			}
			else {
				input_width = (total_width - spacing) / 2.0f;
			}

			ImGuiDataType data_type;
			if constexpr (std::is_same_v<T, vec2f>) {
				data_type = ImGuiDataType_Float;
			}
			else if constexpr (std::is_same_v<T, vec2i>) {
				data_type = ImGuiDataType_S32;
			}
			else if constexpr (std::is_same_v<T, vec2u>) {
				data_type = ImGuiDataType_U32;
			}
			else {
				static_assert(!sizeof(T*), "Unsupported vec2 type");
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 4, 0 });

			// X Component
			if (s_property_style.show_reset_buttons) {
				if (DrawResetButton("X", s_property_style.button_color)) {
					vec.x = reset_value.x;
					changed = true;
				}
				ImGui::SameLine();
			}

			ImGui::PushItemWidth(input_width);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, s_property_style.input_color);

			if (ImGui::InputScalar("##X", data_type, &vec.x))
			{
				changed = true;
			}
			ImGui::PopStyleColor();
			ImGui::PopItemWidth();


			// Y Component
			ImGui::SameLine();
			if (s_property_style.show_reset_buttons)
			{
				if (DrawResetButton("Y", s_property_style.button_color))
				{
					vec.y = reset_value.y;
					changed = true;
				}
				ImGui::SameLine();
			}

			ImGui::PushItemWidth(input_width);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, s_property_style.input_color);
			if (ImGui::InputScalar("##Y", data_type, &vec.y))
			{
				changed = true;
			}
			ImGui::PopStyleColor();
			ImGui::PopItemWidth();

			// Quick actions
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
				ImGui::OpenPopup("vec2_context_menu");
			}

			if (ImGui::BeginPopup("vec2_context_menu")) {
				if (ImGui::MenuItem("Copy")) {
					ImGui::SetClipboardText(fmt::format("{}, {}", vec.x, vec.y).c_str());
				}
				if (ImGui::MenuItem("Paste")) {
					const char* clipboard = ImGui::GetClipboardText();
					if (clipboard) {
						std::string str(clipboard);
						size_t comma = str.find(',');
						if (comma != std::string::npos) {
							try {
								vec.x = static_cast<decltype(vec.x)>(std::stof(str.substr(0, comma)));
								vec.y = static_cast<decltype(vec.y)>(std::stof(str.substr(comma + 1)));
								changed = true;
							}
							catch (...) {
								// Invalid paste
							}
						}
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Normalize")) {
					if constexpr (std::is_same_v<T, vec2f>) {
						vec = vec.normalized();
						changed = true;
					}
				}
				if (ImGui::MenuItem("Zero")) {
					vec.x = vec.y = 0;
					changed = true;
				}
				ImGui::EndPopup();
			}

			ImGui::PopStyleVar();
			EndProperty();

			if (changed)
				Scene::get_active_scene()->set_save_required();

			return changed;
		}

		template<typename T>
		static bool draw_vec3(const char* label, T& vec, const vec3f& reset_value = vec3f(0, 0, 0),
			const char* tooltip = nullptr) {
			bool changed = false;
			BeginProperty(label, tooltip);

			const float total_width = ImGui::CalcItemWidth();
			const float line_height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 3.0f;
			const ImVec2 button_size = { line_height + 5.0f, line_height };
			const float spacing = ImGui::GetStyle().ItemSpacing.x;


			float input_width = total_width;
			if (s_property_style.show_reset_buttons) {
				input_width = (total_width - (button_size.x * 3.0f) - spacing * 4.0f) / 3.0f;
			}
			else {
				input_width = (total_width - spacing * 2.0f) / 3.0f;
			}

			ImGuiDataType data_type;
			if constexpr (std::is_same_v<T, vec3f>) {
				data_type = ImGuiDataType_Float;
			}
			else if constexpr (std::is_same_v<T, vec3i>) {
				data_type = ImGuiDataType_S32;
			}
			else if constexpr (std::is_same_v<T, vec3u>) {
				data_type = ImGuiDataType_U32;
			}
			else {
				static_assert(!sizeof(T*), "Unsupported vec3 type");
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 4, 0 });


			if (s_property_style.show_reset_buttons) {
				if (DrawResetButton("X", s_property_style.button_color)) {
					vec.x = static_cast<decltype(vec.x)>(reset_value.x);
					changed = true;
				}
				ImGui::SameLine();
			}

			ImGui::PushItemWidth(input_width);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, s_property_style.input_color);
			if (DrawDragScalar("##X", &vec.x, data_type, s_property_style.speed, s_property_style.format)) {
				changed = true;
			}
			ImGui::PopStyleColor();
			ImGui::PopItemWidth();

			// Y Component
			ImGui::SameLine();
			if (s_property_style.show_reset_buttons) {
				if (DrawResetButton("Y", s_property_style.button_color)) {
					vec.y = static_cast<decltype(vec.y)>(reset_value.y);
					changed = true;
				}
				ImGui::SameLine();
			}

			ImGui::PushItemWidth(input_width);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, s_property_style.input_color);
			if (DrawDragScalar("##Y", &vec.y, data_type, s_property_style.speed, s_property_style.format)) {
				changed = true;
			}
			ImGui::PopStyleColor();
			ImGui::PopItemWidth();

			// Z Component
			ImGui::SameLine();
			if (s_property_style.show_reset_buttons) {
				if (DrawResetButton("Z", s_property_style.button_color)) {
					vec.z = static_cast<decltype(vec.z)>(reset_value.z);
					changed = true;
				}

			}

			ImGui::PushItemWidth(input_width);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, s_property_style.input_color);
			if (DrawDragScalar("##Z", &vec.z, data_type, s_property_style.speed, s_property_style.format)) {
				changed = true;
			}
			ImGui::PopStyleColor();
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();
			EndProperty();
			ImGui::Spacing();

			if (changed)
				Scene::get_active_scene()->set_save_required();

			return changed;
		}

		template<typename T>
		static bool draw_vec4(const char* label, T& vec, const vec4f& reset_value = vec4f(0, 0, 0, 0),
			const char* tooltip = nullptr) {
			bool changed = false;
			BeginProperty(label, tooltip);

			// For vec4, use two rows
			const float total_width = ImGui::CalcItemWidth();
			const float line_height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
			const ImVec2 button_size = { line_height + 3.0f, line_height };
			const float spacing = ImGui::GetStyle().ItemSpacing.x;

			// Calculate input field width
			float input_width = (total_width - (button_size.x * 2.0f) - spacing * 3.0f) / 2.0f;

			ImGuiDataType data_type;
			if constexpr (std::is_same_v<T, vec4f>) {
				data_type = ImGuiDataType_Float;
			}
			else if constexpr (std::is_same_v<T, vec4i>) {
				data_type = ImGuiDataType_S32;
			}
			else if constexpr (std::is_same_v<T, vec4u>) {
				data_type = ImGuiDataType_U32;
			}
			else {
				static_assert(!sizeof(T*), "Unsupported vec4 type");
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 4, 0 });

			// First row (X, Y)
			// X Component
			if (s_property_style.show_reset_buttons) {
				if (DrawResetButton("X", s_property_style.button_color)) {
					vec.x = static_cast<decltype(vec.x)>(reset_value.x);
					changed = true;
				}
				ImGui::SameLine();
			}

			ImGui::PushItemWidth(input_width);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, s_property_style.input_color);
			if (DrawDragScalar("##X", &vec.x, data_type, s_property_style.speed, s_property_style.format)) {
				changed = true;
			}
			ImGui::PopStyleColor();
			ImGui::PopItemWidth();

			// Y Component
			ImGui::SameLine();
			if (s_property_style.show_reset_buttons) {
				if (DrawResetButton("Y", s_property_style.button_color)) {
					vec.y = static_cast<decltype(vec.y)>(reset_value.y);
					changed = true;
				}
				ImGui::SameLine();
			}

			ImGui::PushItemWidth(input_width);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, s_property_style.input_color);
			if (DrawDragScalar("##Y", &vec.y, data_type, s_property_style.speed, s_property_style.format)) {
				changed = true;
			}
			ImGui::PopStyleColor();
			ImGui::PopItemWidth();

			// Second row (Z, W)
			ImGui::NewLine();

			// Z Component
			if (s_property_style.show_reset_buttons) {
				if (DrawResetButton("Z", s_property_style.button_color)) {
					vec.z = static_cast<decltype(vec.z)>(reset_value.z);
					changed = true;
				}
				ImGui::SameLine();
			}

			ImGui::PushItemWidth(input_width);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, s_property_style.input_color);
			if (DrawDragScalar("##Z", &vec.z, data_type, s_property_style.speed, s_property_style.format)) {
				changed = true;
			}
			ImGui::PopStyleColor();
			ImGui::PopItemWidth();

			// W Component
			ImGui::SameLine();
			if (s_property_style.show_reset_buttons) {
				if (DrawResetButton("W", s_property_style.button_color)) {
					vec.w = static_cast<decltype(vec.w)>(reset_value.w);
					changed = true;
				}
				ImGui::SameLine();
			}

			ImGui::PushItemWidth(input_width);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, s_property_style.input_color);
			if (DrawDragScalar("##W", &vec.w, data_type, s_property_style.speed, s_property_style.format)) {
				changed = true;
			}
			ImGui::PopStyleColor();
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();
			EndProperty();

			if (changed)
				Scene::get_active_scene()->set_save_required();

			return changed;
		}

		template<typename T>
		static bool draw_value(const char* label, T& value, T min = std::numeric_limits<T>::lowest(),
			T max = std::numeric_limits<T>::max(), const char* tooltip = nullptr) {
			bool changed = false;
			BeginProperty(label, tooltip);

			ImGuiDataType data_type;
			const char* format = s_property_style.format;

			if constexpr (std::is_same_v<T, float>) {
				data_type = ImGuiDataType_Float;
			}
			else if constexpr (std::is_same_v<T, double>) {
				data_type = ImGuiDataType_Double;
				format = "%.6f";
			}
			else if constexpr (std::is_same_v<T, int>) {
				data_type = ImGuiDataType_S32;
				format = "%d";
			}
			else if constexpr (std::is_same_v<T, unsigned int>) {
				data_type = ImGuiDataType_U32;
				format = "%u";
			}
			else if constexpr (std::is_same_v<T, int8_t>) {
				data_type = ImGuiDataType_S8;
				format = "%d";
			}
			else if constexpr (std::is_same_v<T, uint8_t>) {
				data_type = ImGuiDataType_U8;
				format = "%u";
			}
			else if constexpr (std::is_same_v<T, int16_t>) {
				data_type = ImGuiDataType_S16;
				format = "%d";
			}
			else if constexpr (std::is_same_v<T, uint16_t>) {
				data_type = ImGuiDataType_U16;
				format = "%u";
			}
			else if constexpr (std::is_same_v<T, int64_t>) {
				data_type = ImGuiDataType_S64;
				format = "%lld";
			}
			else if constexpr (std::is_same_v<T, uint64_t>) {
				data_type = ImGuiDataType_U64;
				format = "%llu";
			}
			else {
				static_assert(!sizeof(T*), "Unsupported value type");
			}

			if (s_property_style.slider_enabled && std::is_floating_point_v<T>) {
				float float_value = static_cast<float>(value);
				if (ImGui::SliderFloat("##value", &float_value,
					static_cast<float>(min),
					static_cast<float>(max), format)) {
					value = static_cast<T>(float_value);
					changed = true;
				}
			}
			else if (s_property_style.drag_enabled) {
				if (ImGui::DragScalar("##value", data_type, &value,
					static_cast<float>(s_property_style.speed),
					&min, &max, format)) {
					changed = true;
				}
			}
			else {
				if (ImGui::InputScalar("##value", data_type, &value, nullptr, nullptr, format)) {
					changed = true;
				}
			}

			// Context menu for additional options
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
				ImGui::OpenPopup("value_context_menu");
			}

			if (ImGui::BeginPopup("value_context_menu")) {
				if (ImGui::MenuItem("Copy")) {
					if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
						ImGui::SetClipboardText(fmt::format("{}", value).c_str());
					}
					else {
						ImGui::SetClipboardText(fmt::format("{}", value).c_str());
					}
				}

				if (ImGui::MenuItem("Paste")) {
					const char* clipboard = ImGui::GetClipboardText();
					if (clipboard) {
						try {
							if constexpr (std::is_same_v<T, float>) {
								value = std::stof(clipboard);
								changed = true;
							}
							else if constexpr (std::is_same_v<T, int>) {
								value = std::stoi(clipboard);
								changed = true;
							}
							else if constexpr (std::is_same_v<T, unsigned int>) {
								value = static_cast<unsigned int>(std::stoul(clipboard));
								changed = true;
							}
						}
						catch (...) {
							// Invalid paste
						}
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Reset to Zero")) {
					value = T{ 0 };
					changed = true;
				}

				if (ImGui::MenuItem("Set to Min")) {
					value = min;
					changed = true;
				}

				if (ImGui::MenuItem("Set to Max")) {
					value = max;
					changed = true;
				}

				ImGui::EndPopup();
			}

			EndProperty();


			if (changed)
				Scene::get_active_scene()->set_save_required();

			return changed;
		}

		static bool draw_color(const char* label, Color& color,
			ImGuiColorEditFlags flags = ImGuiColorEditFlags_None,
			const char* tooltip = nullptr)
		{
			bool changed = false;
			BeginProperty(label, tooltip);

			vec4f normalized =
			{
					color.r / 255.0f,
					color.g / 255.0f,
					color.b / 255.0f,
					color.a / 255.0f
			};

			if (ImGui::ColorEdit4("##color", &normalized.x, flags))
			{
				color.r = static_cast<uint8_t>(normalized.x * 255.0f);
				color.g = static_cast<uint8_t>(normalized.y * 255.0f);
				color.b = static_cast<uint8_t>(normalized.z * 255.0f);
				color.a = static_cast<uint8_t>(normalized.w * 255.0f);
				changed = true;
			}

			ImGui::SameLine(0, 4);
			if (ImGui::Button("Picker", ImVec2(60, 0)))
			{
				ImGui::OpenPopup("color_picker_popup");
			}

			if (ImGui::BeginPopup("color_picker_popup"))
			{
				ImGui::ColorPicker4("##picker", &normalized.x,
					flags | ImGuiColorEditFlags_DisplayRGB |
					ImGuiColorEditFlags_DisplayHSV |
					ImGuiColorEditFlags_DisplayHex);

				if (ImGui::Button("Apply"))
				{
					color.r = static_cast<uint8_t>(normalized.x * 255.0f);
					color.g = static_cast<uint8_t>(normalized.y * 255.0f);
					color.b = static_cast<uint8_t>(normalized.z * 255.0f);
					color.a = static_cast<uint8_t>(normalized.w * 255.0f);
					changed = true;
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
			{
				ImGui::OpenPopup("color_context_menu");
			}

			if (ImGui::BeginPopup("color_context_menu"))
			{
				if (ImGui::MenuItem("Copy as Hex"))
				{
					ImGui::SetClipboardText(fmt::format("#{:02X}{:02X}{:02X}{:02X}",
						color.r, color.g, color.b, color.a).c_str());
				}

				if (ImGui::MenuItem("Copy as RGB"))
				{
					ImGui::SetClipboardText(fmt::format("rgb({}, {}, {})",
						color.r, color.g, color.b).c_str());
				}

				if (ImGui::MenuItem("Copy as RGBA"))
				{
					ImGui::SetClipboardText(fmt::format("rgba({}, {}, {}, {})",
						color.r, color.g, color.b, color.a / 255.0f).c_str());
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Set to Black"))
				{
					color = Color::Black;
					changed = true;
				}

				if (ImGui::MenuItem("Set to White"))
				{
					color = Color::White;
					changed = true;
				}

				if (ImGui::MenuItem("Set to Red"))
				{
					color = Color::Red;
					changed = true;
				}

				if (ImGui::MenuItem("Set to Green"))
				{
					color = Color::Green;
					changed = true;
				}

				if (ImGui::MenuItem("Set to Blue"))
				{
					color = Color::Blue;
					changed = true;
				}

				ImGui::EndPopup();
			}

			EndProperty();


			if (changed)
				Scene::get_active_scene()->set_save_required();

			return changed;
		}

		static bool draw_bool(const char* label, bool& value, const char* tooltip = nullptr) {
			bool changed = false;
			BeginProperty(label, tooltip);

			if (ImGui::Checkbox("##value", &value)) {
				changed = true;
			}

			EndProperty();

			if (changed)
				Scene::get_active_scene()->set_save_required();

			return changed;
		}

		static bool draw_string(const char* label, std::string& value, size_t buffer_size = 256,
			const char* tooltip = nullptr) {
			bool changed = false;
			BeginProperty(label, tooltip);


			static std::unordered_map<std::string, std::vector<char>> buffers;

			std::string key = std::string(label) + "##" + value;
			auto& buffer = buffers[key];

			if (buffer.empty() || std::string(buffer.data()) != value) {
				buffer.resize(buffer_size);
				std::strncpy(buffer.data(), value.c_str(), buffer_size - 1);
				buffer[buffer_size - 1] = '\0';
			}

			if (ImGui::InputText("##value", buffer.data(), buffer_size))
			{
				value = std::string(buffer.data());
				changed = true;
			}


			EndProperty();


			if (changed)
				Scene::get_active_scene()->set_save_required();

			return changed;
		}

		static bool draw_string_multiline(
			const char* label,
			std::string& value,
			size_t buffer_size = 1024,
			ImVec2 box_size = ImVec2(0, 100),
			const char* tooltip = nullptr)
		{
			bool changed = false;
			BeginProperty(label, tooltip);

			static std::unordered_map<std::string, std::vector<char>> buffers;

			// Stable key per property
			std::string key = std::string(label) + "##multiline";
			auto& buffer = buffers[key];

			if (buffer.empty() || std::string(buffer.data()) != value)
			{
				buffer.resize(buffer_size);
				std::strncpy(buffer.data(), value.c_str(), buffer_size - 1);
				buffer[buffer_size - 1] = '\0';
			}

			if (ImGui::InputTextMultiline(
				"##value",
				buffer.data(),
				buffer_size,
				box_size,
				ImGuiInputTextFlags_AllowTabInput))
			{
				value = std::string(buffer.data());
				changed = true;
			}

			EndProperty();

			if (changed)
				Scene::get_active_scene()->set_save_required();

			return changed;
		}


		template<typename Enum>
		static bool draw_enum(const char* label, Enum& value, const std::vector<std::string>& options,
			const char* tooltip = nullptr) {
			bool changed = false;
			BeginProperty(label, tooltip);

			int current_item = static_cast<int>(value);
			if (ImGui::Combo("##value", &current_item,
				[](void* data, int idx, const char** out_text) {
					auto& opts = *static_cast<std::vector<std::string>*>(data);
					*out_text = opts[idx].c_str();
					return true;
				},
				(void*)&options, static_cast<int>(options.size()))) {
				value = static_cast<Enum>(current_item);
				changed = true;
			}

			EndProperty();

			if (changed)
				Scene::get_active_scene()->set_save_required();

			return changed;
		}

		// Style configuration functions
		static void SetPropertyStyle(const PropertyStyle& style) {
			s_property_style = style;
		}

		static PropertyStyle& GetPropertyStyle() {
			return s_property_style;
		}

		static void PushPropertyStyle() {

		}

		static void PopPropertyStyle() {
			s_property_style.label_color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
			s_property_style.value_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			s_property_style.button_color = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
			s_property_style.input_color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
			s_property_style.min_width = 150.0f;
			s_property_style.speed = 1.0f;
			s_property_style.format = "%.3f";
			s_property_style.show_reset_buttons = true;
			s_property_style.show_labels = true;
			s_property_style.drag_enabled = true;
			s_property_style.slider_enabled = false;
			s_property_style.slider_min = 0.0f;
			s_property_style.slider_max = 1.0f;
		}


	private:
		static Panels s_show_panels;
		static PropertyStyle s_property_style;
	};

}
