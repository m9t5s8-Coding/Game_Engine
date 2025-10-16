#pragma once

#include <Apch.hpp>

namespace ag
{
  enum class Event_Type
  {
    None = 0,
    // Window Event
    Window_Close,
    Window_Resize,
    Window_Focus,
    Window_lost_Focus,
    Window_Moved,

    // App Event
    App_Tick,
    App_Update,
    App_Render,

    // Keyboard Event
    Key_Pressed,
    Key_Released,
    Key_Typed,

    // Mouse Event
    Mouse_Button_Pressed,
    Mouse_Button_Released,
    Mouse_Moved,
    Mouse_Scrolled
  };

#define BIT(x) (1 << x)

  enum Event_Category
  {
    None = 0,
    Event_Category_Application = BIT(0),
    Event_Category_Input = BIT(1),
    Event_Category_Keyboard = BIT(2),
    Event_Category_Mouse = BIT(3),
    Event_Category_Mouse_Button = BIT(4)
  };

#define EVENT_CLASS_TYPE(type)                                                     \
  static Event_Type get_static_type() { return Event_Type::type; }                 \
  virtual Event_Type get_event_type() const override { return get_static_type(); } \
  virtual const char *get_name() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
  virtual int get_category_flag() const override { return category; }

  class Event
  {
  public:
    virtual ~Event() = default;

    bool Handled = false;

    virtual Event_Type get_event_type() const = 0;
    virtual const char *get_name() const = 0;
    virtual int get_category_flag() const = 0;
    virtual std::string to_string() const { return get_name(); }

    inline bool is_in_category(Event_Category category) const
    {
      return get_category_flag() & category;
    }
  };

  class EventDispatcher
  {
  public:
    explicit EventDispatcher(Event &event)
        : m_Event(event)
    {
    }

    template <typename T, typename F>
    bool Dispatch(const F &func)
    {
      if (m_Event.get_event_type() == T::get_static_type())
      {
        m_Event.Handled |= func(static_cast<T &>(m_Event));
        return true;
      }
      return false;
    }

  private:
    Event &m_Event;
  };

  inline std::ostream &operator<<(std::ostream &os, const Event &e)
  {
    return os << e.to_string();
  }

}
