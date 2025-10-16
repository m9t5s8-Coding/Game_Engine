#pragma once

#include <Apch.hpp>
#include <Events/Event.hpp>
#include <Math/Math.hpp>
#include <Core/Core.hpp>


namespace ag
{

  class WindowResizeEvent final : public Event
  {
  public:
    WindowResizeEvent(const vec2u& p_size)
        : m_size(p_size) {}

    AG_uint get_width() const { return m_size.x; }
    AG_uint get_height() const { return m_size.y; }

    vec2u get_size() const { return m_size; }

    std::string to_string() const override
    {
      std::stringstream ss;
      ss << "WindowResizeEvent: " << m_size.x << ", " << m_size.y;
      return ss.str();
    }

    EVENT_CLASS_TYPE(Window_Resize)
    EVENT_CLASS_CATEGORY(Event_Category_Application)
  private:
    vec2u m_size;
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
