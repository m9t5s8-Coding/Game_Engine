#pragma once
#ifdef PLATFORM_ANDROID

  #include <android_native_app_glue.h>
  #include <EGL/egl.h>
  #include <GLES3/gl3.h>

  #include <Core/Windows.hpp>
  #include <Events/Event.hpp>

namespace ag
{
class AndroidWindow : public Window
{
public:
  AndroidWindow(const WindowProps& props, android_app* app);
  ~AndroidWindow() override;

  void on_update() override;
  void close_window() override;

  AG_uint get_width() const override
  {
    return m_width;
  }
  AG_uint get_height() const override
  {
    return m_height;
  }
  vec2u get_size() const override
  {
    return {m_width, m_height};
  }
  std::string get_title() const override
  {
    return m_title;
  }

  void  set_size(const vec2u& size) override;
  void  set_position(const vec2i& position) override;
  vec2u get_position() const override;
  void  show_decoration(bool show) override;
  void  center_window() override;
  bool  is_center_window() const override
  {
    return false;
  }

  void set_full_screen() override;
  bool is_full_screen() const override
  {
    return true;
  }

  void set_vsync(bool enabled) override;
  bool is_vsync() const override
  {
    return m_vsync;
  }

  void* get_native_window() const override
  {
    return m_app->window;
  }
  std::string get_clipboard_string() const override
  {
    return "";
  }

  void set_event_callback(const EventCallbackFunc& cb) override
  {
    m_callback = cb;
  }

  // EGL methods
  bool init_egl();
  void destroy_egl();
  bool is_egl_ready() const
  {
    return m_display != EGL_NO_DISPLAY;
  }

  // Android event handling methods
  void    handle_android_cmd(int32_t cmd);
  int32_t handle_android_input(AInputEvent* event);

  bool is_touch_active()
  {
    return m_touch_active;
  }
  vec2f get_touch_position()
  {
    return m_mouse_pos;
  }

private:
  void init_android_event_callbacks();

  android_app*      m_app     = nullptr;
  EGLDisplay        m_display = EGL_NO_DISPLAY;
  EGLSurface        m_surface = EGL_NO_SURFACE;
  EGLContext        m_context = EGL_NO_CONTEXT;
  AG_uint           m_width   = 0;
  AG_uint           m_height  = 0;
  std::string       m_title;
  EventCallbackFunc m_callback;
  bool              m_vsync       = true;
  bool              m_initialized = false;

  // Input state
  vec2f m_mouse_pos    = {0.0f, 0.0f};
  bool  m_touch_active = false;
  int   m_touch_id     = -1;
  bool  m_pinching     = false;
  float m_last_pinch_distance;
};

}  // namespace ag

#endif  // PLATFORM_ANDROID
