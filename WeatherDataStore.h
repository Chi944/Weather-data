#ifndef WEATHER_DATA_STORE_H_INCLUDED
#define WEATHER_DATA_STORE_H_INCLUDED

#include <map>
#include "Vector.h"
#include "WeatherRecord.h"
#include "Bst.h"
#include "Constants.h"

    /**
     * @file WeatherDataStore.h
     * @brief Indexed weather data store — the primary data structure for Assignment 2.
     *
     * Uses two complementary data structures:
     *  - std::map<int, std::map<int, MonthlyData>>: maps year -> month -> aggregated
     *    float vectors, giving O(log n) access to any year/month combination.
     *  - Bst<int>: stores unique years in a binary search tree, supporting O(log n)
     *    year lookup and in-order traversal via function pointers (ascending order).
     *
     * Rationale for map: fast keyed access by year/month avoids O(n) linear scans
     * over raw records each time a menu option is invoked.
     *
     * Rationale for BST: provides sorted year iteration with the Bst<T>::f1Typ
     * function-pointer callback, enabling decoupled traversal logic — the caller
     * supplies behaviour without modifying WeatherDataStore.
     *
     * @author Deston
     * @version 1.1
     * @date 24/03/2026
     */

// ---------------------------------------------------------------------------
// MonthlyData — per-month aggregated readings stored as parallel vectors
// ---------------------------------------------------------------------------

/**
 * @brief Per-month weather data stored as three parallel float vectors.
 *
 * windSpeeds[i], temperatures[i], and solarRaw[i] all originate from the
 * same weather record, preserving the pairings needed for sPCC calculation.
 */
struct MonthlyData
{
    Vector<float> windSpeeds;    ///< Wind speeds in km/h (converted from m/s on load)
    Vector<float> temperatures;  ///< Ambient temperatures in degrees C
    Vector<float> solarRaw;      ///< Solar radiation in W/m2 (raw, pre-filter)
};

// ---------------------------------------------------------------------------
// WeatherDataStore
// ---------------------------------------------------------------------------

class WeatherDataStore
{
public:
    /** @brief Default constructor — creates an empty store. */
    WeatherDataStore();

    /**
     * @brief Populate the store from a flat vector of WeatherRecord.
     *
     * Iterates once through all records, inserting each reading into the
     * correct map bucket (year -> month) and the BST (year only).
     * Wind speed is converted from m/s to km/h during insertion.
     *
     * @param records  All loaded weather records.
     */
    void Build(const Vector<WeatherRecord>& records);

    /**
     * @brief Check whether any data exists for a given year and month.
     * @param year   Calendar year.
     * @param month  Month number (1-12).
     * @return true if at least one record exists for that year/month.
     */
    bool HasData(int year, int month) const;

    /**
     * @brief Retrieve read-only MonthlyData for a specific year and month.
     * @param year   Calendar year.
     * @param month  Month number (1-12).
     * @return Pointer to MonthlyData, or nullptr if no data exists.
     */
    const MonthlyData* GetMonthData(int year, int month) const;

    /**
     * @brief Collect parallel wind/temperature/solarRaw vectors for one month
     *        across ALL loaded years (used for sPCC option 3).
     *
     * @param month     Month number (1-12).
     * @param winds     Output: wind speeds (km/h) from all years for this month.
     * @param temps     Output: temperatures (C) from all years for this month.
     * @param solarRaw  Output: raw solar radiation (W/m2) from all years.
     */
    void GetMonthDataAllYears(int month,
                               Vector<float>& winds,
                               Vector<float>& temps,
                               Vector<float>& solarRaw) const;

    /**
     * @brief In-order traversal of the year BST using the Bst<int>::f1Typ callback.
     *
     * Signature matches Bst<int>::f1Typ exactly: void (*)(int&).
     * Calls visit(year) for each unique year in ascending order.
     * The caller supplies any per-year operation without modifying this class.
     *
     * @param visit  Callback receiving each year as int& (per Bst<T>::f1Typ).
     */
    void TraverseYears(void (*visit)(int&)) const;

    /**
     * @brief Search the year BST for a given year.
     * @param year  Year to look up.
     * @return true if at least one record has this year.
     */
    bool YearExists(int year) const;

    /**
     * @brief Number of unique years stored in the BST.
     * @return Count of unique years loaded.
     */
    int GetYearCount() const;

    /**
     * @brief Check whether the store contains any data at all.
     * @return true if the store is empty.
     */
    bool IsEmpty() const;

private:
    /** @brief Year -> month -> parallel data vectors (O(log n) access). */
    std::map<int, std::map<int, MonthlyData> > m_data;

    /** @brief BST of unique years — sorted traversal and O(log n) search. */
    Bst<int> m_yearBst;

    /**
     * @brief Convert wind speed from m/s to km/h.
     * @param mps  Speed in metres per second.
     * @return Speed in kilometres per hour.
     */
    static float ToKmh(float mps);
};

#endif // WEATHER_DATA_STORE_H_INCLUDED
