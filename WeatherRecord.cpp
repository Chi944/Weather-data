#include "WeatherRecord.h"

WeatherRecord::WeatherRecord()
    : m_windSpeed(0.0f), m_solarRadiation(0.0f), m_temperature(0.0f)
{
}

Date  WeatherRecord::GetDate() const
{
    return m_date;
}

void  WeatherRecord::SetDate(const Date& d)
{
    m_date = d;
}

Time  WeatherRecord::GetTime() const
{
    return m_time;
}

void  WeatherRecord::SetTime(const Time& t)
{
    m_time = t;
}

float WeatherRecord::GetWindSpeed() const
{
    return m_windSpeed;
}

void  WeatherRecord::SetWindSpeed(float speed)
{
    m_windSpeed = speed;
}

float WeatherRecord::GetSolarRadiation() const
{
    return m_solarRadiation;
}

void  WeatherRecord::SetSolarRadiation(float radiation)
{
    m_solarRadiation = radiation;
}

float WeatherRecord::GetTemperature() const
{
    return m_temperature;
}

void  WeatherRecord::SetTemperature(float temp)
{
    m_temperature = temp;
}

int WeatherRecord::GetYear() const
{
    return m_date.GetYear();
}

bool WeatherRecord::MatchesMonthYear(int month, int year) const
{
    return m_date.GetMonth() == month && m_date.GetYear() == year;
}

