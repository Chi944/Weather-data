#ifndef DATE_H_INCLUDED
#define DATE_H_INCLUDED

    /**
     * @file Date.h
     * @brief Calendar Date class with validation and comparison operators.
     *
     * @details
     * Pure model class, no I/O performed internally. Encapsulates day,
     * month, and year with full validation including leap-year logic.
     * SetDate validates all three components atomically before applying
     * any change, preventing a partially-invalid state.
     *
     * Three comparison operators (operator<, operator>, operator==) enable
     * use as the template parameter T of Bst<T>. Dates are ordered
     * chronologically: year is the most significant field, followed by
     * month, then day.
     *
     * @author  Deston
     * @version 2.0
     * @date    12/03/2026
     */
class Date
{
public:

    /** @brief Default constructor � initialises to 01/01/1900. */
    Date();

    /**
     * @brief Parameterised constructor.
     * @param d  Day   (1-31)
     * @param m  Month (1-12)
     * @param y  Year  (>= 1900)
     * @note Falls back to 01/01/1900 if the combination is invalid.
     */
    Date(int d, int m, int y);

    /** @brief Returns the day component (1-31). */
    int GetDay()   const;

    /** @brief Returns the month component (1-12). */
    int GetMonth() const;

    /** @brief Returns the year component (>= 1900). */
    int GetYear()  const;

    /**
     * @brief Set all three date components atomically.
     * @param d  Day   (1-31)
     * @param m  Month (1-12)
     * @param y  Year  (>= 1900)
     * @return true if valid and applied; false otherwise (object unchanged).
     */
    bool SetDate(int d, int m, int y);

    /**
     * @brief Chronological equality.
     * @return true if day, month, and year all match.
     */
    bool operator==(const Date& other) const;

    /**
     * @brief Chronological less-than (year -> month -> day).
     * @return true if *this is earlier than other.
     */
    bool operator<(const Date& other) const;

    /**
     * @brief Chronological greater-than.
     * @return true if *this is later than other.
     */
    bool operator>(const Date& other) const;

private:

    int m_day;
    int m_month;
    int m_year;

    /**
     * @brief Validate date components including leap-year logic.
     * @return true if the combination forms a valid calendar date.
     */
    bool IsValid(int d, int m, int y) const;
};

#endif // DATE_H_INCLUDED
