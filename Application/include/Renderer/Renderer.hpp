#pragma once

#include <Renderer/RenderCommand.hpp>
#include <Renderer/OrthographicCamera.hpp>
#include <Renderer/Shader.hpp>

namespace aero
{
  class Renderer
  {
  public:
    static void begin_scene(OrthographicCamera p_camera);
    static void end_scene();

    static void on_window_resize(uint32_t width, uint32_t height);

    static void submit(const std::shared_ptr<Shader>& p_shader, const std::shared_ptr<VertexArray>& p_vertexarray, const glm::mat4& p_transform = glm::mat4(1.0f));

    static void init();

    inline static RendererAPI::API get_API() { return RendererAPI::get_API(); }

  private:
    struct SceneData
    {
      glm::mat4 view_projection_mat;
    };
    static SceneData* s_scenedata;
  };
}
