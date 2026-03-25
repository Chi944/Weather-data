#ifndef WEATHER_DATA_STORE_H_INCLUDED
#define WEATHER_DATA_STORE_H_INCLUDED

#include "Map.h"
#include "Vector.h"
#include "WeatherRecord.h"
#include "Bst.h"
#include "Constants.h"

    /**
     * @file WeatherDataStore.h
     * @brief Indexed weather data store
     *
     * @details
     * Uses two complementary data structures whose properties suit the
     * assignment access patterns:
     *
     *   Map<int, Map<int, MonthlyData>>
     *     The outer Map associates each year with an inner Map.
     *     The inner Map associates each month (1-12) with a MonthlyData
     *     struct holding three parallel float vectors (wind, temperature,
     *     solar).  O(n) lookup is acceptable because there are at most
     *     ~10 outer entries (years) and 12 inner entries (months).
     *
     *   Bst<int>
     *     Stores unique calendar years in a BST.  Provides O(log n) year
     *     lookup (search) and sorted in-order traversal via f1Typ function
     *     pointers.  DisplayAvailableYears passes PrintYear as the callback,
     *     demonstrating the required BST-with-function-pointer pattern.
     *     The BST rejects duplicate years automatically (insert returns false
     *     for duplicates), guaranteeing a clean year index.
     *
     * Rationale summary
     *   Map: encapsulates all year->month->data access behind a clean
     *        interface.  Replacing the internal Vector storage with
     *        another structure would not require any change to client code.
     *   BST: enables sorted year iteration and O(log n) year existence
     *        checks, and satisfies the function-pointer traversal requirement.
     *
     * @author Deston
     * @version 2.0
     * @date 24/03/2026
     */

// ---------------------------------------------------------------------------
// MonthlyData — per-month data stored as three parallel float vectors
// ---------------------------------------------------------------------------

/**
 * @brief Aggregated weather readings for one calendar month.
 *
 * The three vectors are parallel: windSpeeds[i], temperatures[i], and
 * solarRaw[i] all come from the same original WeatherRecord, preserving
 * the pairing needed for sPCC (Pearson correlation) calculations.
 */
struct MonthlyData
{
    Vector<float> windSpeeds;    ///< Wind speeds in km/h (converted from m/s on insertion)
    Vector<float> temperatures;  ///< Ambient temperatures in degrees C
    Vector<float> solarRaw;      ///< Solar radiation in W/m2 (raw, pre-threshold filter)
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
     * @brief Populate the store from a flat Vector of WeatherRecord.
     *
     * Iterates once through all records.  Each record is inserted into
     * Map[year][month] (created automatically via operator[] if absent)
     * and the year is inserted into the BST (duplicates silently ignored).
     * Wind speed is converted from m/s to km/h during insertion.
     *
     * @param records  All loaded weather records.
     */
    void Build(const Vector<WeatherRecord>& records);

    /**
     * @brief Check whether data exists for a given year and month.
     * @param year   Calendar year.
     * @param month  Month number (1-12).
     * @return true if at least one reading was stored for that year/month.
     */
    bool HasData(int year, int month) const;

    /**
     * @brief Const pointer to the MonthlyData for a specific year and month.
     * @param year   Calendar year.
     * @param month  Month number (1-12).
     * @return Pointer to MonthlyData, or nullptr if no data exists.
     */
    const MonthlyData* GetMonthData(int year, int month) const;

    /**
     * @brief Collect parallel wind/temperature/solarRaw vectors for one month
     *        across ALL loaded years (used for sPCC in option 3).
     *
     * Iterates the outer Map by index to visit every year, then checks
     * whether that year's inner Map contains the requested month.
     *
     * @param month     Month number (1-12).
     * @param winds     Output: wind speeds (km/h) across all years.
     * @param temps     Output: temperatures (C) across all years.
     * @param solarRaw  Output: raw solar radiation (W/m2) across all years.
     */
    void GetMonthDataAllYears(int month,
                               Vector<float>& winds,
                               Vector<float>& temps,
                               Vector<float>& solarRaw) const;

    /**
     * @brief In-order traversal of the year BST using the f1Typ callback.
     *
     * Passes the caller-supplied function pointer directly to
     * Bst<int>::inOrderTraversal, which visits each year in ascending order
     * and invokes the callback
     *
     * @param visit  Callback matching Bst<int>::f1Typ: void (*)(int&).
     */
    void TraverseYears(void (*visit)(int&)) const;

    /**
     * @brief Check whether a year exists in the BST.
     * @param year  Year to look up.
     * @return true if the year was loaded.
     */
    bool YearExists(int year) const;

    /**
     * @brief Number of unique years stored in the BST.
     * @return Count of unique years.
     */
    int GetYearCount() const;

    /**
     * @brief Check whether the store is completely empty.
     * @return true if no records have been built into the store.
     */
    bool IsEmpty() const;

private:

    /**
     * @brief Nested Map: year -> month -> MonthlyData vectors.
     *
     * Outer Map key: calendar year (int).
     * Inner Map key: month number 1-12 (int).
     * Value: MonthlyData struct with three parallel float Vectors.
     */
    Map<int, Map<int, MonthlyData> > m_data;

    /** @brief BST of unique years — sorted traversal and O(log n) search. */
    Bst<int> m_yearBst;

    /**
     * @brief Convert wind speed from m/s to km/h.
     * @param mps  Speed in metres per second.
     * @return Speed in kilometres per hour (mps * WIND_MPS_TO_KMH).
     */
    static float ToKmh(float mps);
};

#endif // WEATHER_DATA_STORE_H_INCLUDED
