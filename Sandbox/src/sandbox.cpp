#include <Aero.hpp>
#include <iostream>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <Platform/OpenGL/OpenGLShader.hpp>

class ExampleLayer : public aero::Layer
{
public:
  ExampleLayer()
      : Layer("ExampleLayer"),
        m_camera_controller(1.6f),
        m_triangle_pos(0.0f)
  {
    m_vertexarray.reset(aero::VertexArray::create());

    float vertices[] = {
        -0.25f, 0.25f, 0.0f, 1.0f,  // top-left
        -0.25f, -0.25f, 0.0f, 0.0f, // bottom-left
        0.25f, -0.25f, 1.0f, 0.0f,  // bottom-right
        0.25f, 0.25f, 1.0f, 1.0f    // top-right
    };

    std::shared_ptr<aero::VertexBuffer> l_vertexbuffer;
    l_vertexbuffer.reset(aero::VertexBuffer::create(vertices, sizeof(vertices)));

    aero::BufferLayout layout = {
        {aero::ShaderDataType::Float2, "a_Position"},
        {aero::ShaderDataType::Float2, "a_Texture"}};

    l_vertexbuffer->set_layout(layout);
    m_vertexarray->add_vertex_buffer(l_vertexbuffer);

    uint32_t indices[] = {
        0, 1, 2,
        2, 3, 0};

    std::shared_ptr<aero::IndexBuffer> L_indexbuffer;
    L_indexbuffer.reset(aero::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
    m_vertexarray->set_index_buffer(L_indexbuffer);

    auto shader = m_shader_library.load("assets/shaders/texture.glsl");
    auto shader1 = m_shader_library.load("assets/shaders/rectangle.glsl");
    auto shader2 = m_shader_library.load("triangle", "shaders/triangle.vertex", "shaders/triangle.fragment");

    m_texture = aero::Texture2D::create("assets/textures/solo.jpg");
    auto gl_shader = std::dynamic_pointer_cast<aero::OpenGLShader>(shader);
    gl_shader->bind();
    gl_shader->set_int("u_texture", 0);
  }

  void on_update(aero::TimeStamp ts) override
  {
    float deltatime = ts.get_seconds();
    m_camera_controller.on_update(ts);

    aero::RenderCommand::set_clear_color(aero::Color::Black);
    aero::RenderCommand::clear();

    aero::Renderer::begin_scene(m_camera_controller.get_camera());

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_triangle_pos);
    auto l_shader = m_shader_library.get("texture");
    m_texture->bind();
    aero::Renderer::submit(l_shader, m_vertexarray, transform);

    aero::Renderer::end_scene();
  }

  void on_imgui_render() override
  {
  }

  void on_event(aero::Event &event) override
  {
    m_camera_controller.on_event(event);
  }

private:
  aero::ShaderLibrary m_shader_library;
  std::shared_ptr<aero::VertexArray> m_vertexarray;
  std::shared_ptr<aero::Texture2D> m_texture;
  aero::OrthoCameraController m_camera_controller;

  glm::vec3 m_triangle_pos;
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
