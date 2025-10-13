#pragma once

namespace aero
{
  class TimeStamp
  {
  public:
    TimeStamp(float p_time = 0.0f)
        : m_time(p_time)
    {
    }

    float get_seconds() const { return m_time; }
    float get_milliseconds() const { return m_time * 1000; }
  private:
    float m_time;
  };
}
