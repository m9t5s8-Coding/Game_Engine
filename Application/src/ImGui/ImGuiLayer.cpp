#include <ImGui/ImGuiLayer.hpp>
#include <imgui.h>
#include <imgui_internal.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <Core/Application.hpp>

namespace aero
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
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    Application &app = Application::get();
    GLFWwindow *window = static_cast<GLFWwindow *>(app.get_window().get_native_window());

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
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
    ImGuiIO &io = ImGui::GetIO();
    Application &app = Application::get();
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
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
  }

  void ImGuiLayer::on_imgui_render()
  {
    static bool show = true;
    ImGui::ShowDemoWindow(&show);
  }

  void ImGuiLayer::on_event(Event &event)
  {
    if (m_block_events)
    {
        ImGuiIO& io = ImGui::GetIO();
        event.Handled |= event.is_in_category(Event_Category_Mouse) && io.WantCaptureMouse;
        event.Handled |= event.is_in_category(Event_Category_Keyboard) && io.WantCaptureKeyboard;
    }
  }
}
