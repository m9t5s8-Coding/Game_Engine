#pragma once

#include <Apch.hpp>
#include <Windows.hpp>

#include <Events/KeyEvent.hpp>
#include <Events/MouseEvent.hpp>
#include <Events/WindowEvent.hpp>

#include <Layers/LayerStack.hpp>
#include <ImGui/ImGuiLayer.hpp>

#include <Input.hpp>

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

    inline static Application& get() { return *s_Instance; }
    inline Window& get_window() { return *m_Window; }

    ImGuiLayer* get_imgui_layer() { return m_imgui_layer; }

  private:
    bool on_window_close(WindowCloseEvent& e);

    std::unique_ptr<Window> m_Window;
    bool m_Running = true;

    ImGuiLayer* m_imgui_layer;

    LayerStack m_layerstack;



  private:
    static Application* s_Instance;
  };

  Application* create_application();
}
