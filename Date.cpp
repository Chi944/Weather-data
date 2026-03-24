#include "Date.h"

Date::Date() : m_day(1), m_month(1), m_year(1900) {}

Date::Date(int d, int m, int y) : m_day(1), m_month(1), m_year(1900)
{
    SetDate(d, m, y);
}

int Date::GetDay()   const { return m_day;   }
int Date::GetMonth() const { return m_month; }
int Date::GetYear()  const { return m_year;  }

bool Date::SetDate(int d, int m, int y)
{
    if (IsValid(d, m, y))
    {
        m_day   = d;
        m_month = m;
        m_year  = y;
        return true;
    }
    return false;
}

bool Date::operator==(const Date& other) const
{
    return m_year  == other.m_year  &&
           m_month == other.m_month &&
           m_day   == other.m_day;
}

bool Date::operator<(const Date& other) const
{
    if (m_year  != other.m_year)  return m_year  < other.m_year;
    if (m_month != other.m_month) return m_month < other.m_month;
    return m_day < other.m_day;
}

bool Date::operator>(const Date& other) const
{
    return other < *this;
}

bool Date::IsValid(int d, int m, int y) const
{
    if (y < 1900)         return false;
    if (m < 1 || m > 12) return false;
    if (d < 1 || d > 31) return false;

    int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    bool isLeap = ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0));
    if (isLeap) daysInMonth[1] = 29;

    return d <= daysInMonth[m - 1];
}
