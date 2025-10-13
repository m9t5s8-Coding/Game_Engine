#pragma once

#include <Renderer/RendererAPI.hpp>
#include <cstdint>

namespace aero
{
  class RenderCommand
  {
  public:
    inline static void set_clear_color(const Color &p_clear_color) { s_renderAPI->set_clear_color(p_clear_color); }
    inline static void clear() { s_renderAPI->clear(); }

    inline static void set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { s_renderAPI->set_viewport(x, y, width, height); };

    inline static void init() { s_renderAPI->init(); };

    inline static void draw_indexed(const std::shared_ptr<VertexArray> &p_vertexarray)
    {
      s_renderAPI->draw_indexed(p_vertexarray);
    }

  private:
    static RendererAPI *s_renderAPI;
  };

}
