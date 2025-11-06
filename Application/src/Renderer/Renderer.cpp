#include <Renderer/Renderer.hpp>
#include <Platform/OpenGL/OpenGLShader.hpp>
#include <Renderer/Renderer2D.hpp>

namespace ag
{
  Renderer::SceneData* Renderer::s_scenedata = new Renderer::SceneData;
  void Renderer::begin_scene(const View &view)
  {
    s_scenedata->view_matrix = view.get_view_matrix();
  }

  void Renderer::end_scene()
  {

  }

  void Renderer::init()
  {
    Renderer2D::init();
    RenderCommand::init();
  }

  void Renderer::on_window_resize(const vec2u& p_size)
  {
    RenderCommand::set_viewport(0, 0, p_size.x, p_size.y);
  }

  void Renderer::submit(const std::shared_ptr<Shader>& p_shader, const std::shared_ptr<VertexArray>& p_vertexarray)
  {
    p_shader->bind();
    p_shader->set_mat3("u_view_matrix", s_scenedata->view_matrix);
    p_vertexarray->bind();
    RenderCommand::draw_indexed(p_vertexarray);
  }
  void Renderer::submit_instanced(const AG_ref<Shader>& p_shader, const AG_ref<VertexArray>& p_vertexarray, const AG_uint instanced_count)
  {
    p_shader->bind();
    p_shader->set_mat3("u_view_matrix", s_scenedata->view_matrix);
    p_vertexarray->bind();
    RenderCommand::draw_instanced(p_vertexarray, instanced_count);
  }
}
