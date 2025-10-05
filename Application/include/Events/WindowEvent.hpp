#pragma once

#include <Apch.hpp>
#include <Events/Event.hpp>


namespace aero
{

  class WindowResizeEvent final : public Event
  {
  public:
    WindowResizeEvent(const unsigned int width, const unsigned int height)
        : m_Width(width), m_Height(height) {}

    unsigned int get_width() const { return m_Width; }
    unsigned int get_height() const { return m_Height; }

    std::string to_string() const override
    {
      std::stringstream ss;
      ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
      return ss.str();
    }

    EVENT_CLASS_TYPE(Window_Resize)
    EVENT_CLASS_CATEGORY(Event_Category_Application)
  private:
    unsigned int m_Width, m_Height;
  };

  class WindowCloseEvent final : public Event
  {
  public:
    WindowCloseEvent() = default;

    EVENT_CLASS_TYPE(Window_Close)
    EVENT_CLASS_CATEGORY(Event_Category_Application)
  };

  class AppTickEvent final : public Event
  {
  public:
    AppTickEvent() = default;

    EVENT_CLASS_TYPE(App_Tick)
    EVENT_CLASS_CATEGORY(Event_Category_Application)
  };

  class AppUpdateEvent final : public Event
  {
  public:
    AppUpdateEvent() = default;

    EVENT_CLASS_TYPE(App_Update)
    EVENT_CLASS_CATEGORY(Event_Category_Application)
  };

  class AppRenderEvent final : public Event
  {
  public:
    AppRenderEvent() = default;

    EVENT_CLASS_TYPE(App_Render)
    EVENT_CLASS_CATEGORY(Event_Category_Application)
  };
}
