#pragma once

#include <Apch.hpp>
#include <Core/Windows.hpp>

#include <Events/KeyEvent.hpp>
#include <Events/MouseEvent.hpp>
#include <Events/WindowEvent.hpp>

#include <Layers/LayerStack.hpp>
#include <ImGui/ImGuiLayer.hpp>

#include <Core/Input.hpp>

#include <Renderer/IndexBuffer.hpp>
#include <Renderer/VertexBuffer.hpp>
#include <Renderer/VertexArray.hpp>
#include <Renderer/Shader.hpp>

#include <Renderer/View.hpp>

#include <Core/TimeStamp.hpp>

namespace ag
{
  class Application
  {
  public:
    Application();
    virtual ~Application();

    // main loop
    void run();

    void on_event(Event &e);
    void push_layer(Layer *layer);
    void push_overlay(Layer *overlay);

    inline static Application &get() { return *s_Instance; }
    inline Window &get_window() { return *m_Window; }

    ImGuiLayer *get_imgui_layer() { return m_imgui_layer; }

  private:
    bool on_window_close(WindowCloseEvent &e);
    bool on_window_resize(WindowResizeEvent &e);

    std::unique_ptr<Window> m_Window;
    bool m_running = true;
    bool m_minimized = false;

    ImGuiLayer *m_imgui_layer;
    LayerStack m_layerstack;

    float m_last_frametime = 0.0f;

  private:
    static Application *s_Instance;
  };

  Application *create_application();
}
