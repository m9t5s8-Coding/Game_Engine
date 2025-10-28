#pragma once

#include <imgui.h>
#include <string>
#include <type_traits>

namespace ag::UI
{
	static float columnWidth = 180.0f;
	// ---------------------------------------
	// Utility: Uniform label/value structure
	// ---------------------------------------
	static void BeginProperty(const char* label)
	{
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%s", label);
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
	}

	static void EndProperty()
	{
		ImGui::PopItemWidth();
		ImGui::Columns(1);
		ImGui::Spacing();
	}

	// ---------------------------------------
	// Vector2 Drawer (stylish and compact)
	// ---------------------------------------
	template <typename T>
	static bool draw_vec2(const char* label, T& vec, const vec2f& reset_value = vec2f(0, 0))
	{
		ImGui::PushID(label);
		ImGui::Separator();
		BeginProperty(label);

		ImGuiDataType data_type;
		if constexpr (std::is_same_v<T, vec2f>)
			data_type = ImGuiDataType_Float;
		else if constexpr (std::is_same_v<T, vec2i>)
			data_type = ImGuiDataType_S32;
		else if constexpr (std::is_same_v<T, vec2u>)
			data_type = ImGuiDataType_U32;
		else
			static_assert(!sizeof(T*), "Unsupported vec2 type");

		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		float totalWidth = ImGui::CalcItemWidth();
		float inputWidth = (totalWidth - (buttonSize.x * 2.0f) - ImGui::GetStyle().ItemSpacing.x * 3.0f) / 2.0f;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 4, 0 });


		bool is_changed = false;
		// X
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize))
			vec.x = reset_value.x;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::PushItemWidth(inputWidth);
		if (ImGui::InputScalar("##X", data_type, &vec.x))
		{
			is_changed = true;
		}
		ImGui::PopItemWidth();

		ImGui::SameLine();

		// Y
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize))
			vec.y = reset_value.y;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::PushItemWidth(inputWidth);
		if (ImGui::InputScalar("##Y", data_type, &vec.y))
		{
			is_changed = true;
		}
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		EndProperty();
		ImGui::PopID();

		return is_changed;
	}

	// ---------------------------------------
	// Value Drawer (for single floats/ints)
	// ---------------------------------------
	template <typename T>
	static void draw_value(const char* label, T& value)
	{
		ImGui::PushID(label);
		ImGui::Separator();
		BeginProperty(label);

		ImGuiDataType data_type;
		if constexpr (std::is_same_v<T, float>)
			data_type = ImGuiDataType_Float;
		else if constexpr (std::is_same_v<T, int>)
			data_type = ImGuiDataType_S32;
		else if constexpr (std::is_same_v<T, unsigned int>)
			data_type = ImGuiDataType_U32;
		else
			static_assert(!sizeof(T*), "Unsupported value type");

		ImGui::InputScalar("##value", data_type, &value);

		EndProperty();
		ImGui::PopID();
	}

	// ---------------------------------------
	// Color Drawer
	// ---------------------------------------
	static void draw_color(const char* label, Color& color)
	{
		ImGui::PushID(label);
		BeginProperty(label);

		vec4f normalized;
		color.normalize_color(normalized);

		if (ImGui::ColorEdit4("##color", (float*)&normalized, ImGuiColorEditFlags_NoInputs))
		{
			color.r = normalized.x * 255.0f;
			color.g = normalized.y * 255.0f;
			color.b = normalized.z * 255.0f;
			color.a = normalized.w * 255.0f;
		}

		EndProperty();
		ImGui::PopID();
	}

	// ---------------------------------------
	// String Drawer
	// ---------------------------------------
	static bool draw_string(const char* label, std::string& str)
	{
		ImGui::PushID(label);
		BeginProperty(label);

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), str.c_str());
		bool is_changed = false;
		if (ImGui::InputText("##string", buffer, sizeof(buffer)))
		{
			str = std::string(buffer);
			is_changed = true;
		}

		EndProperty();
		ImGui::PopID();

		return is_changed;
	}

	// ---------------------------------------
	// Section Title (for grouping)
	// ---------------------------------------
	static void draw_title(const char* label)
	{
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::SeparatorText(label);
		ImGui::Spacing();
	}
}
