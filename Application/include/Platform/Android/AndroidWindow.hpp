#pragma once
#ifdef PLATFORM_ANDROID

  #include <android_native_app_glue.h>
  #include <EGL/egl.h>
  #include <GLES3/gl3.h>

  #include <array>
  #include <vector>

  #include <Core/Windows.hpp>
  #include <Events/Event.hpp>
  #include <Events/KeyCodes.hpp>

namespace ag
{
struct TouchPoint
{
  int   id = -1;
  vec2f position;
};

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
  // Drops only the window surface. Keeping the EGL context alive across app
  // switches avoids losing all renderer-owned GL objects on resume.
  void release_surface();
  void destroy_egl();
  bool is_egl_ready() const
  {
    return m_initialized && m_surface_ready;
  }
  bool is_surface_ready() const { return m_surface_ready; }
  // Clears the current surface to opaque black and presents it. Called on pause
  // so the app switcher snapshot is dark instead of a stale gameplay frame.
  void render_dark_frame();

  // Android event handling methods
  void    handle_android_cmd(int32_t cmd);
  int32_t handle_android_input(AInputEvent* event);

  bool is_touch_active() const
  {
    return m_touch_active;
  }
  vec2f get_touch_position() const
  {
    return m_mouse_pos;
  }

  size_t             get_touch_count() const { return m_touch_points.size(); }
  const TouchPoint*  get_touch_point(size_t index) const;
  bool               is_pinching() const { return m_pinching; }
  float              get_pinch_distance() const;
  bool               is_key_down(KeyCode key) const;
  void               clear_input_state();

private:
  void init_android_event_callbacks();
  void sync_touch_points(AInputEvent* event);
  void remove_touch_point(int id);
  void update_app_activity();
  vec2u query_surface_size() const;

  android_app*  m_app          = nullptr;
  ANativeWindow* m_native_window = nullptr;
  EGLDisplay    m_display       = EGL_NO_DISPLAY;
  EGLConfig     m_config        = nullptr;
  EGLSurface    m_surface       = EGL_NO_SURFACE;
  EGLContext    m_context       = EGL_NO_CONTEXT;
  AG_uint       m_width         = 0;
  AG_uint       m_height        = 0;
  std::string   m_title;
  EventCallbackFunc m_callback;
  bool          m_vsync         = true;
  bool          m_initialized   = false;
  bool          m_surface_ready = false;
  bool          m_app_resumed   = false;
  bool          m_app_active    = false;

  // Input state
  vec2f              m_mouse_pos          = {0.0f, 0.0f};
  bool               m_touch_active       = false;
  int                m_touch_id           = -1;
  bool               m_pinching           = false;
  float              m_last_pinch_distance = 0.0f;
  std::vector<TouchPoint> m_touch_points;
  std::array<bool, Key::Menu + 1> m_key_state{};
};

}  // namespace ag

#endif  // PLATFORM_ANDROID
