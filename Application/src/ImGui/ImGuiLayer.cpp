#include <imgui.h>
#include <imgui_internal.h>

#include <ImGui/ImGuiLayer.hpp>

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
  #include <backends/imgui_impl_glfw.h>
  #include <GLFW/glfw3.h>

#elif defined(PLATFORM_ANDROID)
  #include <backends/imgui_impl_android.h>
#endif

#include <backends/imgui_impl_opengl3.h>
#include <icons.h>

#include <Core/Application.hpp>
#include <Project/Assetmanager.hpp>

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

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif

#ifndef PLATFORM_ANDROID
  ImFontConfig font_cfg;
  font_cfg.PixelSnapH  = true;
  font_cfg.OversampleH = 1;
  font_cfg.OversampleV = 1;

  ImFont* main_font = load_font("fonts/OpenSans-Regular.ttf", 20.0f, &font_cfg, nullptr);

  static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};

  ImFontConfig icon_cfg;
  icon_cfg.MergeMode        = true;
  icon_cfg.PixelSnapH       = true;
  icon_cfg.OversampleH      = 1;
  icon_cfg.OversampleV      = 1;
  icon_cfg.GlyphMinAdvanceX = 14.0f;

  load_font("fonts/fa-solid-900.ttf", 16.0f, &icon_cfg, icon_ranges);

  ImFont* large_font = load_font("fonts/OpenSans-Regular.ttf", 28.0f, &font_cfg, nullptr);

  io.FontDefault = main_font;
#endif

  set_engine_theme();

  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    ImGuiStyle& style                 = ImGui::GetStyle();
    style.WindowRounding              = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
  Application& app    = Application::get();
  GLFWwindow*  window = static_cast<GLFWwindow*>(app.get_window().get_native_window());
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 450");
#endif
}

void ImGuiLayer::on_detach()
{
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
#endif

  ImGui::DestroyContext();
}

void ImGuiLayer::begin()
{
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
#endif

  ImGui::NewFrame();
}

void ImGuiLayer::end()
{
  ImGuiIO& io = ImGui::GetIO();

  ImGui::Render();

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
#endif
  }
}

ImFont* ImGuiLayer::load_font(const std::string& path,
                              float              size,
                              ImFontConfig*      config,
                              const ImWchar*     ranges)
{
  ImGuiIO& io = ImGui::GetIO();

  if (!AssetManager::is_packed(AssetManager::Domain::Engine))
  {
    return io.Fonts->AddFontFromFileTTF(path.c_str(), size, config, ranges);
  }

  static std::vector<std::vector<uint8_t>> s_font_storage;

  auto bytes = AssetManager::read_bytes(path, AssetManager::Domain::Engine);
  if (bytes.empty())
  {
    AERO_CORE_ERROR("Failed to load font from pak: {}", path);
    return nullptr;
  }

  s_font_storage.push_back(bytes);

  ImFontConfig cfg         = config ? *config : ImFontConfig{};
  cfg.FontDataOwnedByAtlas = false;  // CRITICAL

  return io.Fonts->AddFontFromMemoryTTF(s_font_storage.back().data(),
                                        s_font_storage.back().size(),
                                        size,
                                        &cfg,
                                        ranges);
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
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  // Text Colors
  colors[ImGuiCol_Text]         = Color(240, 240, 255).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(140, 120, 160).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(15, 5, 25).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(10, 5, 20).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(12, 8, 22).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(80, 40, 120).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds
  colors[ImGuiCol_FrameBg]        = Color(25, 15, 35).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(40, 25, 55).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(55, 35, 75).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(10, 5, 20).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(20, 10, 35).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(10, 5, 20, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(12, 8, 22).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(10, 5, 20).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(60, 35, 85).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(90, 50, 120).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(120, 70, 160).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(255, 0, 255).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(200, 50, 200).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(255, 100, 255).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(30, 15, 45).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(60, 30, 80).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(90, 45, 120).to_imvec4();

  // Headers
  colors[ImGuiCol_Header]        = Color(35, 20, 50).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(60, 35, 85).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(85, 50, 120).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(80, 40, 120).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(120, 60, 180).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(160, 80, 240).to_imvec4();

  // Resize grips
  colors[ImGuiCol_ResizeGrip]        = Color(100, 50, 150, 80).to_imvec4();
  colors[ImGuiCol_ResizeGripHovered] = Color(150, 80, 220, 170).to_imvec4();
  colors[ImGuiCol_ResizeGripActive]  = Color(200, 100, 255).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                       = Color(20, 10, 30).to_imvec4();
  colors[ImGuiCol_TabHovered]                = Color(50, 30, 70).to_imvec4();
  colors[ImGuiCol_TabSelected]               = Color(30, 15, 45).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline]       = Color(0, 255, 255).to_imvec4();
  colors[ImGuiCol_TabDimmed]                 = Color(15, 8, 22, 200).to_imvec4();
  colors[ImGuiCol_TabDimmedSelected]         = Color(20, 10, 30).to_imvec4();
  colors[ImGuiCol_TabDimmedSelectedOverline] = Color(0, 200, 200, 180).to_imvec4();

  // Docking
  colors[ImGuiCol_DockingPreview] = Color(200, 100, 255, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(12, 8, 22).to_imvec4();

  // Plots
  colors[ImGuiCol_PlotLines]            = Color(180, 100, 255).to_imvec4();
  colors[ImGuiCol_PlotLinesHovered]     = Color(220, 150, 255).to_imvec4();
  colors[ImGuiCol_PlotHistogram]        = Color(150, 80, 220).to_imvec4();
  colors[ImGuiCol_PlotHistogramHovered] = Color(200, 120, 255).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg]     = Color(35, 20, 50).to_imvec4();
  colors[ImGuiCol_TableBorderStrong] = Color(80, 40, 120).to_imvec4();
  colors[ImGuiCol_TableBorderLight]  = Color(50, 30, 70).to_imvec4();
  colors[ImGuiCol_TableRowBg]        = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt]     = Color(30, 15, 45, 40).to_imvec4();

  // Links & Selection
  colors[ImGuiCol_TextLink]       = Color(0, 255, 255).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(80, 40, 120, 90).to_imvec4();

  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(255, 100, 255).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(0, 255, 255).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(200, 150, 255, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(12, 8, 22, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(12, 8, 22, 180).to_imvec4();

  apply_default_style_settings(style);
}
void ImGuiLayer::set_warm_amber_theme()
{
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  // Text Colors
  colors[ImGuiCol_Text]         = Color(230, 220, 200).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(150, 130, 100).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(20, 15, 10).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(15, 10, 5).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(18, 13, 8).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(60, 45, 30).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds
  colors[ImGuiCol_FrameBg]        = Color(30, 22, 15).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(45, 35, 25).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(60, 48, 35).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(15, 10, 5).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(25, 18, 12).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(15, 10, 5, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(18, 13, 8).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(15, 10, 5).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(60, 45, 30).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(90, 70, 50).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(120, 95, 70).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(255, 180, 80).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(200, 140, 60).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(255, 180, 80).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(35, 25, 15).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(60, 45, 30).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(85, 65, 45).to_imvec4();

  // Headers
  colors[ImGuiCol_Header]        = Color(40, 30, 20).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(65, 50, 35).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(90, 70, 50).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(60, 45, 30).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(100, 75, 50).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(140, 105, 70).to_imvec4();

  // Resize grips
  colors[ImGuiCol_ResizeGrip]        = Color(100, 75, 50, 80).to_imvec4();
  colors[ImGuiCol_ResizeGripHovered] = Color(150, 115, 80, 170).to_imvec4();
  colors[ImGuiCol_ResizeGripActive]  = Color(200, 150, 100).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                       = Color(25, 18, 12).to_imvec4();
  colors[ImGuiCol_TabHovered]                = Color(55, 42, 30).to_imvec4();
  colors[ImGuiCol_TabSelected]               = Color(35, 25, 15).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline]       = Color(255, 180, 80).to_imvec4();
  colors[ImGuiCol_TabDimmed]                 = Color(20, 15, 10, 200).to_imvec4();
  colors[ImGuiCol_TabDimmedSelected]         = Color(25, 18, 12).to_imvec4();
  colors[ImGuiCol_TabDimmedSelectedOverline] = Color(200, 140, 60, 180).to_imvec4();

  // Docking
  colors[ImGuiCol_DockingPreview] = Color(200, 140, 60, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(18, 13, 8).to_imvec4();

  // Plots
  colors[ImGuiCol_PlotLines]            = Color(200, 150, 90).to_imvec4();
  colors[ImGuiCol_PlotLinesHovered]     = Color(255, 200, 120).to_imvec4();
  colors[ImGuiCol_PlotHistogram]        = Color(180, 130, 70).to_imvec4();
  colors[ImGuiCol_PlotHistogramHovered] = Color(230, 170, 100).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg]     = Color(40, 30, 20).to_imvec4();
  colors[ImGuiCol_TableBorderStrong] = Color(60, 45, 30).to_imvec4();
  colors[ImGuiCol_TableBorderLight]  = Color(40, 30, 20).to_imvec4();
  colors[ImGuiCol_TableRowBg]        = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt]     = Color(35, 25, 15, 40).to_imvec4();

  // Links & Selection
  colors[ImGuiCol_TextLink]       = Color(255, 200, 100).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(80, 60, 40, 90).to_imvec4();

  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(255, 200, 120).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(255, 180, 80).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(255, 220, 180, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(18, 13, 8, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(18, 13, 8, 180).to_imvec4();

  // Style settings
  style.WindowRounding           = 3.0f;
  style.FrameRounding            = 3.0f;
  style.GrabRounding             = 3.0f;
  style.PopupRounding            = 3.0f;
  style.TabRounding              = 3.0f;
  style.ScrollbarRounding        = 2.0f;
  style.WindowMenuButtonPosition = ImGuiDir_Right;
  style.ScrollbarSize            = 14.0f;
  style.GrabMinSize              = 12.0f;
  style.DockingSeparatorSize     = 1.0f;
  style.SeparatorTextBorderSize  = 1.0f;
  style.WindowPadding            = ImVec2(7.0f, 7.0f);
  style.FramePadding             = ImVec2(6.0f, 6.0f);
  style.ItemSpacing              = ImVec2(6.0f, 4.0f);
  style.ItemInnerSpacing         = ImVec2(4.0f, 4.0f);
  style.IndentSpacing            = 12.0f;
}
void ImGuiLayer::set_ocean_blue_theme()
{
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  // Text Colors
  colors[ImGuiCol_Text]         = Color(200, 220, 240).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(100, 130, 150).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(5, 12, 20).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(3, 8, 15).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(6, 10, 18).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(30, 50, 80).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds
  colors[ImGuiCol_FrameBg]        = Color(12, 20, 30).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(20, 35, 50).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(30, 50, 70).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(3, 8, 15).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(8, 18, 30).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(3, 8, 15, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(6, 10, 18).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(3, 8, 15).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(30, 55, 80).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(50, 85, 120).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(70, 115, 160).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(80, 200, 255).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(60, 140, 200).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(100, 180, 255).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(15, 25, 40).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(30, 50, 75).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(45, 75, 110).to_imvec4();

  // Headers
  colors[ImGuiCol_Header]        = Color(20, 35, 55).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(35, 60, 90).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(50, 85, 125).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(30, 50, 80).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(50, 85, 130).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(70, 120, 180).to_imvec4();

  // Resize grips
  colors[ImGuiCol_ResizeGrip]        = Color(60, 110, 160, 80).to_imvec4();
  colors[ImGuiCol_ResizeGripHovered] = Color(90, 160, 230, 170).to_imvec4();
  colors[ImGuiCol_ResizeGripActive]  = Color(120, 200, 255).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                       = Color(10, 18, 28).to_imvec4();
  colors[ImGuiCol_TabHovered]                = Color(30, 50, 75).to_imvec4();
  colors[ImGuiCol_TabSelected]               = Color(15, 25, 40).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline]       = Color(80, 200, 255).to_imvec4();
  colors[ImGuiCol_TabDimmed]                 = Color(5, 12, 20, 200).to_imvec4();
  colors[ImGuiCol_TabDimmedSelected]         = Color(10, 18, 28).to_imvec4();
  colors[ImGuiCol_TabDimmedSelectedOverline] = Color(60, 160, 220, 180).to_imvec4();

  // Docking
  colors[ImGuiCol_DockingPreview] = Color(80, 180, 240, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(6, 10, 18).to_imvec4();

  // Plots
  colors[ImGuiCol_PlotLines]            = Color(100, 180, 240).to_imvec4();
  colors[ImGuiCol_PlotLinesHovered]     = Color(140, 220, 255).to_imvec4();
  colors[ImGuiCol_PlotHistogram]        = Color(70, 150, 210).to_imvec4();
  colors[ImGuiCol_PlotHistogramHovered] = Color(110, 190, 255).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg]     = Color(20, 35, 55).to_imvec4();
  colors[ImGuiCol_TableBorderStrong] = Color(30, 50, 80).to_imvec4();
  colors[ImGuiCol_TableBorderLight]  = Color(15, 30, 50).to_imvec4();
  colors[ImGuiCol_TableRowBg]        = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt]     = Color(15, 25, 40, 40).to_imvec4();

  // Links & Selection
  colors[ImGuiCol_TextLink]       = Color(100, 200, 255).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(40, 80, 120, 90).to_imvec4();

  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(120, 220, 255).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(80, 200, 255).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(150, 230, 255, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(6, 10, 18, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(6, 10, 18, 180).to_imvec4();

  // Style settings
  style.WindowRounding           = 3.0f;
  style.FrameRounding            = 2.0f;
  style.GrabRounding             = 3.0f;
  style.PopupRounding            = 3.0f;
  style.TabRounding              = 3.0f;
  style.ScrollbarRounding        = 2.0f;
  style.WindowMenuButtonPosition = ImGuiDir_Right;
  style.ScrollbarSize            = 14.0f;
  style.GrabMinSize              = 12.0f;
  style.DockingSeparatorSize     = 1.0f;
  style.SeparatorTextBorderSize  = 1.0f;
  style.WindowPadding            = ImVec2(6.0f, 6.0f);
  style.FramePadding             = ImVec2(6.0f, 6.0f);
  style.ItemSpacing              = ImVec2(6.0f, 4.0f);
  style.ItemInnerSpacing         = ImVec2(4.0f, 4.0f);
  style.IndentSpacing            = 15.0f;
}
void ImGuiLayer::set_blood_red_theme()
{
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  // Text Colors
  colors[ImGuiCol_Text]         = Color(240, 200, 200).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(140, 110, 110).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(15, 5, 5).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(10, 3, 3).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(12, 6, 6).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(80, 30, 30).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds
  colors[ImGuiCol_FrameBg]        = Color(25, 12, 12).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(40, 20, 20).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(60, 30, 30).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(10, 3, 3).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(20, 8, 8).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(10, 3, 3, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(12, 6, 6).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(10, 3, 3).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(60, 25, 25).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(90, 40, 40).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(120, 55, 55).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(255, 80, 80).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(200, 60, 60).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(255, 100, 100).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(30, 12, 12).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(60, 25, 25).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(90, 40, 40).to_imvec4();

  // Headers
  colors[ImGuiCol_Header]        = Color(35, 18, 18).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(60, 30, 30).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(85, 45, 45).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(80, 30, 30).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(120, 50, 50).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(160, 70, 70).to_imvec4();

  // Resize grips
  colors[ImGuiCol_ResizeGrip]        = Color(100, 40, 40, 80).to_imvec4();
  colors[ImGuiCol_ResizeGripHovered] = Color(150, 65, 65, 170).to_imvec4();
  colors[ImGuiCol_ResizeGripActive]  = Color(200, 90, 90).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                       = Color(18, 8, 8).to_imvec4();
  colors[ImGuiCol_TabHovered]                = Color(50, 22, 22).to_imvec4();
  colors[ImGuiCol_TabSelected]               = Color(30, 12, 12).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline]       = Color(255, 80, 80).to_imvec4();
  colors[ImGuiCol_TabDimmed]                 = Color(15, 5, 5, 200).to_imvec4();
  colors[ImGuiCol_TabDimmedSelected]         = Color(18, 8, 8).to_imvec4();
  colors[ImGuiCol_TabDimmedSelectedOverline] = Color(200, 60, 60, 180).to_imvec4();

  // Docking
  colors[ImGuiCol_DockingPreview] = Color(200, 80, 80, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(12, 6, 6).to_imvec4();

  // Plots
  colors[ImGuiCol_PlotLines]            = Color(200, 100, 100).to_imvec4();
  colors[ImGuiCol_PlotLinesHovered]     = Color(255, 150, 150).to_imvec4();
  colors[ImGuiCol_PlotHistogram]        = Color(180, 80, 80).to_imvec4();
  colors[ImGuiCol_PlotHistogramHovered] = Color(230, 120, 120).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg]     = Color(35, 18, 18).to_imvec4();
  colors[ImGuiCol_TableBorderStrong] = Color(80, 30, 30).to_imvec4();
  colors[ImGuiCol_TableBorderLight]  = Color(50, 20, 20).to_imvec4();
  colors[ImGuiCol_TableRowBg]        = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt]     = Color(30, 12, 12, 40).to_imvec4();

  // Links & Selection
  colors[ImGuiCol_TextLink]       = Color(255, 120, 120).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(80, 35, 35, 90).to_imvec4();

  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(255, 150, 150).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(255, 80, 80).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(255, 180, 180, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(12, 6, 6, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(12, 6, 6, 180).to_imvec4();

  // Style settings
  style.WindowRounding           = 2.0f;
  style.FrameRounding            = 2.0f;
  style.GrabRounding             = 2.0f;
  style.PopupRounding            = 2.0f;
  style.TabRounding              = 2.0f;
  style.ScrollbarRounding        = 1.0f;
  style.WindowMenuButtonPosition = ImGuiDir_Right;
  style.ScrollbarSize            = 14.0f;
  style.GrabMinSize              = 12.0f;
  style.DockingSeparatorSize     = 1.0f;
  style.SeparatorTextBorderSize  = 1.0f;
  style.WindowPadding            = ImVec2(6.0f, 6.0f);
  style.FramePadding             = ImVec2(6.0f, 6.0f);
  style.ItemSpacing              = ImVec2(6.0f, 4.0f);
  style.ItemInnerSpacing         = ImVec2(4.0f, 4.0f);
  style.IndentSpacing            = 12.0f;
}
void ImGuiLayer::set_monochrome_slate_theme()
{
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  // Text Colors
  colors[ImGuiCol_Text]         = Color(220, 220, 220).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(120, 120, 120).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(18, 18, 18).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(12, 12, 12).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(15, 15, 15).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(60, 60, 60).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds
  colors[ImGuiCol_FrameBg]        = Color(28, 28, 28).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(45, 45, 45).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(60, 60, 60).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(12, 12, 12).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(22, 22, 22).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(12, 12, 12, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(15, 15, 15).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(12, 12, 12).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(60, 60, 60).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(90, 90, 90).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(120, 120, 120).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(180, 180, 180).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(140, 140, 140).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(190, 190, 190).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(32, 32, 32).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(55, 55, 55).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(75, 75, 75).to_imvec4();

  // Headers
  colors[ImGuiCol_Header]        = Color(38, 38, 38).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(62, 62, 62).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(82, 82, 82).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(60, 60, 60).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(100, 100, 100).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(140, 140, 140).to_imvec4();

  // Resize grips
  colors[ImGuiCol_ResizeGrip]        = Color(100, 100, 100, 80).to_imvec4();
  colors[ImGuiCol_ResizeGripHovered] = Color(150, 150, 150, 170).to_imvec4();
  colors[ImGuiCol_ResizeGripActive]  = Color(200, 200, 200).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                       = Color(22, 22, 22).to_imvec4();
  colors[ImGuiCol_TabHovered]                = Color(52, 52, 52).to_imvec4();
  colors[ImGuiCol_TabSelected]               = Color(32, 32, 32).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline]       = Color(180, 180, 180).to_imvec4();
  colors[ImGuiCol_TabDimmed]                 = Color(18, 18, 18, 200).to_imvec4();
  colors[ImGuiCol_TabDimmedSelected]         = Color(22, 22, 22).to_imvec4();
  colors[ImGuiCol_TabDimmedSelectedOverline] = Color(140, 140, 140, 180).to_imvec4();

  // Docking
  colors[ImGuiCol_DockingPreview] = Color(160, 160, 160, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(15, 15, 15).to_imvec4();

  // Plots
  colors[ImGuiCol_PlotLines]            = Color(160, 160, 160).to_imvec4();
  colors[ImGuiCol_PlotLinesHovered]     = Color(200, 200, 200).to_imvec4();
  colors[ImGuiCol_PlotHistogram]        = Color(140, 140, 140).to_imvec4();
  colors[ImGuiCol_PlotHistogramHovered] = Color(180, 180, 180).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg]     = Color(38, 38, 38).to_imvec4();
  colors[ImGuiCol_TableBorderStrong] = Color(60, 60, 60).to_imvec4();
  colors[ImGuiCol_TableBorderLight]  = Color(40, 40, 40).to_imvec4();
  colors[ImGuiCol_TableRowBg]        = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt]     = Color(32, 32, 32, 40).to_imvec4();

  // Links & Selection
  colors[ImGuiCol_TextLink]       = Color(200, 200, 200).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(70, 70, 70, 90).to_imvec4();

  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(220, 220, 220).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(180, 180, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(230, 230, 230, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(15, 15, 15, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(15, 15, 15, 180).to_imvec4();

  // Style settings
  style.WindowRounding           = 0.0f;
  style.FrameRounding            = 0.0f;
  style.GrabRounding             = 0.0f;
  style.PopupRounding            = 0.0f;
  style.TabRounding              = 0.0f;
  style.ScrollbarRounding        = 0.0f;
  style.WindowMenuButtonPosition = ImGuiDir_Right;
  style.ScrollbarSize            = 14.0f;
  style.GrabMinSize              = 12.0f;
  style.DockingSeparatorSize     = 1.0f;
  style.SeparatorTextBorderSize  = 1.0f;
  style.WindowPadding            = ImVec2(8.0f, 8.0f);
  style.FramePadding             = ImVec2(6.0f, 6.0f);
  style.ItemSpacing              = ImVec2(8.0f, 4.0f);
  style.ItemInnerSpacing         = ImVec2(6.0f, 4.0f);
  style.IndentSpacing            = 14.0f;
}
void ImGuiLayer::set_light_azure_theme()
{
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  // Text Colors
  colors[ImGuiCol_Text]         = Color(40, 50, 60).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(140, 150, 160).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(245, 248, 252).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(250, 252, 255).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(248, 250, 253).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(200, 215, 230).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds
  colors[ImGuiCol_FrameBg]        = Color(235, 240, 248).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(220, 230, 245).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(200, 215, 235).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(230, 238, 248).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(210, 225, 245).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(240, 245, 252, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(238, 243, 250).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(240, 245, 252).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(180, 195, 215).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(150, 170, 195).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(120, 145, 175).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(30, 120, 200).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(60, 140, 210).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(30, 110, 180).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(225, 235, 248).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(200, 220, 245).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(175, 200, 235).to_imvec4();

  // Headers
  colors[ImGuiCol_Header]        = Color(215, 230, 248).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(190, 215, 245).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(165, 195, 235).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(200, 215, 230).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(150, 180, 215).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(100, 150, 200).to_imvec4();

  // Resize grips
  colors[ImGuiCol_ResizeGrip]        = Color(120, 160, 200, 80).to_imvec4();
  colors[ImGuiCol_ResizeGripHovered] = Color(80, 130, 180, 170).to_imvec4();
  colors[ImGuiCol_ResizeGripActive]  = Color(40, 100, 160).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                       = Color(230, 238, 248).to_imvec4();
  colors[ImGuiCol_TabHovered]                = Color(200, 220, 245).to_imvec4();
  colors[ImGuiCol_TabSelected]               = Color(245, 248, 252).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline]       = Color(30, 120, 200).to_imvec4();
  colors[ImGuiCol_TabDimmed]                 = Color(240, 245, 250, 200).to_imvec4();
  colors[ImGuiCol_TabDimmedSelected]         = Color(235, 242, 250).to_imvec4();
  colors[ImGuiCol_TabDimmedSelectedOverline] = Color(60, 150, 220, 180).to_imvec4();

  // Docking
  colors[ImGuiCol_DockingPreview] = Color(80, 160, 230, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(242, 246, 252).to_imvec4();

  // Plots
  colors[ImGuiCol_PlotLines]            = Color(50, 130, 200).to_imvec4();
  colors[ImGuiCol_PlotLinesHovered]     = Color(20, 100, 170).to_imvec4();
  colors[ImGuiCol_PlotHistogram]        = Color(70, 150, 210).to_imvec4();
  colors[ImGuiCol_PlotHistogramHovered] = Color(40, 120, 180).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg]     = Color(215, 230, 248).to_imvec4();
  colors[ImGuiCol_TableBorderStrong] = Color(190, 210, 230).to_imvec4();
  colors[ImGuiCol_TableBorderLight]  = Color(220, 230, 245).to_imvec4();
  colors[ImGuiCol_TableRowBg]        = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt]     = Color(220, 232, 248, 40).to_imvec4();

  // Links & Selection
  colors[ImGuiCol_TextLink]       = Color(30, 110, 190).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(180, 210, 240, 90).to_imvec4();

  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(60, 150, 230).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(30, 120, 200).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(100, 180, 240, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(220, 230, 240, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(210, 220, 235, 180).to_imvec4();

  // Style settings (same as ocean blue for consistency)
  style.WindowRounding           = 3.0f;
  style.FrameRounding            = 2.0f;
  style.GrabRounding             = 3.0f;
  style.PopupRounding            = 3.0f;
  style.TabRounding              = 3.0f;
  style.ScrollbarRounding        = 2.0f;
  style.WindowMenuButtonPosition = ImGuiDir_Right;
  style.ScrollbarSize            = 14.0f;
  style.GrabMinSize              = 12.0f;
  style.DockingSeparatorSize     = 1.0f;
  style.SeparatorTextBorderSize  = 1.0f;
  style.WindowPadding            = ImVec2(6.0f, 6.0f);
  style.FramePadding             = ImVec2(6.0f, 6.0f);
  style.ItemSpacing              = ImVec2(6.0f, 4.0f);
  style.ItemInnerSpacing         = ImVec2(4.0f, 4.0f);
  style.IndentSpacing            = 15.0f;
}
void ImGuiLayer::set_sunset_orange_theme()
{
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  // Text Colors
  colors[ImGuiCol_Text]         = Color(250, 240, 230).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(150, 120, 100).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(25, 15, 10).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(20, 12, 8).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(28, 18, 12).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(100, 50, 30).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds
  colors[ImGuiCol_FrameBg]        = Color(40, 25, 15).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(60, 35, 20).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(80, 45, 25).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(20, 12, 8).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(35, 20, 12).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(20, 12, 8, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(28, 18, 12).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(20, 12, 8).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(100, 60, 35).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(140, 85, 50).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(180, 110, 65).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(255, 150, 80).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(220, 120, 60).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(255, 160, 90).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(50, 30, 18).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(80, 50, 30).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(110, 70, 40).to_imvec4();

  // Headers
  colors[ImGuiCol_Header]        = Color(60, 35, 20).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(90, 55, 30).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(120, 75, 45).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(100, 50, 30).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(140, 80, 45).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(180, 110, 65).to_imvec4();

  // Resize grips
  colors[ImGuiCol_ResizeGrip]        = Color(140, 85, 50, 80).to_imvec4();
  colors[ImGuiCol_ResizeGripHovered] = Color(200, 120, 70, 170).to_imvec4();
  colors[ImGuiCol_ResizeGripActive]  = Color(255, 150, 80).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                       = Color(35, 20, 12).to_imvec4();
  colors[ImGuiCol_TabHovered]                = Color(80, 50, 30).to_imvec4();
  colors[ImGuiCol_TabSelected]               = Color(50, 30, 18).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline]       = Color(255, 150, 80).to_imvec4();
  colors[ImGuiCol_TabDimmed]                 = Color(25, 15, 10, 200).to_imvec4();
  colors[ImGuiCol_TabDimmedSelected]         = Color(35, 20, 12).to_imvec4();
  colors[ImGuiCol_TabDimmedSelectedOverline] = Color(200, 120, 70, 180).to_imvec4();

  // Docking
  colors[ImGuiCol_DockingPreview] = Color(220, 140, 80, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(28, 18, 12).to_imvec4();

  // Plots
  colors[ImGuiCol_PlotLines]            = Color(240, 140, 80).to_imvec4();
  colors[ImGuiCol_PlotLinesHovered]     = Color(255, 180, 120).to_imvec4();
  colors[ImGuiCol_PlotHistogram]        = Color(200, 120, 70).to_imvec4();
  colors[ImGuiCol_PlotHistogramHovered] = Color(240, 160, 100).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg]     = Color(60, 35, 20).to_imvec4();
  colors[ImGuiCol_TableBorderStrong] = Color(100, 50, 30).to_imvec4();
  colors[ImGuiCol_TableBorderLight]  = Color(60, 35, 20).to_imvec4();
  colors[ImGuiCol_TableRowBg]        = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt]     = Color(50, 30, 18, 40).to_imvec4();

  // Links & Selection
  colors[ImGuiCol_TextLink]       = Color(255, 170, 100).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(100, 60, 35, 90).to_imvec4();

  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(255, 180, 110).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(255, 150, 80).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(255, 200, 140, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(28, 18, 12, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(28, 18, 12, 180).to_imvec4();

  apply_default_style_settings(style);
}
void ImGuiLayer::set_forest_green_theme()
{
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  // Text Colors
  colors[ImGuiCol_Text]         = Color(220, 240, 225).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(110, 140, 120).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(10, 20, 12).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(8, 16, 10).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(12, 22, 14).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(40, 80, 50).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds
  colors[ImGuiCol_FrameBg]        = Color(20, 35, 25).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(30, 50, 38).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(40, 65, 50).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(8, 16, 10).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(15, 30, 20).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(8, 16, 10, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(12, 22, 14).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(8, 16, 10).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(50, 90, 60).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(70, 120, 85).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(90, 150, 110).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(120, 255, 150).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(80, 180, 100).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(120, 230, 150).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(25, 40, 30).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(40, 65, 50).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(55, 90, 70).to_imvec4();

  // Headers
  colors[ImGuiCol_Header]        = Color(30, 55, 40).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(50, 85, 65).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(70, 115, 90).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(40, 80, 50).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(65, 120, 85).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(90, 160, 120).to_imvec4();

  // Resize grips
  colors[ImGuiCol_ResizeGrip]        = Color(70, 130, 90, 80).to_imvec4();
  colors[ImGuiCol_ResizeGripHovered] = Color(110, 190, 140, 170).to_imvec4();
  colors[ImGuiCol_ResizeGripActive]  = Color(140, 240, 180).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                       = Color(15, 28, 20).to_imvec4();
  colors[ImGuiCol_TabHovered]                = Color(40, 65, 50).to_imvec4();
  colors[ImGuiCol_TabSelected]               = Color(25, 40, 30).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline]       = Color(120, 255, 150).to_imvec4();
  colors[ImGuiCol_TabDimmed]                 = Color(10, 20, 12, 200).to_imvec4();
  colors[ImGuiCol_TabDimmedSelected]         = Color(15, 28, 20).to_imvec4();
  colors[ImGuiCol_TabDimmedSelectedOverline] = Color(90, 200, 120, 180).to_imvec4();

  // Docking
  colors[ImGuiCol_DockingPreview] = Color(110, 220, 150, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(12, 22, 14).to_imvec4();

  // Plots
  colors[ImGuiCol_PlotLines]            = Color(120, 220, 160).to_imvec4();
  colors[ImGuiCol_PlotLinesHovered]     = Color(160, 255, 200).to_imvec4();
  colors[ImGuiCol_PlotHistogram]        = Color(90, 180, 130).to_imvec4();
  colors[ImGuiCol_PlotHistogramHovered] = Color(130, 230, 170).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg]     = Color(30, 55, 40).to_imvec4();
  colors[ImGuiCol_TableBorderStrong] = Color(40, 80, 50).to_imvec4();
  colors[ImGuiCol_TableBorderLight]  = Color(25, 50, 35).to_imvec4();
  colors[ImGuiCol_TableRowBg]        = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt]     = Color(25, 40, 30, 40).to_imvec4();

  // Links & Selection
  colors[ImGuiCol_TextLink]       = Color(140, 255, 180).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(50, 100, 70, 90).to_imvec4();

  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(150, 255, 190).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(120, 255, 150).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(180, 255, 210, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(12, 22, 14, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(12, 22, 14, 180).to_imvec4();

  apply_default_style_settings(style);
}
void ImGuiLayer::set_purple_nebula_theme()
{
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  // Text Colors
  colors[ImGuiCol_Text]         = Color(240, 220, 255).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(140, 120, 160).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(15, 10, 25).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(12, 8, 20).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(18, 12, 28).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(80, 50, 120).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds
  colors[ImGuiCol_FrameBg]        = Color(28, 20, 40).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(42, 30, 60).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(56, 40, 80).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(12, 8, 20).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(25, 18, 40).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(12, 8, 20, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(18, 12, 28).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(12, 8, 20).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(90, 60, 130).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(120, 85, 170).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(150, 110, 210).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(200, 120, 255).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(160, 90, 220).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(200, 130, 255).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(35, 25, 50).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(55, 40, 80).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(75, 55, 110).to_imvec4();

  // Headers
  colors[ImGuiCol_Header]        = Color(45, 30, 70).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(70, 50, 105).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(95, 70, 140).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(80, 50, 120).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(120, 80, 170).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(160, 110, 220).to_imvec4();

  // Resize grips
  colors[ImGuiCol_ResizeGrip]        = Color(130, 85, 180, 80).to_imvec4();
  colors[ImGuiCol_ResizeGripHovered] = Color(180, 120, 240, 170).to_imvec4();
  colors[ImGuiCol_ResizeGripActive]  = Color(220, 150, 255).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                       = Color(22, 15, 35).to_imvec4();
  colors[ImGuiCol_TabHovered]                = Color(55, 40, 80).to_imvec4();
  colors[ImGuiCol_TabSelected]               = Color(35, 25, 50).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline]       = Color(200, 120, 255).to_imvec4();
  colors[ImGuiCol_TabDimmed]                 = Color(15, 10, 25, 200).to_imvec4();
  colors[ImGuiCol_TabDimmedSelected]         = Color(22, 15, 35).to_imvec4();
  colors[ImGuiCol_TabDimmedSelectedOverline] = Color(160, 100, 220, 180).to_imvec4();

  // Docking
  colors[ImGuiCol_DockingPreview] = Color(180, 120, 240, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(18, 12, 28).to_imvec4();

  // Plots
  colors[ImGuiCol_PlotLines]            = Color(190, 130, 240).to_imvec4();
  colors[ImGuiCol_PlotLinesHovered]     = Color(220, 170, 255).to_imvec4();
  colors[ImGuiCol_PlotHistogram]        = Color(160, 100, 210).to_imvec4();
  colors[ImGuiCol_PlotHistogramHovered] = Color(200, 140, 255).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg]     = Color(45, 30, 70).to_imvec4();
  colors[ImGuiCol_TableBorderStrong] = Color(80, 50, 120).to_imvec4();
  colors[ImGuiCol_TableBorderLight]  = Color(50, 35, 75).to_imvec4();
  colors[ImGuiCol_TableRowBg]        = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt]     = Color(35, 25, 50, 40).to_imvec4();

  // Links & Selection
  colors[ImGuiCol_TextLink]       = Color(210, 150, 255).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(80, 50, 120, 90).to_imvec4();

  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(220, 160, 255).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(200, 120, 255).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(230, 180, 255, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(18, 12, 28, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(18, 12, 28, 180).to_imvec4();

  apply_default_style_settings(style);
}
void ImGuiLayer::set_crimson_red_theme()
{
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  // Text Colors
  colors[ImGuiCol_Text]         = Color(255, 230, 230).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(160, 110, 110).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(20, 8, 8).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(16, 6, 6).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(24, 10, 10).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(100, 40, 40).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds
  colors[ImGuiCol_FrameBg]        = Color(35, 15, 15).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(50, 22, 22).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(70, 30, 30).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(16, 6, 6).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(30, 12, 12).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(16, 6, 6, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(24, 10, 10).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(16, 6, 6).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(100, 50, 50).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(140, 70, 70).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(180, 90, 90).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(255, 100, 100).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(200, 80, 80).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(255, 120, 120).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(45, 20, 20).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(70, 35, 35).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(95, 50, 50).to_imvec4();

  // Headers
  colors[ImGuiCol_Header]        = Color(55, 25, 25).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(85, 40, 40).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(115, 55, 55).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(100, 40, 40).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(150, 65, 65).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(200, 90, 90).to_imvec4();

  // Resize grips
  colors[ImGuiCol_ResizeGrip]        = Color(140, 70, 70, 80).to_imvec4();
  colors[ImGuiCol_ResizeGripHovered] = Color(200, 100, 100, 170).to_imvec4();
  colors[ImGuiCol_ResizeGripActive]  = Color(255, 130, 130).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                       = Color(30, 12, 12).to_imvec4();
  colors[ImGuiCol_TabHovered]                = Color(70, 35, 35).to_imvec4();
  colors[ImGuiCol_TabSelected]               = Color(45, 20, 20).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline]       = Color(255, 100, 100).to_imvec4();
  colors[ImGuiCol_TabDimmed]                 = Color(20, 8, 8, 200).to_imvec4();
  colors[ImGuiCol_TabDimmedSelected]         = Color(30, 12, 12).to_imvec4();
  colors[ImGuiCol_TabDimmedSelectedOverline] = Color(200, 80, 80, 180).to_imvec4();

  // Docking
  colors[ImGuiCol_DockingPreview] = Color(220, 110, 110, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(24, 10, 10).to_imvec4();

  // Plots
  colors[ImGuiCol_PlotLines]            = Color(240, 120, 120).to_imvec4();
  colors[ImGuiCol_PlotLinesHovered]     = Color(255, 160, 160).to_imvec4();
  colors[ImGuiCol_PlotHistogram]        = Color(200, 100, 100).to_imvec4();
  colors[ImGuiCol_PlotHistogramHovered] = Color(240, 140, 140).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg]     = Color(55, 25, 25).to_imvec4();
  colors[ImGuiCol_TableBorderStrong] = Color(100, 40, 40).to_imvec4();
  colors[ImGuiCol_TableBorderLight]  = Color(60, 28, 28).to_imvec4();
  colors[ImGuiCol_TableRowBg]        = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt]     = Color(45, 20, 20, 40).to_imvec4();

  // Links & Selection
  colors[ImGuiCol_TextLink]       = Color(255, 140, 140).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(100, 50, 50, 90).to_imvec4();

  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(255, 150, 150).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(255, 100, 100).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(255, 180, 180, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(24, 10, 10, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(24, 10, 10, 180).to_imvec4();

  apply_default_style_settings(style);
}
void ImGuiLayer::set_cyan_teal_theme()
{
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  // Text Colors
  colors[ImGuiCol_Text]         = Color(220, 245, 245).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(110, 145, 145).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(8, 18, 20).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(6, 14, 16).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(10, 20, 22).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(40, 80, 85).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds
  colors[ImGuiCol_FrameBg]        = Color(18, 32, 35).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(28, 48, 52).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(38, 64, 70).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(6, 14, 16).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(14, 28, 32).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(6, 14, 16, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(10, 20, 22).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(6, 14, 16).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(50, 95, 100).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(70, 130, 140).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(90, 165, 175).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(100, 230, 240).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(70, 180, 195).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(110, 230, 245).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(22, 38, 42).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(35, 60, 66).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(48, 82, 90).to_imvec4();

  // Headers
  colors[ImGuiCol_Header]        = Color(28, 52, 58).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(45, 80, 88).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(62, 108, 118).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(40, 80, 85).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(65, 120, 130).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(90, 160, 175).to_imvec4();

  // Resize grips
  colors[ImGuiCol_ResizeGrip]        = Color(75, 140, 150, 80).to_imvec4();
  colors[ImGuiCol_ResizeGripHovered] = Color(115, 200, 215, 170).to_imvec4();
  colors[ImGuiCol_ResizeGripActive]  = Color(140, 240, 255).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                       = Color(14, 26, 30).to_imvec4();
  colors[ImGuiCol_TabHovered]                = Color(35, 60, 66).to_imvec4();
  colors[ImGuiCol_TabSelected]               = Color(22, 38, 42).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline]       = Color(100, 230, 240).to_imvec4();
  colors[ImGuiCol_TabDimmed]                 = Color(8, 18, 20, 200).to_imvec4();
  colors[ImGuiCol_TabDimmedSelected]         = Color(14, 26, 30).to_imvec4();
  colors[ImGuiCol_TabDimmedSelectedOverline] = Color(75, 190, 205, 180).to_imvec4();

  // Docking
  colors[ImGuiCol_DockingPreview] = Color(110, 220, 235, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(10, 20, 22).to_imvec4();

  // Plots
  colors[ImGuiCol_PlotLines]            = Color(120, 220, 235).to_imvec4();
  colors[ImGuiCol_PlotLinesHovered]     = Color(160, 245, 255).to_imvec4();
  colors[ImGuiCol_PlotHistogram]        = Color(90, 185, 200).to_imvec4();
  colors[ImGuiCol_PlotHistogramHovered] = Color(130, 225, 240).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg]     = Color(28, 52, 58).to_imvec4();
  colors[ImGuiCol_TableBorderStrong] = Color(40, 80, 85).to_imvec4();
  colors[ImGuiCol_TableBorderLight]  = Color(24, 46, 52).to_imvec4();
  colors[ImGuiCol_TableRowBg]        = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt]     = Color(22, 38, 42, 40).to_imvec4();

  // Links & Selection
  colors[ImGuiCol_TextLink]       = Color(130, 240, 255).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(45, 90, 100, 90).to_imvec4();

  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(150, 245, 255).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(100, 230, 240).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(180, 250, 255, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(10, 20, 22, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(10, 20, 22, 180).to_imvec4();

  apply_default_style_settings(style);
}
void ImGuiLayer::set_soft_pink_theme()
{
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  // Text Colors
  colors[ImGuiCol_Text]         = Color(255, 235, 245).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(160, 130, 145).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(22, 12, 18).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(18, 10, 14).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(26, 14, 20).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(95, 55, 75).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds
  colors[ImGuiCol_FrameBg]        = Color(35, 22, 28).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(50, 32, 42).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(65, 42, 56).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(18, 10, 14).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(32, 18, 26).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(18, 10, 14, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(26, 14, 20).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(18, 10, 14).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(100, 65, 82).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(140, 90, 115).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(180, 115, 148).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(255, 150, 200).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(210, 120, 165).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(255, 165, 210).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(42, 26, 34).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(65, 40, 53).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(88, 54, 72).to_imvec4();

  // Headers
  colors[ImGuiCol_Header]        = Color(52, 32, 42).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(80, 50, 65).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(108, 68, 88).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(95, 55, 75).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(140, 85, 112).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(185, 115, 150).to_imvec4();

  // Resize grips
  colors[ImGuiCol_ResizeGrip]        = Color(145, 90, 118, 80).to_imvec4();
  colors[ImGuiCol_ResizeGripHovered] = Color(205, 130, 168, 170).to_imvec4();
  colors[ImGuiCol_ResizeGripActive]  = Color(255, 170, 215).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                       = Color(28, 16, 22).to_imvec4();
  colors[ImGuiCol_TabHovered]                = Color(65, 40, 53).to_imvec4();
  colors[ImGuiCol_TabSelected]               = Color(42, 26, 34).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline]       = Color(255, 150, 200).to_imvec4();
  colors[ImGuiCol_TabDimmed]                 = Color(22, 12, 18, 200).to_imvec4();
  colors[ImGuiCol_TabDimmedSelected]         = Color(28, 16, 22).to_imvec4();
  colors[ImGuiCol_TabDimmedSelectedOverline] = Color(210, 125, 168, 180).to_imvec4();

  // Docking
  colors[ImGuiCol_DockingPreview] = Color(225, 145, 185, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(26, 14, 20).to_imvec4();

  // Plots
  colors[ImGuiCol_PlotLines]            = Color(235, 155, 195).to_imvec4();
  colors[ImGuiCol_PlotLinesHovered]     = Color(255, 195, 225).to_imvec4();
  colors[ImGuiCol_PlotHistogram]        = Color(200, 130, 170).to_imvec4();
  colors[ImGuiCol_PlotHistogramHovered] = Color(240, 170, 210).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg]     = Color(52, 32, 42).to_imvec4();
  colors[ImGuiCol_TableBorderStrong] = Color(95, 55, 75).to_imvec4();
  colors[ImGuiCol_TableBorderLight]  = Color(58, 36, 47).to_imvec4();
  colors[ImGuiCol_TableRowBg]        = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt]     = Color(42, 26, 34, 40).to_imvec4();

  // Links & Selection
  colors[ImGuiCol_TextLink]       = Color(255, 180, 215).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(90, 55, 73, 90).to_imvec4();

  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(255, 190, 220).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(255, 150, 200).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(255, 210, 235, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(26, 14, 20, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(26, 14, 20, 180).to_imvec4();

  apply_default_style_settings(style);
}
void ImGuiLayer::set_golden_amber_theme()
{
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  // Text Colors
  colors[ImGuiCol_Text]         = Color(255, 245, 220).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(160, 145, 110).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(22, 18, 8).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(18, 14, 6).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(26, 20, 10).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(100, 80, 35).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds
  colors[ImGuiCol_FrameBg]        = Color(38, 30, 15).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(55, 45, 22).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(72, 60, 30).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(18, 14, 6).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(32, 26, 12).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(18, 14, 6, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(26, 20, 10).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(18, 14, 6).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(110, 90, 40).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(150, 125, 55).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(190, 160, 70).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(255, 210, 90).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(220, 180, 75).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(255, 215, 100).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(48, 38, 18).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(72, 58, 28).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(96, 78, 38).to_imvec4();

  // Headers
  colors[ImGuiCol_Header]        = Color(58, 46, 22).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(88, 70, 34).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(118, 94, 46).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(100, 80, 35).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(150, 120, 55).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(200, 160, 75).to_imvec4();

  // Resize grips
  colors[ImGuiCol_ResizeGrip]        = Color(155, 125, 55, 80).to_imvec4();
  colors[ImGuiCol_ResizeGripHovered] = Color(215, 175, 75, 170).to_imvec4();
  colors[ImGuiCol_ResizeGripActive]  = Color(255, 210, 90).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                       = Color(28, 22, 10).to_imvec4();
  colors[ImGuiCol_TabHovered]                = Color(72, 58, 28).to_imvec4();
  colors[ImGuiCol_TabSelected]               = Color(48, 38, 18).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline]       = Color(255, 210, 90).to_imvec4();
  colors[ImGuiCol_TabDimmed]                 = Color(22, 18, 8, 200).to_imvec4();
  colors[ImGuiCol_TabDimmedSelected]         = Color(28, 22, 10).to_imvec4();
  colors[ImGuiCol_TabDimmedSelectedOverline] = Color(210, 170, 75, 180).to_imvec4();

  // Docking
  colors[ImGuiCol_DockingPreview] = Color(230, 190, 85, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(26, 20, 10).to_imvec4();

  // Plots
  colors[ImGuiCol_PlotLines]            = Color(240, 200, 90).to_imvec4();
  colors[ImGuiCol_PlotLinesHovered]     = Color(255, 225, 120).to_imvec4();
  colors[ImGuiCol_PlotHistogram]        = Color(210, 175, 80).to_imvec4();
  colors[ImGuiCol_PlotHistogramHovered] = Color(245, 210, 110).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg]     = Color(58, 46, 22).to_imvec4();
  colors[ImGuiCol_TableBorderStrong] = Color(100, 80, 35).to_imvec4();
  colors[ImGuiCol_TableBorderLight]  = Color(64, 52, 24).to_imvec4();
  colors[ImGuiCol_TableRowBg]        = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt]     = Color(48, 38, 18, 40).to_imvec4();

  // Links & Selection
  colors[ImGuiCol_TextLink]       = Color(255, 220, 110).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(100, 80, 40, 90).to_imvec4();

  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(255, 225, 120).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(255, 210, 90).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(255, 235, 150, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(26, 20, 10, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(26, 20, 10, 180).to_imvec4();

  apply_default_style_settings(style);
}
void ImGuiLayer::set_light_lavender_theme()
{
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  // Text Colors
  colors[ImGuiCol_Text]         = Color(45, 40, 60).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(145, 135, 160).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(248, 245, 252).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(252, 250, 255).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(250, 247, 254).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(210, 200, 225).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds
  colors[ImGuiCol_FrameBg]        = Color(238, 232, 248).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(225, 215, 242).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(210, 198, 235).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(235, 228, 248).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(220, 210, 240).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(242, 238, 250, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(242, 238, 250).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(245, 242, 252).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(190, 175, 215).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(165, 145, 195).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(140, 120, 175).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(100, 70, 180).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(130, 100, 200).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(100, 70, 170).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(230, 222, 245).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(210, 198, 235).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(190, 175, 222).to_imvec4();

  // Headers
  colors[ImGuiCol_Header]        = Color(220, 210, 240).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(200, 185, 230).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(180, 165, 215).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(210, 200, 225).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(170, 150, 200).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(130, 110, 175).to_imvec4();

  // Resize grips
  colors[ImGuiCol_ResizeGrip]        = Color(150, 130, 190, 80).to_imvec4();
  colors[ImGuiCol_ResizeGripHovered] = Color(120, 95, 165, 170).to_imvec4();
  colors[ImGuiCol_ResizeGripActive]  = Color(100, 75, 150).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                       = Color(235, 228, 248).to_imvec4();
  colors[ImGuiCol_TabHovered]                = Color(210, 198, 235).to_imvec4();
  colors[ImGuiCol_TabSelected]               = Color(248, 245, 252).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline]       = Color(100, 70, 180).to_imvec4();
  colors[ImGuiCol_TabDimmed]                 = Color(242, 238, 250, 200).to_imvec4();
  colors[ImGuiCol_TabDimmedSelected]         = Color(238, 232, 248).to_imvec4();
  colors[ImGuiCol_TabDimmedSelectedOverline] = Color(130, 105, 195, 180).to_imvec4();

  // Docking
  colors[ImGuiCol_DockingPreview] = Color(150, 120, 210, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(245, 242, 252).to_imvec4();

  // Plots
  colors[ImGuiCol_PlotLines]            = Color(120, 90, 190).to_imvec4();
  colors[ImGuiCol_PlotLinesHovered]     = Color(90, 60, 160).to_imvec4();
  colors[ImGuiCol_PlotHistogram]        = Color(140, 110, 200).to_imvec4();
  colors[ImGuiCol_PlotHistogramHovered] = Color(110, 80, 170).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg]     = Color(220, 210, 240).to_imvec4();
  colors[ImGuiCol_TableBorderStrong] = Color(200, 190, 225).to_imvec4();
  colors[ImGuiCol_TableBorderLight]  = Color(225, 218, 242).to_imvec4();
  colors[ImGuiCol_TableRowBg]        = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt]     = Color(228, 220, 245, 40).to_imvec4();

  // Links & Selection
  colors[ImGuiCol_TextLink]       = Color(110, 80, 190).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(190, 175, 215, 90).to_imvec4();

  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(130, 100, 200).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(100, 70, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(160, 130, 210, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(225, 218, 240, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(220, 210, 235, 180).to_imvec4();

  apply_default_style_settings(style);
}
void ImGuiLayer::set_midnight_blue_theme()
{
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  // Text Colors
  colors[ImGuiCol_Text]         = Color(230, 240, 255).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(120, 135, 160).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(8, 12, 22).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(6, 9, 18).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(10, 14, 25).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(35, 50, 85).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds
  colors[ImGuiCol_FrameBg]        = Color(16, 22, 38).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(24, 34, 56).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(32, 46, 74).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(6, 9, 18).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(12, 18, 32).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(6, 9, 18, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(10, 14, 25).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(6, 9, 18).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(45, 65, 105).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(65, 95, 145).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(85, 125, 185).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(120, 180, 255).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(90, 140, 210).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(130, 190, 255).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(20, 28, 48).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(35, 50, 80).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(50, 72, 112).to_imvec4();

  // Headers
  colors[ImGuiCol_Header]        = Color(25, 38, 62).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(40, 60, 96).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(55, 82, 130).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(35, 50, 85).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(60, 90, 140).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(85, 130, 195).to_imvec4();

  // Resize grips
  colors[ImGuiCol_ResizeGrip]        = Color(70, 110, 170, 80).to_imvec4();
  colors[ImGuiCol_ResizeGripHovered] = Color(110, 170, 240, 170).to_imvec4();
  colors[ImGuiCol_ResizeGripActive]  = Color(140, 210, 255).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                       = Color(12, 18, 32).to_imvec4();
  colors[ImGuiCol_TabHovered]                = Color(35, 50, 80).to_imvec4();
  colors[ImGuiCol_TabSelected]               = Color(20, 28, 48).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline]       = Color(120, 180, 255).to_imvec4();
  colors[ImGuiCol_TabDimmed]                 = Color(8, 12, 22, 200).to_imvec4();
  colors[ImGuiCol_TabDimmedSelected]         = Color(12, 18, 32).to_imvec4();
  colors[ImGuiCol_TabDimmedSelectedOverline] = Color(90, 150, 220, 180).to_imvec4();

  // Docking
  colors[ImGuiCol_DockingPreview] = Color(110, 170, 240, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(10, 14, 25).to_imvec4();

  // Plots
  colors[ImGuiCol_PlotLines]            = Color(130, 190, 255).to_imvec4();
  colors[ImGuiCol_PlotLinesHovered]     = Color(170, 220, 255).to_imvec4();
  colors[ImGuiCol_PlotHistogram]        = Color(100, 160, 230).to_imvec4();
  colors[ImGuiCol_PlotHistogramHovered] = Color(140, 200, 255).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg]     = Color(25, 38, 62).to_imvec4();
  colors[ImGuiCol_TableBorderStrong] = Color(35, 50, 85).to_imvec4();
  colors[ImGuiCol_TableBorderLight]  = Color(20, 32, 54).to_imvec4();
  colors[ImGuiCol_TableRowBg]        = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt]     = Color(20, 28, 48, 40).to_imvec4();

  // Links & Selection
  colors[ImGuiCol_TextLink]       = Color(140, 200, 255).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(50, 80, 130, 90).to_imvec4();

  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(150, 210, 255).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(120, 180, 255).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(180, 230, 255, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(10, 14, 25, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(10, 14, 25, 180).to_imvec4();

  apply_default_style_settings(style);
}
void ImGuiLayer::apply_default_style_settings(ImGuiStyle& style)
{
  style.WindowRounding           = 4.0f;
  style.FrameRounding            = 4.0f;
  style.GrabRounding             = 4.0f;
  style.PopupRounding            = 6.0f;
  style.TabRounding              = 3.0f;
  style.ScrollbarRounding        = 2.0f;
  style.WindowMenuButtonPosition = ImGuiDir_Right;
  style.ScrollbarSize            = 14.0f;
  style.GrabMinSize              = 12.0f;
  style.DockingSeparatorSize     = 1.0f;
  style.SeparatorTextBorderSize  = 1.0f;
  style.WindowPadding            = ImVec2(6.0f, 6.0f);
  style.FramePadding             = ImVec2(6.0f, 6.0f);
  style.ItemSpacing              = ImVec2(6.0f, 4.0f);
  style.ItemInnerSpacing         = ImVec2(4.0f, 4.0f);
  style.IndentSpacing            = 15.0f;
}
void ImGuiLayer::set_unity_dark_theme()
{
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  // Text Colors
  colors[ImGuiCol_Text]         = Color(210, 210, 210).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(128, 128, 128).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(56, 56, 56).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(60, 60, 60).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(50, 50, 50).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(35, 35, 35).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds (Input fields, etc)
  colors[ImGuiCol_FrameBg]        = Color(42, 42, 42).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(68, 68, 68).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(75, 75, 75).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(40, 40, 40).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(42, 42, 42).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(40, 40, 40, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(48, 48, 48).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(43, 43, 43).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(85, 85, 85).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(100, 100, 100).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(120, 120, 120).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(65, 140, 235).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(65, 140, 235).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(85, 160, 255).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(70, 70, 70).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(82, 82, 82).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(90, 90, 90).to_imvec4();

  // Headers (Collapsible sections)
  colors[ImGuiCol_Header]        = Color(65, 65, 65).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(75, 75, 75).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(80, 80, 80).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(35, 35, 35).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(65, 140, 235).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(85, 160, 255).to_imvec4();

  // Resize grips
  colors[ImGuiCol_ResizeGrip]        = Color(65, 140, 235, 80).to_imvec4();
  colors[ImGuiCol_ResizeGripHovered] = Color(85, 160, 255, 170).to_imvec4();
  colors[ImGuiCol_ResizeGripActive]  = Color(105, 180, 255).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                       = Color(48, 48, 48).to_imvec4();
  colors[ImGuiCol_TabHovered]                = Color(75, 75, 75).to_imvec4();
  colors[ImGuiCol_TabSelected]               = Color(56, 56, 56).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline]       = Color(65, 140, 235).to_imvec4();
  colors[ImGuiCol_TabDimmed]                 = Color(40, 40, 40, 200).to_imvec4();
  colors[ImGuiCol_TabDimmedSelected]         = Color(48, 48, 48).to_imvec4();
  colors[ImGuiCol_TabDimmedSelectedOverline] = Color(55, 120, 215, 180).to_imvec4();

  // Docking
  colors[ImGuiCol_DockingPreview] = Color(65, 140, 235, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(50, 50, 50).to_imvec4();

  // Plots
  colors[ImGuiCol_PlotLines]            = Color(155, 155, 155).to_imvec4();
  colors[ImGuiCol_PlotLinesHovered]     = Color(255, 140, 0).to_imvec4();
  colors[ImGuiCol_PlotHistogram]        = Color(230, 180, 0).to_imvec4();
  colors[ImGuiCol_PlotHistogramHovered] = Color(255, 200, 0).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg]     = Color(48, 48, 48).to_imvec4();
  colors[ImGuiCol_TableBorderStrong] = Color(60, 60, 60).to_imvec4();
  colors[ImGuiCol_TableBorderLight]  = Color(45, 45, 45).to_imvec4();
  colors[ImGuiCol_TableRowBg]        = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt]     = Color(255, 255, 255, 15).to_imvec4();

  // Links & Selection
  colors[ImGuiCol_TextLink]       = Color(65, 140, 235).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(44, 93, 163, 90).to_imvec4();

  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(255, 200, 0).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(65, 140, 235).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(255, 255, 255, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(80, 80, 80, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(80, 80, 80, 180).to_imvec4();

  apply_default_style_settings(style);
}
void ImGuiLayer::set_unity_light_theme()
{
  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  colors[ImGuiCol_Text]         = Color(25, 25, 25).to_imvec4();
  colors[ImGuiCol_TextDisabled] = Color(100, 100, 100).to_imvec4();

  // Window Backgrounds
  colors[ImGuiCol_WindowBg] = Color(170, 170, 170).to_imvec4();
  colors[ImGuiCol_ChildBg]  = Color(180, 180, 180).to_imvec4();
  colors[ImGuiCol_PopupBg]  = Color(160, 160, 160).to_imvec4();

  // Borders
  colors[ImGuiCol_Border]       = Color(100, 100, 100).to_imvec4();
  colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

  // Frame backgrounds (Input fields, etc)
  colors[ImGuiCol_FrameBg]        = Color(200, 200, 200).to_imvec4();
  colors[ImGuiCol_FrameBgHovered] = Color(215, 215, 215).to_imvec4();
  colors[ImGuiCol_FrameBgActive]  = Color(225, 225, 225).to_imvec4();

  // Title bars
  colors[ImGuiCol_TitleBg]          = Color(150, 150, 150).to_imvec4();
  colors[ImGuiCol_TitleBgActive]    = Color(160, 160, 160).to_imvec4();
  colors[ImGuiCol_TitleBgCollapsed] = Color(150, 150, 150, 200).to_imvec4();

  // Menu & Scrollbar
  colors[ImGuiCol_MenuBarBg]            = Color(160, 160, 160).to_imvec4();
  colors[ImGuiCol_ScrollbarBg]          = Color(170, 170, 170).to_imvec4();
  colors[ImGuiCol_ScrollbarGrab]        = Color(130, 130, 130).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabHovered] = Color(110, 110, 110).to_imvec4();
  colors[ImGuiCol_ScrollbarGrabActive]  = Color(90, 90, 90).to_imvec4();

  // Interactive elements
  colors[ImGuiCol_CheckMark]        = Color(30, 120, 215).to_imvec4();
  colors[ImGuiCol_SliderGrab]       = Color(30, 120, 215).to_imvec4();
  colors[ImGuiCol_SliderGrabActive] = Color(10, 100, 195).to_imvec4();

  // Buttons
  colors[ImGuiCol_Button]        = Color(200, 200, 200).to_imvec4();
  colors[ImGuiCol_ButtonHovered] = Color(215, 215, 215).to_imvec4();
  colors[ImGuiCol_ButtonActive]  = Color(180, 180, 180).to_imvec4();

  // Headers (Collapsible sections)
  colors[ImGuiCol_Header]        = Color(150, 150, 150).to_imvec4();
  colors[ImGuiCol_HeaderHovered] = Color(170, 170, 170).to_imvec4();
  colors[ImGuiCol_HeaderActive]  = Color(185, 185, 185).to_imvec4();

  // Separator
  colors[ImGuiCol_Separator]        = Color(100, 100, 100).to_imvec4();
  colors[ImGuiCol_SeparatorHovered] = Color(30, 120, 215).to_imvec4();
  colors[ImGuiCol_SeparatorActive]  = Color(10, 100, 195).to_imvec4();

  // Tabs
  colors[ImGuiCol_Tab]                 = Color(160, 160, 160).to_imvec4();
  colors[ImGuiCol_TabHovered]          = Color(180, 180, 180).to_imvec4();
  colors[ImGuiCol_TabSelected]         = Color(170, 170, 170).to_imvec4();
  colors[ImGuiCol_TabSelectedOverline] = Color(30, 120, 215).to_imvec4();

  // Docking & Misc (slightly darker)
  colors[ImGuiCol_DockingPreview] = Color(30, 120, 215, 140).to_imvec4();
  colors[ImGuiCol_DockingEmptyBg] = Color(160, 160, 160).to_imvec4();

  // Tables
  colors[ImGuiCol_TableHeaderBg] = Color(150, 150, 150).to_imvec4();
  colors[ImGuiCol_TableRowBg]    = Color(0, 0, 0, 0).to_imvec4();
  colors[ImGuiCol_TableRowBgAlt] = Color(0, 0, 0, 25).to_imvec4();

  // Text selection / links
  colors[ImGuiCol_TextLink]       = Color(30, 120, 215).to_imvec4();
  colors[ImGuiCol_TextSelectedBg] = Color(61, 133, 224, 100).to_imvec4();
  // Special states
  colors[ImGuiCol_DragDropTarget]        = Color(255, 200, 0).to_imvec4();
  colors[ImGuiCol_NavCursor]             = Color(30, 120, 215).to_imvec4();
  colors[ImGuiCol_NavWindowingHighlight] = Color(70, 70, 70, 180).to_imvec4();
  colors[ImGuiCol_NavWindowingDimBg]     = Color(200, 200, 200, 150).to_imvec4();
  colors[ImGuiCol_ModalWindowDimBg]      = Color(200, 200, 200, 180).to_imvec4();

  // Unity-style adjustments
  style.WindowRounding           = 0.0f;
  style.FrameRounding            = 2.0f;
  style.GrabRounding             = 2.0f;
  style.PopupRounding            = 2.0f;
  style.TabRounding              = 0.0f;
  style.ScrollbarRounding        = 4.0f;
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ScrollbarSize            = 14.0f;
  style.GrabMinSize              = 10.0f;
  style.DockingSeparatorSize     = 2.0f;
  style.SeparatorTextBorderSize  = 1.0f;
  style.WindowPadding            = ImVec2(8.0f, 8.0f);
  style.FramePadding             = ImVec2(4.0f, 3.0f);
  style.ItemSpacing              = ImVec2(8.0f, 4.0f);
  style.ItemInnerSpacing         = ImVec2(4.0f, 4.0f);
  style.IndentSpacing            = 21.0f;
  style.WindowBorderSize         = 1.0f;
  style.FrameBorderSize          = 0.0f;
}

}  // namespace ag
