#include <Renderer/Renderer.hpp>
#include <Renderer/Renderer2D.hpp>

namespace ag
{
Renderer::SceneData* Renderer::s_scenedata = new Renderer::SceneData;
void                 Renderer::begin_scene(const View& view, const vec2f& viewport_size)
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
  vec2f viewport_center      = size / 2;
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

void Renderer::bind(AG_uint id, AG_uint p_slot)
{
  RenderCommand::bind(id, p_slot);
}

void Renderer::on_window_resize(const vec2u& p_size)
{
  RenderCommand::set_viewport(0, 0, p_size.x, p_size.y);
}

void Renderer::enable_blend()
{
  RenderCommand::enable_blend();
}
void Renderer::disable_blend()
{
  RenderCommand::disable_blend();
}

void Renderer::submit(const std::shared_ptr<VertexArray>& p_vertexarray)
{
  p_vertexarray->bind();
  RenderCommand::draw_indexed(p_vertexarray);
  p_vertexarray->unbind();
}
void Renderer::submit_instanced(const AG_ref<Shader>&      p_shader,
                                const AG_ref<VertexArray>& p_vertexarray,
                                const AG_uint              instanced_count)
{
  p_shader->bind();

#ifdef PLATFORM_ANDROID
  // ES shader uses individual sampler uniforms
  p_shader->set_int("u_texture0", 0);
  p_shader->set_int("u_texture1", 1);
  p_shader->set_int("u_texture2", 2);
#else
  int samplers[3];
  for (int i = 0; i < 3; i++)
    samplers[i] = i;
  p_shader->set_int_array("u_textures", samplers, 3);
#endif

  p_shader->set_mat3("u_view_matrix", s_scenedata->view_matrix);
  p_shader->set_mat3("u_screen_matrix", s_scenedata->screen_matrix);
  p_vertexarray->bind();
  RenderCommand::draw_instanced(p_vertexarray, instanced_count);
  p_vertexarray->unbind();
}
}  // namespace ag
