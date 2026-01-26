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
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		// Text Colors - terminal green/white
		colors[ImGuiCol_Text] = Color(200, 200, 200).to_imvec4();
		colors[ImGuiCol_TextDisabled] = Color(120, 140, 120).to_imvec4();

		// Window Backgrounds - deep terminal black
		colors[ImGuiCol_WindowBg] = Color(10, 15, 10).to_imvec4();
		colors[ImGuiCol_ChildBg] = Color(5, 10, 5).to_imvec4();
		colors[ImGuiCol_PopupBg] = Color(8, 12, 8).to_imvec4();

		// Borders - subtle green tint
		colors[ImGuiCol_Border] = Color(30, 50, 30).to_imvec4();
		colors[ImGuiCol_BorderShadow] = Color(0, 0, 0, 0).to_imvec4();

		// Frame backgrounds - dark with green tint
		colors[ImGuiCol_FrameBg] = Color(18, 28, 18).to_imvec4();
		colors[ImGuiCol_FrameBgHovered] = Color(28, 45, 28).to_imvec4();
		colors[ImGuiCol_FrameBgActive] = Color(38, 60, 38).to_imvec4();

		// Title bars - terminal black
		colors[ImGuiCol_TitleBg] = Color(5, 10, 5).to_imvec4();
		colors[ImGuiCol_TitleBgActive] = Color(10, 20, 10).to_imvec4();
		colors[ImGuiCol_TitleBgCollapsed] = Color(5, 10, 5, 200).to_imvec4();

		// Menu & Scrollbar
		colors[ImGuiCol_MenuBarBg] = Color(8, 15, 8).to_imvec4();
		colors[ImGuiCol_ScrollbarBg] = Color(5, 10, 5).to_imvec4();
		colors[ImGuiCol_ScrollbarGrab] = Color(40, 65, 40).to_imvec4();
		colors[ImGuiCol_ScrollbarGrabHovered] = Color(60, 90, 60).to_imvec4();
		colors[ImGuiCol_ScrollbarGrabActive] = Color(80, 120, 80).to_imvec4();

		// Interactive elements - bright terminal green
		colors[ImGuiCol_CheckMark] = Color(100, 255, 100).to_imvec4();
		colors[ImGuiCol_SliderGrab] = Color(80, 200, 80).to_imvec4();
		colors[ImGuiCol_SliderGrabActive] = Color(120, 255, 120).to_imvec4();

		// Buttons - dark green with bright hover
		colors[ImGuiCol_Button] = Color(20, 35, 20).to_imvec4();
		colors[ImGuiCol_ButtonHovered] = Color(40, 70, 40).to_imvec4();
		colors[ImGuiCol_ButtonActive] = Color(60, 100, 60).to_imvec4();

		// Headers - subtle green shades
		colors[ImGuiCol_Header] = Color(25, 40, 25).to_imvec4();
		colors[ImGuiCol_HeaderHovered] = Color(40, 65, 40).to_imvec4();
		colors[ImGuiCol_HeaderActive] = Color(55, 85, 55).to_imvec4();

		// Separators - green accent
		colors[ImGuiCol_Button] = Color(25, 40, 25).to_imvec4();
		colors[ImGuiCol_ButtonHovered] = Color(45, 75, 45).to_imvec4();
		colors[ImGuiCol_ButtonActive] = Color(65, 105, 65).to_imvec4();

		// Resize grips - bright green
		colors[ImGuiCol_ResizeGrip] = Color(60, 120, 60, 80).to_imvec4();
		colors[ImGuiCol_ResizeGripHovered] = Color(90, 180, 90, 170).to_imvec4();
		colors[ImGuiCol_ResizeGripActive] = Color(120, 240, 120).to_imvec4();

		// Tabs - terminal style
		colors[ImGuiCol_Tab] = Color(15, 25, 15).to_imvec4();
		colors[ImGuiCol_TabHovered] = Color(35, 55, 35).to_imvec4();
		colors[ImGuiCol_TabSelected] = Color(20, 35, 20).to_imvec4();
		colors[ImGuiCol_TabSelectedOverline] = Color(100, 255, 100).to_imvec4();
		colors[ImGuiCol_TabDimmed] = Color(10, 15, 10, 200).to_imvec4();
		colors[ImGuiCol_TabDimmedSelected] = Color(15, 25, 15).to_imvec4();
		colors[ImGuiCol_TabDimmedSelectedOverline] = Color(80, 200, 80, 180).to_imvec4();

		// Docking
		colors[ImGuiCol_DockingPreview] = Color(80, 200, 80, 140).to_imvec4();
		colors[ImGuiCol_DockingEmptyBg] = Color(8, 12, 8).to_imvec4();

		// Plots - terminal green
		colors[ImGuiCol_PlotLines] = Color(120, 200, 120).to_imvec4();
		colors[ImGuiCol_PlotLinesHovered] = Color(150, 255, 150).to_imvec4();
		colors[ImGuiCol_PlotHistogram] = Color(80, 180, 80).to_imvec4();
		colors[ImGuiCol_PlotHistogramHovered] = Color(120, 240, 120).to_imvec4();

		// Tables - dark with green alternating
		colors[ImGuiCol_TableHeaderBg] = Color(25, 40, 25).to_imvec4();
		colors[ImGuiCol_TableBorderStrong] = Color(30, 50, 30).to_imvec4();
		colors[ImGuiCol_TableBorderLight] = Color(20, 35, 20).to_imvec4();
		colors[ImGuiCol_TableRowBg] = Color(0, 0, 0, 0).to_imvec4();
		colors[ImGuiCol_TableRowBgAlt] = Color(20, 35, 20, 40).to_imvec4();

		// Links & Selection
		colors[ImGuiCol_TextLink] = Color(100, 255, 100).to_imvec4();
		colors[ImGuiCol_TextSelectedBg] = Color(50, 100, 50, 90).to_imvec4();

		// Special states
		colors[ImGuiCol_DragDropTarget] = Color(150, 255, 150).to_imvec4();
		colors[ImGuiCol_NavCursor] = Color(100, 255, 100).to_imvec4();
		colors[ImGuiCol_NavWindowingHighlight] = Color(200, 255, 200, 180).to_imvec4();
		colors[ImGuiCol_NavWindowingDimBg] = Color(8, 12, 8, 150).to_imvec4();
		colors[ImGuiCol_ModalWindowDimBg] = Color(8, 12, 8, 180).to_imvec4();

		// Style settings for terminal look
		style.WindowRounding = 2.0f;
		style.FrameRounding = 2.0f;
		style.GrabRounding = 2.0f;
		style.PopupRounding = 2.0f;
		style.TabRounding = 2.0f;
		style.ScrollbarRounding = 0.0f;
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
}
