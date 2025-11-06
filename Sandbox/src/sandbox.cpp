#include <Sandbox2D.hpp>


class Sandbox final : public ag::Application
{
public:
  Sandbox()
  {
    
  }
  ~Sandbox() override {};

  virtual void on_create() override
  {
    ag::WindowProps props;
    props.Size = ag::vec2u(1289, 720);
    props.Title = "Aero Runtime";
    init(props);
    get().get_window().center_window();

    ag::NodeFactory::init();
    ag::Renderer::init();
    ag::LuaManager::init();

    push_layer(new ag::Sandbox2D());
  }

  virtual void on_destroy() override
  {
    ag::NodeFactory::shut_down();
    ag::Renderer2D::shut_down();
    ag::LuaManager::shut_down();
  }
};

ag::Application *ag::create_application()
{
   return new Sandbox();
}
