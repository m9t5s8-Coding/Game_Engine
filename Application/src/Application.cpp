#include <Application.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>

struct Application::Application_Impl
{
  GLFWwindow *window;

  static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
  {
    glViewport(0, 0, width, height);
  }
};

Application::Application()
{
  impl = std::make_unique<Application_Impl>();
}

Application::~Application()
{

}

void Application::Init(int width, int height, const char *title)
{


  glfwInit();


  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  impl->window = glfwCreateWindow(width, height, title, nullptr, nullptr);

  if (nullptr == impl->window)
  {
    glfwTerminate();
    std::cout << "Failed to create Window!" << std::endl;
    return;
  }

  glfwMakeContextCurrent(impl->window);
  glfwSetFramebufferSizeCallback(impl->window, Application_Impl::framebuffer_size_callback);

 

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD!" << std::endl;
    return;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(impl->window, true);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void Application::Run()
{
  while (!glfwWindowShouldClose(impl->window))
  {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //update and draw here

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(impl->window);
  }
}

void Application::ShutDown()
{
  if (impl->window)
  {
    glfwDestroyWindow(impl->window);
  }
  glfwTerminate();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
