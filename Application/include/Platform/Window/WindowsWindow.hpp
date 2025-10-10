#pragma once

#include <Core/Windows.hpp>
#include <GLFW/glfw3.h>
#include <Renderer/GraphicsContext.hpp>

namespace aero
{
  class WindowsWindow final : public Window
  {
  public:
    explicit WindowsWindow(const WindowProps &props);
    ~WindowsWindow() override;


    [[nodiscard]] inline unsigned int get_width() const override { return m_Data.width; }
    [[nodiscard]] inline unsigned int get_height() const override { return m_Data.height; }


    void clear_window(const aero::Color &color = aero::Color::Black) override;
    void on_update() override;
    void close_window() override;

    inline void set_event_callback(const EventCallbackFunc &callback) override { m_Data.event_callback = callback; }
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
      unsigned int width, height;

      bool vsync;

      EventCallbackFunc event_callback;
    };

    WindowData m_Data;
  };
}
