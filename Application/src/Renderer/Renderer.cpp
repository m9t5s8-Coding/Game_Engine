#include <Renderer/Renderer.hpp>
#include <Platform/OpenGL/OpenGLShader.hpp>

namespace ag
{
  Renderer::SceneData* Renderer::s_scenedata = new Renderer::SceneData;
  void Renderer::begin_scene(const View& view)
  {

  }

  void Renderer::end_scene()
  {

  }

  void Renderer::init()
  {
    RenderCommand::init();
  }

  void Renderer::on_window_resize(const vec2u& p_size)
  {
    RenderCommand::set_viewport(0, 0, p_size.x, p_size.y);
  }

  void Renderer::submit(const std::shared_ptr<Shader>& p_shader, const std::shared_ptr<VertexArray>& p_vertexarray)
  {
    p_shader->bind();
    p_vertexarray->bind();
    RenderCommand::draw_indexed(p_vertexarray);
  }
}
