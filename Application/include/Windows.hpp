#pragma once

#include <Apch.hpp>
#include <Events/Event.hpp>
#include <Color.hpp>

namespace aero
{
  struct WindowProps
  {
    std::string Title;
    unsigned int Width;
    unsigned int Height;

    explicit WindowProps(std::string p_title = "Aero Engine",
                         const unsigned int p_width = 1280,
                         const unsigned int p_height = 720)
        : Title(std::move(p_title)), Width(p_width), Height(p_height) {}
  };

  class Window
  {
  public:
    using EventCallbackFunc = std::function<void(Event &)>;

    virtual ~Window() = default;

    virtual void on_update() = 0;

    [[nodiscard]] virtual unsigned int get_width() const = 0;
    [[nodiscard]] virtual unsigned int get_height() const = 0;

    virtual bool is_open() const = 0;
    virtual void clear_window(const aero::Color &color = aero::Color::Black) = 0;
    virtual void display_window() = 0;
    virtual void process_events() = 0;
    virtual void close_window() = 0;

    virtual void set_event_callback(const EventCallbackFunc &callback) = 0;
    virtual void set_vsync(bool enabled) = 0;
    [[nodiscard]] virtual bool is_vsync() const = 0;

    virtual void *get_native_window() const = 0;

    static Window *create(const WindowProps &props = WindowProps());
  };

}
