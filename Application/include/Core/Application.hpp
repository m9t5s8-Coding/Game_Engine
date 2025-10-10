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
#include <Renderer/Shader.hpp>

namespace aero
{
  class Application
  {
  public:
    Application();
    virtual ~Application();

    void run();

    void on_event(Event &e);
    void push_layer(Layer *layer);
    void push_overlay(Layer *overlay);

    inline static Application &get() { return *s_Instance; }
    inline Window &get_window() { return *m_Window; }

    ImGuiLayer *get_imgui_layer() { return m_imgui_layer; }

  private:
    std::unique_ptr<VertexBuffer> m_vertexbuffer;
    std::unique_ptr<IndexBuffer> m_indexbuffer;
    std::unique_ptr<Shader> m_shader;

    unsigned int m_vertexarray;

    bool on_window_close(WindowCloseEvent &e);

    std::unique_ptr<Window> m_Window;
    bool m_running = true;

    ImGuiLayer *m_imgui_layer;

    LayerStack m_layerstack;

  private:
    static Application *s_Instance;
  };

  Application *create_application();
}
