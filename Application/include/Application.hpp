#pragma once

#include <Apch.hpp>
#include <Windows.hpp>

#include <Events/KeyEvent.hpp>
#include <Events/MouseEvent.hpp>
#include <Events/WindowEvent.hpp>

#include <Layers/LayerStack.hpp>

namespace aero
{
  class Application
  {
  public:
    Application();
    virtual ~Application();

    void run();

    void on_event(Event& e);
    void push_layer(Layer* layer);
    void push_overlay(Layer* overlay);



  private:
    bool on_window_close(WindowCloseEvent& e);

    std::unique_ptr<Window> m_Window;
    bool m_Running = true;

    LayerStack m_layerstack;
  };

  Application* create_application();
}
