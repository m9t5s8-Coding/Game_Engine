#include <Application.hpp>
#include <iostream>



namespace aero
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
  Application::Application()
  {
    m_Window = std::unique_ptr<Window>(Window::create());
    m_Window->set_event_callback(BIND_EVENT_FN(on_event));
  }

  Application::~Application() = default;

  void Application::on_event(Event &e)
  {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(on_window_close));

    for( auto it = m_layerstack.end(); it != m_layerstack.begin(); )
    {
      (*--it)->on_event(e);
      if(e.Handled)
        break;
    }
  }

  void Application::run()
  {
    while (m_Running)
    {
      m_Window->process_events();
      m_Window->clear_window();
      for(Layer* layer: m_layerstack)
      {
        layer->on_update();
      }
      m_Window->display_window();
    }
  }

  bool Application::on_window_close(WindowCloseEvent& e)
  {
    m_Running = false;
    return true;
  }


  void Application::push_layer(Layer* layer)
  {
    m_layerstack.push_layer(layer);
  }

  void Application::push_overlay(Layer* overlay)
  {
    m_layerstack.push_overlay(overlay);
  }

}
