#include <Core/Application.hpp>
#include <iostream>

#include <Core/Core.hpp>
#include <Renderer/BufferLayout.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Renderer2D.hpp>
#include <GameObjects/NodeFactory.hpp>

#include <GLFW/glfw3.h>

namespace ag
{

  Application *Application::s_Instance = nullptr;

  Application::Application()
  {
    s_Instance = this;
  }

  Application::~Application() = default;

  void Application::init(const WindowProps& props)
  {
    m_Window = std::unique_ptr<Window>(Window::create(props));
    m_Window->set_event_callback(AERO_BIND_EVENT_FN(Application::on_event));

    m_imgui_layer = new ImGuiLayer();
    push_overlay(m_imgui_layer);
  }

  void Application::run()
  {
    on_create();
    while (m_running)
    {
      float time = static_cast<float>(glfwGetTime());
      TimeStamp timestamp = time - m_last_frametime;
      m_last_frametime = time;

      if (!m_minimized)
      {
        for (Layer *layer : m_layerstack)
          layer->on_update(timestamp);
      }
      m_imgui_layer->begin();

      for (Layer *layer : m_layerstack)
        layer->on_imgui_render();

      m_imgui_layer->end();

      m_Window->on_update();
    }
    on_destroy();
    
  }

  void Application::push_layer(Layer *layer)
  {
    m_layerstack.push_layer(layer);
    layer->on_attach();
  }
  void Application::pop_layer(Layer* layer)
  {
    m_layerstack.pop_layer(layer);
    layer->on_detach();
  }

  void Application::push_overlay(Layer *overlay)
  {
    m_layerstack.push_overlay(overlay);
    overlay->on_attach();
  }

  void Application::on_event(Event &e)
  {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(AERO_BIND_EVENT_FN(Application::on_window_close));
    dispatcher.Dispatch<WindowResizeEvent>(AERO_BIND_EVENT_FN(Application::on_window_resize));
    dispatcher.Dispatch<KeyPressedEvent>(AERO_BIND_EVENT_FN(Application::on_key_pressed));

    if (m_imgui_layer)
      m_imgui_layer->on_event(e);

    for (auto it = m_layerstack.end(); it != m_layerstack.begin();)
    {
      (*--it)->on_event(e);
      if (e.Handled)
        break;
    }
  }

  bool Application::on_window_close(WindowCloseEvent &e)
  {
    m_running = false;
    return true;
  }

  bool Application::on_window_resize(WindowResizeEvent &e)
  {
    if (e.get_width() == 0 || e.get_height() == 0)
    {
      m_minimized = true;
      return false;
    }
    m_minimized = false;
    Renderer::on_window_resize(e.get_size());
    return false;
  }
  bool Application::on_key_pressed(KeyPressedEvent &e)
  {
    if(e.get_key_code() == Key::F11)
    {
      m_Window->set_full_screen();
    }
    return false;
  }


}
