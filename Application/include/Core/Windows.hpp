#pragma once

#include <Apch.hpp>
#include <Events/Event.hpp>
#include <Renderer/Color.hpp>
#include <Math/Math.hpp>
#include <Core/Core.hpp>

namespace ag
{
  struct WindowProps
  {
    std::string Title;
    vec2u Size;

    explicit WindowProps(std::string p_title = "Aero Engine",
                         const vec2u p_size = vec2u(1280, 720))
        : Title(std::move(p_title)), Size(p_size) {}
  };

  class Window
  {
  public:
    using EventCallbackFunc = std::function<void(Event &)>;

    virtual ~Window() = default;


    virtual AG_uint get_width() const = 0;
    virtual AG_uint get_height() const = 0;
    virtual vec2u get_size() const = 0;

    virtual void clear_window(const ag::Color &color = ag::Color::Black) = 0;
    virtual void on_update() = 0;
    virtual void close_window() = 0;

    virtual void set_event_callback(const EventCallbackFunc &callback) = 0;
    virtual void set_vsync(bool enabled) = 0;
    [[nodiscard]] virtual bool is_vsync() const = 0;

    virtual void *get_native_window() const = 0;

    static Window *create(const WindowProps &props = WindowProps());
  };

}
