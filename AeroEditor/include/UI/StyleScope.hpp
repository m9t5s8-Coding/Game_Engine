#pragma once

#include <imgui.h>

namespace ag
{
class StyleScope
{
public:
  StyleScope() = default;

  void push_style_var(ImGuiStyleVar idx, const ImVec2& val)
  {
    ImGui::PushStyleVar(idx, val);
    m_style_var_count++;
  }

  void push_style_var(ImGuiStyleVar idx, float val)
  {
    ImGui::PushStyleVar(idx, val);
    m_style_var_count++;
  }

  void push_style_color(ImGuiCol idx, const ImVec4& col)
  {
    ImGui::PushStyleColor(idx, col);
    m_style_color_count++;
  }

  ~StyleScope()
  {
    if (m_style_color_count > 0)
      ImGui::PopStyleColor(m_style_color_count);
    if (m_style_var_count > 0)
      ImGui::PopStyleVar(m_style_var_count);
  }

private:
  int m_style_var_count   = 0;
  int m_style_color_count = 0;
};
}  // namespace ag
