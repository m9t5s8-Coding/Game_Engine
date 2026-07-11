#pragma once

#include <Events/Event.hpp>

namespace ag {
class TextInputEvent : public Event {
public:
  TextInputEvent(char character)
    : m_character(character) {}

  char get_character() const { return m_character; }

  std::string to_string() const override { return std::string("TextInputEvent: ") + m_character; }

  EVENT_CLASS_TYPE(Text_Input)
  EVENT_CLASS_CATEGORY(Event_Category_Keyboard | Event_Category_Input)

private:
  char m_character;
};
}  // namespace ag
