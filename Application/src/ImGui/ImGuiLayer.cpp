#include <ImGui/ImGuiLayer.hpp>
#include <imgui.h>
#include <imgui_internal.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <icons.h>


#include <Core/Application.hpp>

namespace ag
{
  ImGuiLayer::ImGuiLayer()
    : Layer("ImGui Layer")
  {
  }

  ImGuiLayer::~ImGuiLayer()
  {
  }

  void ImGuiLayer::on_attach()
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImFontConfig font_cfg;
    font_cfg.PixelSnapH = true;
    font_cfg.OversampleH = 1;
    font_cfg.OversampleV = 1;

    // Load main font FIRST
    ImFont* main_font = io.Fonts->AddFontFromFileTTF(
      "assets/fonts/OpenSans-Regular.ttf",
      20.0f,
      &font_cfg
    );

    // Font Awesome icon ranges - FIXED!
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    // Merge icons into main font
    ImFontConfig icon_cfg;
    icon_cfg.MergeMode = true;  // IMPORTANT: Merge into previous font
    icon_cfg.PixelSnapH = true;
    icon_cfg.OversampleH = 1;
    icon_cfg.OversampleV = 1;
    icon_cfg.GlyphMinAdvanceX = 14.0f;

    io.Fonts->AddFontFromFileTTF(
      "assets/fonts/fa-solid-900.ttf",
      16.0f,
      &icon_cfg,
      icon_ranges
    );

    // Load large font
    ImFont* large_font = io.Fonts->AddFontFromFileTTF(
      "assets/fonts/OpenSans-Regular.ttf",
      28.0f,
      &font_cfg
    );

    io.FontDefault = main_font;

    set_engine_theme();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      ImGuiStyle& style = ImGui::GetStyle();
      style.WindowRounding = 0.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    Application& app = Application::get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.get_window().get_native_window());
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
  }

  void ImGuiLayer::on_detach()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void ImGuiLayer::begin()
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  }

  void ImGuiLayer::end()
  {
    ImGuiIO& io = ImGui::GetIO();
    Application& app = Application::get();
    io.DisplaySize = ImVec2(
      (float)app.get_window().get_width(),
      (float)app.get_window().get_height()
    );

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Multi-viewport support
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }
  }

  void ImGuiLayer::set_engine_theme()
  {
    set_ocean_blue_theme();
  }

  void ImGuiLayer::on_imgui_render()
  {
  }

  void ImGuiLayer::on_event(Event& event)
  {
    if (m_block_events)
    {
      ImGuiIO& io = ImGui::GetIO();
      event.Handled |= event.is_in_category(Event_Category_Mouse) && io.WantCaptureMouse;
      event.Handled |= event.is_in_category(Event_Category_Keyboard) && io.WantCaptureKeyboard;
    }
  }



 
  void ImGuiLayer::set_cyberpunk_theme()
  {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Text Colors
    colors[ImGuiCol_Text] = Color(240, 240, 255).to_imvec4();
    colors[ImGuiCol_TextDisabled] = Color(140, 120, 160).to_imvec4();

    // Window Backgrounds
    colors[ImGuiCol_WindowBg] = Color(15, 5, 25).to_imvec4();
    colors[ImGuiCol_ChildBg] = Color(10, 5, 20).to_imvec4();
    colors[ImGuiCol_PopupBg] = Color(12, 8, 22).to_imvec4();

    // Borders
    colors[ImGuiCol_Border] = Color(80, 40, 120).to_imvec4();
    colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

    // Frame backgrounds
    colors[ImGuiCol_FrameBg] = Color(25, 15, 35).to_imvec4();
    colors[ImGuiCol_FrameBgHovered] = Color(40, 25, 55).to_imvec4();
    colors[ImGuiCol_FrameBgActive] = Color(55, 35, 75).to_imvec4();

    // Title bars
    colors[ImGuiCol_TitleBg] = Color(10, 5, 20).to_imvec4();
    colors[ImGuiCol_TitleBgActive] = Color(20, 10, 35).to_imvec4();
    colors[ImGuiCol_TitleBgCollapsed] = Color(10, 5, 20, 200).to_imvec4();

    // Menu & Scrollbar
    colors[ImGuiCol_MenuBarBg] = Color(12, 8, 22).to_imvec4();
    colors[ImGuiCol_ScrollbarBg] = Color(10, 5, 20).to_imvec4();
    colors[ImGuiCol_ScrollbarGrab] = Color(60, 35, 85).to_imvec4();
    colors[ImGuiCol_ScrollbarGrabHovered] = Color(90, 50, 120).to_imvec4();
    colors[ImGuiCol_ScrollbarGrabActive] = Color(120, 70, 160).to_imvec4();

    // Interactive elements
    colors[ImGuiCol_CheckMark] = Color(255, 0, 255).to_imvec4();
    colors[ImGuiCol_SliderGrab] = Color(200, 50, 200).to_imvec4();
    colors[ImGuiCol_SliderGrabActive] = Color(255, 100, 255).to_imvec4();

    // Buttons
    colors[ImGuiCol_Button] = Color(30, 15, 45).to_imvec4();
    colors[ImGuiCol_ButtonHovered] = Color(60, 30, 80).to_imvec4();
    colors[ImGuiCol_ButtonActive] = Color(90, 45, 120).to_imvec4();

    // Headers
    colors[ImGuiCol_Header] = Color(35, 20, 50).to_imvec4();
    colors[ImGuiCol_HeaderHovered] = Color(60, 35, 85).to_imvec4();
    colors[ImGuiCol_HeaderActive] = Color(85, 50, 120).to_imvec4();

    // Separator
    colors[ImGuiCol_Separator] = Color(80, 40, 120).to_imvec4();
    colors[ImGuiCol_SeparatorHovered] = Color(120, 60, 180).to_imvec4();
    colors[ImGuiCol_SeparatorActive] = Color(160, 80, 240).to_imvec4();

    // Resize grips
    colors[ImGuiCol_ResizeGrip] = Color(100, 50, 150, 80).to_imvec4();
    colors[ImGuiCol_ResizeGripHovered] = Color(150, 80, 220, 170).to_imvec4();
    colors[ImGuiCol_ResizeGripActive] = Color(200, 100, 255).to_imvec4();

    // Tabs
    colors[ImGuiCol_Tab] = Color(20, 10, 30).to_imvec4();
    colors[ImGuiCol_TabHovered] = Color(50, 30, 70).to_imvec4();
    colors[ImGuiCol_TabSelected] = Color(30, 15, 45).to_imvec4();
    colors[ImGuiCol_TabSelectedOverline] = Color(0, 255, 255).to_imvec4();
    colors[ImGuiCol_TabDimmed] = Color(15, 8, 22, 200).to_imvec4();
    colors[ImGuiCol_TabDimmedSelected] = Color(20, 10, 30).to_imvec4();
    colors[ImGuiCol_TabDimmedSelectedOverline] = Color(0, 200, 200, 180).to_imvec4();

    // Docking
    colors[ImGuiCol_DockingPreview] = Color(200, 100, 255, 140).to_imvec4();
    colors[ImGuiCol_DockingEmptyBg] = Color(12, 8, 22).to_imvec4();

    // Plots
    colors[ImGuiCol_PlotLines] = Color(180, 100, 255).to_imvec4();
    colors[ImGuiCol_PlotLinesHovered] = Color(220, 150, 255).to_imvec4();
    colors[ImGuiCol_PlotHistogram] = Color(150, 80, 220).to_imvec4();
    colors[ImGuiCol_PlotHistogramHovered] = Color(200, 120, 255).to_imvec4();

    // Tables
    colors[ImGuiCol_TableHeaderBg] = Color(35, 20, 50).to_imvec4();
    colors[ImGuiCol_TableBorderStrong] = Color(80, 40, 120).to_imvec4();
    colors[ImGuiCol_TableBorderLight] = Color(50, 30, 70).to_imvec4();
    colors[ImGuiCol_TableRowBg] = Color(0, 0, 0, 0).to_imvec4();
    colors[ImGuiCol_TableRowBgAlt] = Color(30, 15, 45, 40).to_imvec4();

    // Links & Selection
    colors[ImGuiCol_TextLink] = Color(0, 255, 255).to_imvec4();
    colors[ImGuiCol_TextSelectedBg] = Color(80, 40, 120, 90).to_imvec4();

    // Special states
    colors[ImGuiCol_DragDropTarget] = Color(255, 100, 255).to_imvec4();
    colors[ImGuiCol_NavCursor] = Color(0, 255, 255).to_imvec4();
    colors[ImGuiCol_NavWindowingHighlight] = Color(200, 150, 255, 180).to_imvec4();
    colors[ImGuiCol_NavWindowingDimBg] = Color(12, 8, 22, 150).to_imvec4();
    colors[ImGuiCol_ModalWindowDimBg] = Color(12, 8, 22, 180).to_imvec4();

    // Style settings
    style.WindowRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.TabRounding = 4.0f;
    style.ScrollbarRounding = 2.0f;
    style.WindowMenuButtonPosition = ImGuiDir_Right;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 12.0f;
    style.DockingSeparatorSize = 2.0f;
    style.SeparatorTextBorderSize = 1.0f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(6.0f, 6.0f);
    style.ItemSpacing = ImVec2(6.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.IndentSpacing = 14.0f;
  }
  
  void ImGuiLayer::set_warm_amber_theme()
  {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Text Colors
    colors[ImGuiCol_Text] = Color(230, 220, 200).to_imvec4();
    colors[ImGuiCol_TextDisabled] = Color(150, 130, 100).to_imvec4();

    // Window Backgrounds
    colors[ImGuiCol_WindowBg] = Color(20, 15, 10).to_imvec4();
    colors[ImGuiCol_ChildBg] = Color(15, 10, 5).to_imvec4();
    colors[ImGuiCol_PopupBg] = Color(18, 13, 8).to_imvec4();

    // Borders
    colors[ImGuiCol_Border] = Color(60, 45, 30).to_imvec4();
    colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

    // Frame backgrounds
    colors[ImGuiCol_FrameBg] = Color(30, 22, 15).to_imvec4();
    colors[ImGuiCol_FrameBgHovered] = Color(45, 35, 25).to_imvec4();
    colors[ImGuiCol_FrameBgActive] = Color(60, 48, 35).to_imvec4();

    // Title bars
    colors[ImGuiCol_TitleBg] = Color(15, 10, 5).to_imvec4();
    colors[ImGuiCol_TitleBgActive] = Color(25, 18, 12).to_imvec4();
    colors[ImGuiCol_TitleBgCollapsed] = Color(15, 10, 5, 200).to_imvec4();

    // Menu & Scrollbar
    colors[ImGuiCol_MenuBarBg] = Color(18, 13, 8).to_imvec4();
    colors[ImGuiCol_ScrollbarBg] = Color(15, 10, 5).to_imvec4();
    colors[ImGuiCol_ScrollbarGrab] = Color(60, 45, 30).to_imvec4();
    colors[ImGuiCol_ScrollbarGrabHovered] = Color(90, 70, 50).to_imvec4();
    colors[ImGuiCol_ScrollbarGrabActive] = Color(120, 95, 70).to_imvec4();

    // Interactive elements
    colors[ImGuiCol_CheckMark] = Color(255, 180, 80).to_imvec4();
    colors[ImGuiCol_SliderGrab] = Color(200, 140, 60).to_imvec4();
    colors[ImGuiCol_SliderGrabActive] = Color(255, 180, 80).to_imvec4();

    // Buttons
    colors[ImGuiCol_Button] = Color(35, 25, 15).to_imvec4();
    colors[ImGuiCol_ButtonHovered] = Color(60, 45, 30).to_imvec4();
    colors[ImGuiCol_ButtonActive] = Color(85, 65, 45).to_imvec4();

    // Headers
    colors[ImGuiCol_Header] = Color(40, 30, 20).to_imvec4();
    colors[ImGuiCol_HeaderHovered] = Color(65, 50, 35).to_imvec4();
    colors[ImGuiCol_HeaderActive] = Color(90, 70, 50).to_imvec4();

    // Separator
    colors[ImGuiCol_Separator] = Color(60, 45, 30).to_imvec4();
    colors[ImGuiCol_SeparatorHovered] = Color(100, 75, 50).to_imvec4();
    colors[ImGuiCol_SeparatorActive] = Color(140, 105, 70).to_imvec4();

    // Resize grips
    colors[ImGuiCol_ResizeGrip] = Color(100, 75, 50, 80).to_imvec4();
    colors[ImGuiCol_ResizeGripHovered] = Color(150, 115, 80, 170).to_imvec4();
    colors[ImGuiCol_ResizeGripActive] = Color(200, 150, 100).to_imvec4();

    // Tabs
    colors[ImGuiCol_Tab] = Color(25, 18, 12).to_imvec4();
    colors[ImGuiCol_TabHovered] = Color(55, 42, 30).to_imvec4();
    colors[ImGuiCol_TabSelected] = Color(35, 25, 15).to_imvec4();
    colors[ImGuiCol_TabSelectedOverline] = Color(255, 180, 80).to_imvec4();
    colors[ImGuiCol_TabDimmed] = Color(20, 15, 10, 200).to_imvec4();
    colors[ImGuiCol_TabDimmedSelected] = Color(25, 18, 12).to_imvec4();
    colors[ImGuiCol_TabDimmedSelectedOverline] = Color(200, 140, 60, 180).to_imvec4();

    // Docking
    colors[ImGuiCol_DockingPreview] = Color(200, 140, 60, 140).to_imvec4();
    colors[ImGuiCol_DockingEmptyBg] = Color(18, 13, 8).to_imvec4();

    // Plots
    colors[ImGuiCol_PlotLines] = Color(200, 150, 90).to_imvec4();
    colors[ImGuiCol_PlotLinesHovered] = Color(255, 200, 120).to_imvec4();
    colors[ImGuiCol_PlotHistogram] = Color(180, 130, 70).to_imvec4();
    colors[ImGuiCol_PlotHistogramHovered] = Color(230, 170, 100).to_imvec4();

    // Tables
    colors[ImGuiCol_TableHeaderBg] = Color(40, 30, 20).to_imvec4();
    colors[ImGuiCol_TableBorderStrong] = Color(60, 45, 30).to_imvec4();
    colors[ImGuiCol_TableBorderLight] = Color(40, 30, 20).to_imvec4();
    colors[ImGuiCol_TableRowBg] = Color(0, 0, 0, 0).to_imvec4();
    colors[ImGuiCol_TableRowBgAlt] = Color(35, 25, 15, 40).to_imvec4();

    // Links & Selection
    colors[ImGuiCol_TextLink] = Color(255, 200, 100).to_imvec4();
    colors[ImGuiCol_TextSelectedBg] = Color(80, 60, 40, 90).to_imvec4();

    // Special states
    colors[ImGuiCol_DragDropTarget] = Color(255, 200, 120).to_imvec4();
    colors[ImGuiCol_NavCursor] = Color(255, 180, 80).to_imvec4();
    colors[ImGuiCol_NavWindowingHighlight] = Color(255, 220, 180, 180).to_imvec4();
    colors[ImGuiCol_NavWindowingDimBg] = Color(18, 13, 8, 150).to_imvec4();
    colors[ImGuiCol_ModalWindowDimBg] = Color(18, 13, 8, 180).to_imvec4();

    // Style settings
    style.WindowRounding = 3.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.PopupRounding = 3.0f;
    style.TabRounding = 3.0f;
    style.ScrollbarRounding = 2.0f;
    style.WindowMenuButtonPosition = ImGuiDir_Right;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 12.0f;
    style.DockingSeparatorSize = 1.0f;
    style.SeparatorTextBorderSize = 1.0f;
    style.WindowPadding = ImVec2(7.0f, 7.0f);
    style.FramePadding = ImVec2(6.0f, 6.0f);
    style.ItemSpacing = ImVec2(6.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.IndentSpacing = 12.0f;
  }

  void ImGuiLayer::set_ocean_blue_theme()
  {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Text Colors
    colors[ImGuiCol_Text] = Color(200, 220, 240).to_imvec4();
    colors[ImGuiCol_TextDisabled] = Color(100, 130, 150).to_imvec4();

    // Window Backgrounds
    colors[ImGuiCol_WindowBg] = Color(5, 12, 20).to_imvec4();
    colors[ImGuiCol_ChildBg] = Color(3, 8, 15).to_imvec4();
    colors[ImGuiCol_PopupBg] = Color(6, 10, 18).to_imvec4();

    // Borders
    colors[ImGuiCol_Border] = Color(30, 50, 80).to_imvec4();
    colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

    // Frame backgrounds
    colors[ImGuiCol_FrameBg] = Color(12, 20, 30).to_imvec4();
    colors[ImGuiCol_FrameBgHovered] = Color(20, 35, 50).to_imvec4();
    colors[ImGuiCol_FrameBgActive] = Color(30, 50, 70).to_imvec4();

    // Title bars
    colors[ImGuiCol_TitleBg] = Color(3, 8, 15).to_imvec4();
    colors[ImGuiCol_TitleBgActive] = Color(8, 18, 30).to_imvec4();
    colors[ImGuiCol_TitleBgCollapsed] = Color(3, 8, 15, 200).to_imvec4();

    // Menu & Scrollbar
    colors[ImGuiCol_MenuBarBg] = Color(6, 10, 18).to_imvec4();
    colors[ImGuiCol_ScrollbarBg] = Color(3, 8, 15).to_imvec4();
    colors[ImGuiCol_ScrollbarGrab] = Color(30, 55, 80).to_imvec4();
    colors[ImGuiCol_ScrollbarGrabHovered] = Color(50, 85, 120).to_imvec4();
    colors[ImGuiCol_ScrollbarGrabActive] = Color(70, 115, 160).to_imvec4();

    // Interactive elements
    colors[ImGuiCol_CheckMark] = Color(80, 200, 255).to_imvec4();
    colors[ImGuiCol_SliderGrab] = Color(60, 140, 200).to_imvec4();
    colors[ImGuiCol_SliderGrabActive] = Color(100, 180, 255).to_imvec4();

    // Buttons
    colors[ImGuiCol_Button] = Color(15, 25, 40).to_imvec4();
    colors[ImGuiCol_ButtonHovered] = Color(30, 50, 75).to_imvec4();
    colors[ImGuiCol_ButtonActive] = Color(45, 75, 110).to_imvec4();

    // Headers
    colors[ImGuiCol_Header] = Color(20, 35, 55).to_imvec4();
    colors[ImGuiCol_HeaderHovered] = Color(35, 60, 90).to_imvec4();
    colors[ImGuiCol_HeaderActive] = Color(50, 85, 125).to_imvec4();

    // Separator
    colors[ImGuiCol_Separator] = Color(30, 50, 80).to_imvec4();
    colors[ImGuiCol_SeparatorHovered] = Color(50, 85, 130).to_imvec4();
    colors[ImGuiCol_SeparatorActive] = Color(70, 120, 180).to_imvec4();

    // Resize grips
    colors[ImGuiCol_ResizeGrip] = Color(60, 110, 160, 80).to_imvec4();
    colors[ImGuiCol_ResizeGripHovered] = Color(90, 160, 230, 170).to_imvec4();
    colors[ImGuiCol_ResizeGripActive] = Color(120, 200, 255).to_imvec4();

    // Tabs
    colors[ImGuiCol_Tab] = Color(10, 18, 28).to_imvec4();
    colors[ImGuiCol_TabHovered] = Color(30, 50, 75).to_imvec4();
    colors[ImGuiCol_TabSelected] = Color(15, 25, 40).to_imvec4();
    colors[ImGuiCol_TabSelectedOverline] = Color(80, 200, 255).to_imvec4();
    colors[ImGuiCol_TabDimmed] = Color(5, 12, 20, 200).to_imvec4();
    colors[ImGuiCol_TabDimmedSelected] = Color(10, 18, 28).to_imvec4();
    colors[ImGuiCol_TabDimmedSelectedOverline] = Color(60, 160, 220, 180).to_imvec4();

    // Docking
    colors[ImGuiCol_DockingPreview] = Color(80, 180, 240, 140).to_imvec4();
    colors[ImGuiCol_DockingEmptyBg] = Color(6, 10, 18).to_imvec4();

    // Plots
    colors[ImGuiCol_PlotLines] = Color(100, 180, 240).to_imvec4();
    colors[ImGuiCol_PlotLinesHovered] = Color(140, 220, 255).to_imvec4();
    colors[ImGuiCol_PlotHistogram] = Color(70, 150, 210).to_imvec4();
    colors[ImGuiCol_PlotHistogramHovered] = Color(110, 190, 255).to_imvec4();

    // Tables
    colors[ImGuiCol_TableHeaderBg] = Color(20, 35, 55).to_imvec4();
    colors[ImGuiCol_TableBorderStrong] = Color(30, 50, 80).to_imvec4();
    colors[ImGuiCol_TableBorderLight] = Color(15, 30, 50).to_imvec4();
    colors[ImGuiCol_TableRowBg] = Color(0, 0, 0, 0).to_imvec4();
    colors[ImGuiCol_TableRowBgAlt] = Color(15, 25, 40, 40).to_imvec4();

    // Links & Selection
    colors[ImGuiCol_TextLink] = Color(100, 200, 255).to_imvec4();
    colors[ImGuiCol_TextSelectedBg] = Color(40, 80, 120, 90).to_imvec4();

    // Special states
    colors[ImGuiCol_DragDropTarget] = Color(120, 220, 255).to_imvec4();
    colors[ImGuiCol_NavCursor] = Color(80, 200, 255).to_imvec4();
    colors[ImGuiCol_NavWindowingHighlight] = Color(150, 230, 255, 180).to_imvec4();
    colors[ImGuiCol_NavWindowingDimBg] = Color(6, 10, 18, 150).to_imvec4();
    colors[ImGuiCol_ModalWindowDimBg] = Color(6, 10, 18, 180).to_imvec4();

    // Style settings
    style.WindowRounding = 3.0f;
    style.FrameRounding = 2.0f;
    style.GrabRounding = 3.0f;
    style.PopupRounding = 3.0f;
    style.TabRounding = 3.0f;
    style.ScrollbarRounding = 2.0f;
    style.WindowMenuButtonPosition = ImGuiDir_Right;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 12.0f;
    style.DockingSeparatorSize = 1.0f;
    style.SeparatorTextBorderSize = 1.0f;
    style.WindowPadding = ImVec2(6.0f, 6.0f);
    style.FramePadding = ImVec2(6.0f, 6.0f);
    style.ItemSpacing = ImVec2(6.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.IndentSpacing = 15.0f;
  }
  
  void ImGuiLayer::set_blood_red_theme()
  {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Text Colors
    colors[ImGuiCol_Text] = Color(240, 200, 200).to_imvec4();
    colors[ImGuiCol_TextDisabled] = Color(140, 110, 110).to_imvec4();

    // Window Backgrounds
    colors[ImGuiCol_WindowBg] = Color(15, 5, 5).to_imvec4();
    colors[ImGuiCol_ChildBg] = Color(10, 3, 3).to_imvec4();
    colors[ImGuiCol_PopupBg] = Color(12, 6, 6).to_imvec4();

    // Borders
    colors[ImGuiCol_Border] = Color(80, 30, 30).to_imvec4();
    colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

    // Frame backgrounds
    colors[ImGuiCol_FrameBg] = Color(25, 12, 12).to_imvec4();
    colors[ImGuiCol_FrameBgHovered] = Color(40, 20, 20).to_imvec4();
    colors[ImGuiCol_FrameBgActive] = Color(60, 30, 30).to_imvec4();

    // Title bars
    colors[ImGuiCol_TitleBg] = Color(10, 3, 3).to_imvec4();
    colors[ImGuiCol_TitleBgActive] = Color(20, 8, 8).to_imvec4();
    colors[ImGuiCol_TitleBgCollapsed] = Color(10, 3, 3, 200).to_imvec4();

    // Menu & Scrollbar
    colors[ImGuiCol_MenuBarBg] = Color(12, 6, 6).to_imvec4();
    colors[ImGuiCol_ScrollbarBg] = Color(10, 3, 3).to_imvec4();
    colors[ImGuiCol_ScrollbarGrab] = Color(60, 25, 25).to_imvec4();
    colors[ImGuiCol_ScrollbarGrabHovered] = Color(90, 40, 40).to_imvec4();
    colors[ImGuiCol_ScrollbarGrabActive] = Color(120, 55, 55).to_imvec4();

    // Interactive elements
    colors[ImGuiCol_CheckMark] = Color(255, 80, 80).to_imvec4();
    colors[ImGuiCol_SliderGrab] = Color(200, 60, 60).to_imvec4();
    colors[ImGuiCol_SliderGrabActive] = Color(255, 100, 100).to_imvec4();

    // Buttons
    colors[ImGuiCol_Button] = Color(30, 12, 12).to_imvec4();
    colors[ImGuiCol_ButtonHovered] = Color(60, 25, 25).to_imvec4();
    colors[ImGuiCol_ButtonActive] = Color(90, 40, 40).to_imvec4();

    // Headers
    colors[ImGuiCol_Header] = Color(35, 18, 18).to_imvec4();
    colors[ImGuiCol_HeaderHovered] = Color(60, 30, 30).to_imvec4();
    colors[ImGuiCol_HeaderActive] = Color(85, 45, 45).to_imvec4();

    // Separator
    colors[ImGuiCol_Separator] = Color(80, 30, 30).to_imvec4();
    colors[ImGuiCol_SeparatorHovered] = Color(120, 50, 50).to_imvec4();
    colors[ImGuiCol_SeparatorActive] = Color(160, 70, 70).to_imvec4();

    // Resize grips
    colors[ImGuiCol_ResizeGrip] = Color(100, 40, 40, 80).to_imvec4();
    colors[ImGuiCol_ResizeGripHovered] = Color(150, 65, 65, 170).to_imvec4();
    colors[ImGuiCol_ResizeGripActive] = Color(200, 90, 90).to_imvec4();

    // Tabs
    colors[ImGuiCol_Tab] = Color(18, 8, 8).to_imvec4();
    colors[ImGuiCol_TabHovered] = Color(50, 22, 22).to_imvec4();
    colors[ImGuiCol_TabSelected] = Color(30, 12, 12).to_imvec4();
    colors[ImGuiCol_TabSelectedOverline] = Color(255, 80, 80).to_imvec4();
    colors[ImGuiCol_TabDimmed] = Color(15, 5, 5, 200).to_imvec4();
    colors[ImGuiCol_TabDimmedSelected] = Color(18, 8, 8).to_imvec4();
    colors[ImGuiCol_TabDimmedSelectedOverline] = Color(200, 60, 60, 180).to_imvec4();

    // Docking
    colors[ImGuiCol_DockingPreview] = Color(200, 80, 80, 140).to_imvec4();
    colors[ImGuiCol_DockingEmptyBg] = Color(12, 6, 6).to_imvec4();

    // Plots
    colors[ImGuiCol_PlotLines] = Color(200, 100, 100).to_imvec4();
    colors[ImGuiCol_PlotLinesHovered] = Color(255, 150, 150).to_imvec4();
    colors[ImGuiCol_PlotHistogram] = Color(180, 80, 80).to_imvec4();
    colors[ImGuiCol_PlotHistogramHovered] = Color(230, 120, 120).to_imvec4();

    // Tables
    colors[ImGuiCol_TableHeaderBg] = Color(35, 18, 18).to_imvec4();
    colors[ImGuiCol_TableBorderStrong] = Color(80, 30, 30).to_imvec4();
    colors[ImGuiCol_TableBorderLight] = Color(50, 20, 20).to_imvec4();
    colors[ImGuiCol_TableRowBg] = Color(0, 0, 0, 0).to_imvec4();
    colors[ImGuiCol_TableRowBgAlt] = Color(30, 12, 12, 40).to_imvec4();

    // Links & Selection
    colors[ImGuiCol_TextLink] = Color(255, 120, 120).to_imvec4();
    colors[ImGuiCol_TextSelectedBg] = Color(80, 35, 35, 90).to_imvec4();

    // Special states
    colors[ImGuiCol_DragDropTarget] = Color(255, 150, 150).to_imvec4();
    colors[ImGuiCol_NavCursor] = Color(255, 80, 80).to_imvec4();
    colors[ImGuiCol_NavWindowingHighlight] = Color(255, 180, 180, 180).to_imvec4();
    colors[ImGuiCol_NavWindowingDimBg] = Color(12, 6, 6, 150).to_imvec4();
    colors[ImGuiCol_ModalWindowDimBg] = Color(12, 6, 6, 180).to_imvec4();

    // Style settings
    style.WindowRounding = 2.0f;
    style.FrameRounding = 2.0f;
    style.GrabRounding = 2.0f;
    style.PopupRounding = 2.0f;
    style.TabRounding = 2.0f;
    style.ScrollbarRounding = 1.0f;
    style.WindowMenuButtonPosition = ImGuiDir_Right;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 12.0f;
    style.DockingSeparatorSize = 1.0f;
    style.SeparatorTextBorderSize = 1.0f;
    style.WindowPadding = ImVec2(6.0f, 6.0f);
    style.FramePadding = ImVec2(6.0f, 6.0f);
    style.ItemSpacing = ImVec2(6.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.IndentSpacing = 12.0f;
  }
  
  void ImGuiLayer::set_monochrome_slate_theme()
  {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Text Colors
    colors[ImGuiCol_Text] = Color(220, 220, 220).to_imvec4();
    colors[ImGuiCol_TextDisabled] = Color(120, 120, 120).to_imvec4();

    // Window Backgrounds
    colors[ImGuiCol_WindowBg] = Color(18, 18, 18).to_imvec4();
    colors[ImGuiCol_ChildBg] = Color(12, 12, 12).to_imvec4();
    colors[ImGuiCol_PopupBg] = Color(15, 15, 15).to_imvec4();

    // Borders
    colors[ImGuiCol_Border] = Color(60, 60, 60).to_imvec4();
    colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

    // Frame backgrounds
    colors[ImGuiCol_FrameBg] = Color(28, 28, 28).to_imvec4();
    colors[ImGuiCol_FrameBgHovered] = Color(45, 45, 45).to_imvec4();
    colors[ImGuiCol_FrameBgActive] = Color(60, 60, 60).to_imvec4();

    // Title bars
    colors[ImGuiCol_TitleBg] = Color(12, 12, 12).to_imvec4();
    colors[ImGuiCol_TitleBgActive] = Color(22, 22, 22).to_imvec4();
    colors[ImGuiCol_TitleBgCollapsed] = Color(12, 12, 12, 200).to_imvec4();

    // Menu & Scrollbar
    colors[ImGuiCol_MenuBarBg] = Color(15, 15, 15).to_imvec4();
    colors[ImGuiCol_ScrollbarBg] = Color(12, 12, 12).to_imvec4();
    colors[ImGuiCol_ScrollbarGrab] = Color(60, 60, 60).to_imvec4();
    colors[ImGuiCol_ScrollbarGrabHovered] = Color(90, 90, 90).to_imvec4();
    colors[ImGuiCol_ScrollbarGrabActive] = Color(120, 120, 120).to_imvec4();

    // Interactive elements
    colors[ImGuiCol_CheckMark] = Color(180, 180, 180).to_imvec4();
    colors[ImGuiCol_SliderGrab] = Color(140, 140, 140).to_imvec4();
    colors[ImGuiCol_SliderGrabActive] = Color(190, 190, 190).to_imvec4();

    // Buttons
    colors[ImGuiCol_Button] = Color(32, 32, 32).to_imvec4();
    colors[ImGuiCol_ButtonHovered] = Color(55, 55, 55).to_imvec4();
    colors[ImGuiCol_ButtonActive] = Color(75, 75, 75).to_imvec4();

    // Headers
    colors[ImGuiCol_Header] = Color(38, 38, 38).to_imvec4();
    colors[ImGuiCol_HeaderHovered] = Color(62, 62, 62).to_imvec4();
    colors[ImGuiCol_HeaderActive] = Color(82, 82, 82).to_imvec4();

    // Separator
    colors[ImGuiCol_Separator] = Color(60, 60, 60).to_imvec4();
    colors[ImGuiCol_SeparatorHovered] = Color(100, 100, 100).to_imvec4();
    colors[ImGuiCol_SeparatorActive] = Color(140, 140, 140).to_imvec4();

    // Resize grips
    colors[ImGuiCol_ResizeGrip] = Color(100, 100, 100, 80).to_imvec4();
    colors[ImGuiCol_ResizeGripHovered] = Color(150, 150, 150, 170).to_imvec4();
    colors[ImGuiCol_ResizeGripActive] = Color(200, 200, 200).to_imvec4();

    // Tabs
    colors[ImGuiCol_Tab] = Color(22, 22, 22).to_imvec4();
    colors[ImGuiCol_TabHovered] = Color(52, 52, 52).to_imvec4();
    colors[ImGuiCol_TabSelected] = Color(32, 32, 32).to_imvec4();
    colors[ImGuiCol_TabSelectedOverline] = Color(180, 180, 180).to_imvec4();
    colors[ImGuiCol_TabDimmed] = Color(18, 18, 18, 200).to_imvec4();
    colors[ImGuiCol_TabDimmedSelected] = Color(22, 22, 22).to_imvec4();
    colors[ImGuiCol_TabDimmedSelectedOverline] = Color(140, 140, 140, 180).to_imvec4();

    // Docking
    colors[ImGuiCol_DockingPreview] = Color(160, 160, 160, 140).to_imvec4();
    colors[ImGuiCol_DockingEmptyBg] = Color(15, 15, 15).to_imvec4();

    // Plots
    colors[ImGuiCol_PlotLines] = Color(160, 160, 160).to_imvec4();
    colors[ImGuiCol_PlotLinesHovered] = Color(200, 200, 200).to_imvec4();
    colors[ImGuiCol_PlotHistogram] = Color(140, 140, 140).to_imvec4();
    colors[ImGuiCol_PlotHistogramHovered] = Color(180, 180, 180).to_imvec4();

    // Tables
    colors[ImGuiCol_TableHeaderBg] = Color(38, 38, 38).to_imvec4();
    colors[ImGuiCol_TableBorderStrong] = Color(60, 60, 60).to_imvec4();
    colors[ImGuiCol_TableBorderLight] = Color(40, 40, 40).to_imvec4();
    colors[ImGuiCol_TableRowBg] = Color(0, 0, 0, 0).to_imvec4();
    colors[ImGuiCol_TableRowBgAlt] = Color(32, 32, 32, 40).to_imvec4();

    // Links & Selection
    colors[ImGuiCol_TextLink] = Color(200, 200, 200).to_imvec4();
    colors[ImGuiCol_TextSelectedBg] = Color(70, 70, 70, 90).to_imvec4();

    // Special states
    colors[ImGuiCol_DragDropTarget] = Color(220, 220, 220).to_imvec4();
    colors[ImGuiCol_NavCursor] = Color(180, 180, 180).to_imvec4();
    colors[ImGuiCol_NavWindowingHighlight] = Color(230, 230, 230, 180).to_imvec4();
    colors[ImGuiCol_NavWindowingDimBg] = Color(15, 15, 15, 150).to_imvec4();
    colors[ImGuiCol_ModalWindowDimBg] = Color(15, 15, 15, 180).to_imvec4();

    // Style settings
    style.WindowRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.TabRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.WindowMenuButtonPosition = ImGuiDir_Right;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 12.0f;
    style.DockingSeparatorSize = 1.0f;
    style.SeparatorTextBorderSize = 1.0f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(6.0f, 6.0f);
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
    style.IndentSpacing = 14.0f;
  }
}