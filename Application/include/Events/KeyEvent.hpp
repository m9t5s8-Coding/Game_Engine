#pragma once

#include <Apch.hpp>
#include <Events/KeyCodes.hpp>


namespace aero
{
  class KeyEvent : public Event
  {
  public:
    KeyCode get_key_code() const { return m_keycode; }

    EVENT_CLASS_CATEGORY(Event_Category_Keyboard | Event_Category_Input)

  protected:
    KeyEvent(const KeyCode keycode)
        : m_keycode(keycode) {}

    KeyCode m_keycode;
  };




  class KeyPressedEvent final : public KeyEvent
  {
  public:
    KeyPressedEvent(const KeyCode keycode, bool is_Repeat = false)
        : KeyEvent(keycode), m_is_Repeat(is_Repeat) {}

    bool is_repeat() const { return m_is_Repeat; }

    std::string to_string() const override
    {
      std::stringstream ss;
      ss << "KeyPressedEvent: " << m_keycode << " (repeat = " << m_is_Repeat << ")";
      return ss.str();
    }

    EVENT_CLASS_TYPE(Key_Pressed)

  private:
    bool m_is_Repeat;
  };






  class KeyReleasedEvent : public KeyEvent
  {
  public:
    KeyReleasedEvent(const KeyCode keycode)
        : KeyEvent(keycode) {}

    std::string to_string() const override
    {
      std::stringstream ss;
      ss << "KeyReleasedEvent: " << m_keycode;
      return ss.str();
    }

    EVENT_CLASS_TYPE(Key_Released)
  };





  class KeyTypedEvent : public KeyEvent
  {
  public:
    KeyTypedEvent(const KeyCode keycode)
        : KeyEvent(keycode) {}

    std::string to_string() const override
    {
      std::stringstream ss;
      ss << "KeyTypedEvent: " << m_keycode;
      return ss.str();
    }

    EVENT_CLASS_TYPE(Key_Typed)
  };
}
