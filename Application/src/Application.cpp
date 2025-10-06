#include <Application.hpp>
#include <iostream>


namespace aero
{
  Application *Application::s_Instance = nullptr;

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

  Application::Application()
  {
    s_Instance = this;
    m_Window = std::unique_ptr<Window>(Window::create());
    m_Window->set_event_callback(BIND_EVENT_FN(on_event));

    m_imgui_layer = new ImGuiLayer();
    push_overlay(m_imgui_layer);
  }

  Application::~Application() = default;

  void Application::on_event(Event &e)
  {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(on_window_close));

    if(m_imgui_layer)
      m_imgui_layer->on_event(e);

    for (auto it = m_layerstack.end(); it != m_layerstack.begin();)
    {
      (*--it)->on_event(e);
      if (e.Handled)
        break;
    }
  }

  void Application::run()
  {
    while (m_Running)
    {
      m_Window->process_events();
      m_Window->clear_window();


      for (Layer *layer : m_layerstack)
      {
        layer->on_update();
      }

      m_imgui_layer->begin();
      {
        for(Layer *layer: m_layerstack)
        {
          layer->on_imgui_render();
        }
      }
      m_imgui_layer->end();


      m_Window->display_window();
    }
  }

  bool Application::on_window_close(WindowCloseEvent &e)
  {
    m_Running = false;
    return true;
  }

  void Application::push_layer(Layer *layer)
  {
    m_layerstack.push_layer(layer);
    layer->on_attach();
  }

  void Application::push_overlay(Layer *overlay)
  {
    m_layerstack.push_overlay(overlay);
    overlay->on_attach();
  }

}
