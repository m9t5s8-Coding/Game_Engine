#pragma once

namespace aero
{
  enum class RendererAPI
  {
    None = 0,
    OpenGl = 1
  };

  class Renderer
  {
  public:
    inline static RendererAPI get_API() { return s_renderer_API; }

  private:
    static RendererAPI s_renderer_API;
  };
}
