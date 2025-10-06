#include <Aero.hpp>
#include <iostream>

class ExampleLayer : public aero::Layer
{
public:
  ExampleLayer()
      : Layer("ExampleLayer") {}

  void on_update() override
  {
    if (aero::Keyboard::is_key_pressed(aero::Key::A))
    {
      AERO_TRACE("A Key is Pressed");
    }
  }
  void on_event(aero::Event &event) override
  {
    if (event.get_event_type() == aero::Event_Type::Key_Pressed)
    {
      aero::KeyPressedEvent &e = (aero::KeyPressedEvent &)event;
      if (e.get_key_code() == aero::Key::A)
        AERO_TRACE("A Key is Pressed from event");
    }
  }
};

class Sandbox final : public aero::Application
{
public:
  Sandbox()
  {
    push_overlay(new ExampleLayer());
  }
  ~Sandbox() override {};
};

aero::Application *aero::create_application()
{
  return new Sandbox();
}
