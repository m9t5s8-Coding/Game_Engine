#include <Renderer/Renderer.hpp>
#include <Platform/OpenGL/OpenGLShader.hpp>

namespace aero
{
  Renderer::SceneData* Renderer::s_scenedata = new Renderer::SceneData;
  void Renderer::begin_scene(OrthographicCamera p_camera)
  {
    s_scenedata->view_projection_mat = p_camera.get_view_projection_mat();
  }

  void Renderer::end_scene()
  {

  }

  void Renderer::init()
  {
    RenderCommand::init();
  }

  void Renderer::on_window_resize(uint32_t width, uint32_t height)
  {
    RenderCommand::set_viewport(0, 0, width, height);
  }

  void Renderer::submit(const std::shared_ptr<Shader>& p_shader, const std::shared_ptr<VertexArray>& p_vertexarray, const glm::mat4& p_transform)
  {
    p_shader->bind();
    std::dynamic_pointer_cast<OpenGLShader>(p_shader)->set_mat4("u_view_projection", s_scenedata->view_projection_mat);
    std::dynamic_pointer_cast<OpenGLShader>(p_shader)->set_mat4("u_transform", p_transform);

    p_vertexarray->bind();
    RenderCommand::draw_indexed(p_vertexarray);
  }
}
