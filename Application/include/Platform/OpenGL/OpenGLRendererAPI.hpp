#pragma once

#include <Renderer/RendererAPI.hpp>

namespace aero
{
  class OpenGLRendererAPI : public RendererAPI
  {
  public:
    OpenGLRendererAPI() {};
    ~OpenGLRendererAPI() {};

    virtual void set_clear_color(const Color &p_clear_color) override;
    virtual void clear() override;

    virtual void set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

    virtual void init() override;

    virtual void draw_indexed(const std::shared_ptr<VertexArray>& p_vertexarray) override;

  private:
  };

}
