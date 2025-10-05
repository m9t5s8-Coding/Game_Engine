#pragma once

#include <Apch.hpp>
#include <Events/MouseCodes.hpp>

namespace aero
{

  class MouseMovedEvent final : public Event
  {
  public:
    MouseMovedEvent(const float x, const float y)
        : m_MouseX(x), m_MouseY(y) {}

    float getX() const { return m_MouseX; }
    float getY() const { return m_MouseY; }

    std::string to_string() const override
    {
      std::stringstream ss;
      ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
      return ss.str();
    }

    EVENT_CLASS_TYPE(Mouse_Moved)
    EVENT_CLASS_CATEGORY(Event_Category_Mouse | Event_Category_Input)
  private:
    float m_MouseX, m_MouseY;
  };

  class MouseScrolledEvent final : public Event
  {
  public:
    MouseScrolledEvent(const float xOffset, const float yOffset)
        : m_XOffset(xOffset), m_YOffset(yOffset) {}

    float get_offsetX() const { return m_XOffset; }
    float get_offsetY() const { return m_YOffset; }

    std::string to_string() const override
    {
      std::stringstream ss;
      ss << "MouseScrolledEvent: " << get_offsetX() << ", " << get_offsetY();
      return ss.str();
    }

    EVENT_CLASS_TYPE(Mouse_Scrolled)
    EVENT_CLASS_CATEGORY(Event_Category_Mouse | Event_Category_Input)
  private:
    float m_XOffset, m_YOffset;
  };

  class MouseButtonEvent : public Event
  {
  public:
    MouseCode GetMouseButton() const { return m_Button; }

    EVENT_CLASS_CATEGORY(Event_Category_Mouse | Event_Category_Input | Event_Category_Mouse_Button)
  protected:
    MouseButtonEvent(const MouseCode button)
        : m_Button(button) {}

    MouseCode m_Button;
  };

  class MouseButtonPressedEvent final : public MouseButtonEvent
  {
  public:
    MouseButtonPressedEvent(const MouseCode button)
        : MouseButtonEvent(button) {}

    std::string to_string() const override
    {
      std::stringstream ss;
      ss << "MouseButtonPressedEvent: " << m_Button;
      return ss.str();
    }

    EVENT_CLASS_TYPE(Mouse_Button_Pressed)
  };

  class MouseButtonReleasedEvent final : public MouseButtonEvent
  {
  public:
    MouseButtonReleasedEvent(const MouseCode button)
        : MouseButtonEvent(button) {}

    std::string to_string() const override
    {
      std::stringstream ss;
      ss << "MouseButtonReleasedEvent: " << m_Button;
      return ss.str();
    }

    EVENT_CLASS_TYPE(Mouse_Button_Released)
  };

}
