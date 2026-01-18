#pragma once

#include <Renderer/RendererAPI.hpp>
#include <cstdint>

namespace ag
{
  class RenderCommand
  {
  public:
    inline static void set_clear_color(const Color& p_clear_color) { s_renderAPI->set_clear_color(p_clear_color); }
    inline static void clear() { s_renderAPI->clear(); }

    inline static void set_viewport(AG_uint x, AG_uint y, AG_uint width, AG_uint height) { s_renderAPI->set_viewport(x, y, width, height); };

    inline static void init() { s_renderAPI->init(); };

    inline static void bind(AG_uint id, AG_uint p_slot = 1) { s_renderAPI->bind(id, p_slot); }


    inline static void draw_indexed(const std::shared_ptr<VertexArray>& p_vertexarray)
    {
      s_renderAPI->draw_indexed(p_vertexarray);
    }

    inline static void draw_instanced(const AG_ref<VertexArray>& p_vertexarray, const AG_uint instance_count)
    {
      s_renderAPI->draw_instanced(p_vertexarray, instance_count);
    }

    inline static void enable_blend()
    {
      s_renderAPI->enable_blend();
    }

    inline static void disable_blend()
    {
      s_renderAPI->disable_blend();
    }

  private:
    static RendererAPI* s_renderAPI;
  };

}
