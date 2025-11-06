#pragma once

#include <Renderer/VertexArray.hpp>
#include <Renderer/Color.hpp>
#include <memory>

namespace ag
{
  class RendererAPI
  {
  public:
    enum class API
    {
      None = 0,
      OpenGL = 1
    };
  public:
    virtual void set_clear_color(const Color &p_clear_color) = 0;
    virtual void clear() = 0;

    virtual void set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

    virtual void draw_indexed(const std::shared_ptr<VertexArray>& p_vertexarray) = 0;
    virtual void draw_instanced(const AG_ref<VertexArray> &p_vertexarray, const AG_uint instance_count) = 0;

    virtual void init() = 0;

    inline static API get_API() { return s_API; }
  private:
    static API s_API;
  };

}
