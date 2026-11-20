#include <Project/SceneManager.hpp>
#include <Sandbox2D.hpp>

#include <utility>
#ifdef PLATFORM_ANDROID
  #include <android_native_app_glue.h>
#endif

class Sandbox final : public ag::Application {
public:
  explicit Sandbox(std::string project_path) : m_project_path(std::move(project_path)) {}
  ~Sandbox() override {};

  virtual void on_create() override {
#ifndef PLATFORM_ANDROID
    ag::WindowProps props;
    props.Size  = ag::vec2u(1289, 720);
    props.Title = "Aero Runtime";
    init(props);
    get().get_window().center_window();
#endif
    ag::NodeFactory::init();
    ag::Renderer::init();
    ag::ScriptManager::init();
    push_layer(new ag::Sandbox2D(m_project_path));
  }

  virtual void on_destroy() override {
    ag::SceneManager::destroy_scene();
    if (ag::Project::get_active_project())
      ag::Project::get_active_project()->destroy();
    ag::NodeFactory::shut_down();
    ag::Renderer2D::shut_down();
    ag::ScriptManager::shut_down();
    ag::Log::flush();
  }

private:
  std::string m_project_path;
};

#ifndef PLATFORM_ANDROID
ag::Application* ag::create_application() {
  return new Sandbox(ag::Application::get_project_path_argument());
}
#else
ag::Application* ag::create_application(android_app* app) { return new Sandbox(std::string()); }
#endif
