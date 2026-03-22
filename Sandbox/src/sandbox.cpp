#include <Project/SceneManager.hpp>
#include <Sandbox2D.hpp>
#ifdef PLATFORM_ANDROID
  #include <android_native_app_glue.h>
#endif

class Sandbox final : public ag::Application
{
public:
  Sandbox()
  {
  }
  ~Sandbox() override {};

  virtual void on_create() override
  {
    ag::Engine::start_runtime();

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

    push_layer(new ag::Sandbox2D());
  }

  virtual void on_destroy() override
  {
    ag::SceneManager::destroy_scene();
    ag::Project::get_active_project()->destroy();
    ag::NodeFactory::shut_down();
    ag::Renderer2D::shut_down();
    ag::ScriptManager::shut_down();
    ag::Log::flush();
  }
};

#if defined(PLATFORM_WINDOW) || defined(PLATFORM_LINUX)
ag::Application* ag::create_application()
{
  return new Sandbox();
}
#elif defined(PLATFORM_ANDROID)

ag::Application* ag::create_application(android_app* app)
{
  return new Sandbox();
}
#endif
