#include <Core/Application.hpp>
#include <iostream>
#include <glad/glad.h>

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

    glGenVertexArrays(1, &m_vertexarray);
    glBindVertexArray(m_vertexarray);

    float vertices[] = {
        0.0f, 0.5f,
        0.0f, 0.0f,
        0.5f, 0.0f};



    m_vertexbuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    unsigned int indices[] = {0, 1, 2};
    m_indexbuffer.reset(IndexBuffer::create(indices, sizeof(indices)));

    m_shader = std::make_unique<Shader>("shaders/triangle.vertex", "shaders/triangle.fragment");
  }

  Application::~Application() = default;

  void Application::run()
  {
    while (m_running)
    {
      m_Window->clear_window();

      for (Layer *layer : m_layerstack)
      {
        layer->on_update();
      }

      m_shader->use_shader();

      glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

      m_imgui_layer->begin();
      {
        for (Layer *layer : m_layerstack)
        {
          layer->on_imgui_render();
        }
      }
      m_imgui_layer->end();

      m_Window->on_update();
    }
  }

  void Application::on_event(Event &e)
  {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(on_window_close));

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
