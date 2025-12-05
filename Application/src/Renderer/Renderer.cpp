#include <Renderer/Renderer.hpp>
#include <Platform/OpenGL/OpenGLShader.hpp>
#include <Renderer/Renderer2D.hpp>

namespace ag
{
  Renderer::SceneData* Renderer::s_scenedata = new Renderer::SceneData;
  void Renderer::begin_scene(const View& view, const vec2f& viewport_size)
  {
    s_scenedata->view_matrix = view.get_view_matrix();

    vec2f size;
    if (Engine::is_runtime())
    {
      size = view.get_size();
    }
    else
    {
      size = viewport_size;
    }
    vec2f viewport_center = size / 2;
    s_scenedata->screen_matrix = Math::get_view_matrix(size, viewport_center);

    s_scenedata->scale = viewport_size / view.get_size();
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
    p_shader->set_mat3("u_screen_matrix", s_scenedata->screen_matrix);

    p_vertexarray->bind();
    RenderCommand::draw_indexed(p_vertexarray);
  }
  void Renderer::submit_instanced(const AG_ref<Shader>& p_shader, const AG_ref<VertexArray>& p_vertexarray, const AG_uint instanced_count)
  {
    p_shader->bind();
    p_shader->set_mat3("u_view_matrix", s_scenedata->view_matrix);
    p_shader->set_mat3("u_screen_matrix", s_scenedata->screen_matrix);
    p_shader->set_vec2f("u_scale", s_scenedata->scale);
    p_vertexarray->bind();
    RenderCommand::draw_instanced(p_vertexarray, instanced_count);
  }
}
