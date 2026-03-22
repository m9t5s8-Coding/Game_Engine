#pragma once

#ifdef PLATFORM_ANDROID

  #include <Renderer/RendererAPI.hpp>

namespace ag
{
class OpenGL_ESRendererAPI : public RendererAPI
{
public:
  OpenGL_ESRendererAPI()  = default;
  ~OpenGL_ESRendererAPI() = default;

  virtual void set_clear_color(const Color& p_clear_color) override;
  virtual void clear() override;

  virtual void set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

  virtual void init() override;

  virtual void bind(AG_uint id, AG_uint p_slot = 1) override;

  virtual void enable_blend() override;
  virtual void disable_blend() override;

  virtual void draw_indexed(const std::shared_ptr<VertexArray>& p_vertexarray) override;
  virtual void draw_instanced(const AG_ref<VertexArray>& p_vertexarray,
                              const AG_uint              instance_count) override;

private:
  bool m_initialized = false;
};

}  // namespace ag

#endif  // PLATFORM_ANDROID
