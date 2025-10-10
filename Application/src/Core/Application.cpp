#include <Core/Application.hpp>
#include <iostream>
#include <glad/glad.h>

#include <Renderer/BufferLayout.hpp>

namespace aero
{
  #define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

  Application *Application::s_Instance = nullptr;

  static GLenum shader_dt_to_openGL_type(ShaderDataType type)
  {
    switch (type)
    {
      case ShaderDataType::Float:   return GL_FLOAT;
      case ShaderDataType::Float2:  return GL_FLOAT;
      case ShaderDataType::Float3:  return GL_FLOAT;
      case ShaderDataType::Float4:  return GL_FLOAT;
      case ShaderDataType::Mat3:    return GL_FLOAT;
      case ShaderDataType::Mat4:    return GL_FLOAT;
      case ShaderDataType::Int:     return GL_INT;
      case ShaderDataType::Int2:    return GL_INT;
      case ShaderDataType::Int3:    return GL_INT;
      case ShaderDataType::Int4:    return GL_INT;
      case ShaderDataType::Bool:    return GL_BOOL;
    }
    AERO_CORE_ASSERT(false, "Unknown Shader Data Type!");
    return 0;
  }

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
        0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f};


    m_vertexbuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));

    {
      BufferLayout layout = {
          {ShaderDataType::Float2, "a_Position" },
          {ShaderDataType::Float4, "a_Color"} };

      m_vertexbuffer->set_layout(layout);
    }

    uint32_t index = 0;
    const auto& layout = m_vertexbuffer->get_layout();
    for (const auto &element : layout)
    {
      glEnableVertexAttribArray(index);
      glVertexAttribPointer(index, element.get_component_count(),
        shader_dt_to_openGL_type(element.type),
        element.normalized ? GL_TRUE : GL_FALSE,
        layout.get_stride(),
        reinterpret_cast<const void*>(static_cast<uintptr_t>(element.offset)));
      index++;
    }

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
