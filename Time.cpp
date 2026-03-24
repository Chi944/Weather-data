#include "Time.h"

Time::Time() : m_hour(0), m_minute(0)
{
}

Time::Time(int h, int m) : m_hour(0), m_minute(0)
{
    SetTime(h, m);
}

int Time::GetHour() const
{
    return m_hour;
}

int Time::GetMinute() const
{
    return m_minute;
}

bool Time::SetTime(int h, int m)
{
    if (IsValid(h, m))
    {
        m_hour   = h;
        m_minute = m;
        return true;
    }
    return false;
}

bool Time::IsValid(int h, int m) const
{
    return (h >= 0 && h <= 23 && m >= 0 && m <= 59);
}
