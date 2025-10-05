#include <Aero.hpp>
#include <iostream>

class ExampleLayer final : public aero::Layer
{
  public:
  ExampleLayer()
  :Layer("Example") {}

  void on_update() override
  {
    AERO_INFO("ExampleLayer::Update");
  }
  void on_event(aero::Event& event) override
  {
    AERO_TRACE("{0}",event.to_string());
  }
};

class Sandbox final : public aero::Application
{
public:
  Sandbox() { push_layer(new ExampleLayer()); }
  ~Sandbox() override {};
};

aero::Application *aero::create_application()
{
  return new Sandbox();
}





