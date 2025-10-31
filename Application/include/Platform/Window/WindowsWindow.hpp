#pragma once

#include <Core/Windows.hpp>
#include <GLFW/glfw3.h>
#include <Renderer/GraphicsContext.hpp>
#include <Math/Math.hpp>

namespace ag
{
  class WindowsWindow final : public Window
  {
  public:
    explicit WindowsWindow(const WindowProps &props);
    ~WindowsWindow() override;

    virtual AG_uint get_width() const override { return m_win_data.size.x; }
    virtual AG_uint get_height() const override { return m_win_data.size.y; }
    virtual vec2u get_size() const override { return m_win_data.size; }
    virtual void set_size(const vec2u& size) override;

    virtual std::string get_title() const override { return m_win_data.title; }

    virtual void set_position(const vec2i& position) override;
    virtual vec2i get_position() const override;

    virtual void center_window() override;
    virtual bool is_center_window() const override { return m_win_data.is_fullscreen; }

    virtual void set_full_screen() override;
    virtual bool is_full_screen() const override { return m_win_data.is_fullscreen; }

    virtual void show_decoration(const bool show = true) override;

    void on_update() override;
    void close_window() override;

    inline void set_event_callback(const EventCallbackFunc &callback) override { m_win_data.event_callback = callback; }
    void set_vsync(bool enabled) override;
    [[nodiscard]] bool is_vsync() const override;

    virtual void* get_native_window() const { return m_Window; }

  private:
    virtual void init(const WindowProps &props);

  private:
    GLFWwindow *m_Window;

    GraphicsContext* m_context;

    struct WindowData
    {
      std::string title;
      vec2u size;
      vec2i old_size;
      vec2i old_pos;

      bool vsync;
      bool is_center_window = true;
      bool is_fullscreen = false;


      EventCallbackFunc event_callback;
    };

    WindowData m_win_data;
  };
}
