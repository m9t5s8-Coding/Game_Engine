#include <Sandbox2D.hpp>

class Sandbox final : public ag::Application
{
public:
  Sandbox()
  {
    push_layer(new Sandbox2D());

  }
  ~Sandbox() override {};
};

ag::Application *ag::create_application()
{
  return new Sandbox();
}
