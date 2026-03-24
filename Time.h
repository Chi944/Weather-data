#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED

    /**
     * @file Time.h
     * @brief Time class representing a time of day (hour and minute)
     *
     * Pure Model class with no I/O. Encapsulates hour and minute with
     * validation. SetTime validates both components together before any
     * assignment, preventing a partially-invalid state.
     *
     * @author Deston
     * @version 1.0
     * @date 18/02/2026
     */

class Time
{
public:
    /**
     * @brief Default constructor — initialises to 00:00
     */
    Time();

    /**
     * @brief Parameterised constructor
     * @param h Hour (0-23)
     * @param m Minute (0-59)
     */
    Time(int h, int m);

    /**
     * @brief Get the hour component
     * @return Hour (0-23)
     */
    int GetHour() const;

    /**
     * @brief Get the minute component
     * @return Minute (0-59)
     */
    int GetMinute() const;

    /**
     * @brief Set time components atomically
     * @param h Hour (0-23)
     * @param m Minute (0-59)
     * @return true if both components are valid and were set, false otherwise
     */
    bool SetTime(int h, int m);

private:
    int m_hour;   /**< Hour (0-23) */
    int m_minute; /**< Minute (0-59) */

    /**
     * @brief Validate hour and minute values
     * @param h Hour to validate
     * @param m Minute to validate
     * @return true if both values are within their valid ranges
     */
    bool IsValid(int h, int m) const;
};

#endif // TIME_H_INCLUDED
