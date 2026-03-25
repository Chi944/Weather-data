#ifndef WEATHER_RECORD_H_INCLUDED
#define WEATHER_RECORD_H_INCLUDED

#include "Date.h"
#include "Time.h"

    /**
     * @file WeatherRecord.h
     * @brief WeatherRecord class — a single weather station reading
     *
     * Pure Model class with no I/O. Stores the date, time, wind speed,
     * solar radiation, and temperature for one reading interval.
     *
     * MatchesMonthYear and HasSameTimestamp encapsulate comparisons that
     * would otherwise require callers to chain through Date and Time,
     *
     * @author Deston
     * @version 2.0
     * @date 23/03/2026
     */

class WeatherRecord
{
public:
    /**
     * @brief Default constructor — initialises all numeric fields to 0
     */
    WeatherRecord();

    /** @brief Get the date of the reading */
    Date GetDate() const;

    /** @brief Set the date of the reading */
    void SetDate(const Date& d);

    /** @brief Get the time of the reading */
    Time GetTime() const;

    /** @brief Set the time of the reading */
    void SetTime(const Time& t);

    /** @brief Get wind speed in m/s */
    float GetWindSpeed() const;

    /** @brief Set wind speed in m/s */
    void SetWindSpeed(float speed);

    /** @brief Get solar radiation in W/m2 */
    float GetSolarRadiation() const;

    /** @brief Set solar radiation in W/m2 */
    void SetSolarRadiation(float radiation);

    /** @brief Get temperature in degrees C */
    float GetTemperature() const;

    /** @brief Set temperature in degrees C */
    void SetTemperature(float temp);

    /**
     * @brief Get the year of the reading
     * @return Year component of the record's date
     */
    int GetYear() const;

    /**
     * @brief Check whether this record belongs to a given month and year
     * @param month Month number (1-12)
     * @param year  Year
     * @return true if the record's date matches both month and year
     */
    bool MatchesMonthYear(int month, int year) const;

private:
    Date  m_date;           /**< Date of reading */
    Time  m_time;           /**< Time of reading */
    float m_windSpeed;      /**< Wind speed in m/s */
    float m_solarRadiation; /**< Solar radiation in W/m2 */
    float m_temperature;    /**< Temperature in degrees C */
};

#endif // WEATHER_RECORD_H_INCLUDED
