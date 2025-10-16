#pragma once

#include <Renderer/RenderCommand.hpp>
#include <Renderer/View.hpp>
#include <Renderer/Shader.hpp>
#include <Math/Math.hpp>

namespace ag
{
  class Renderer
  {
  public:
    static void begin_scene(const View &view);
    static void end_scene();

    static void on_window_resize(const vec2u& p_size);

    static void submit(const std::shared_ptr<Shader>& p_shader, const std::shared_ptr<VertexArray>& p_vertexarray);

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
