#ifndef CALCULATOR_H_INCLUDED
#define CALCULATOR_H_INCLUDED

#include "WeatherDataStore.h"
#include "Constants.h"

    /**
     * @file Calculator.h
     * @brief Domain-specific statistics on WeatherDataStore data.
     *
     * Methods delegate numeric work to the generic Stats free functions
     * (CalculateMean, CalculateSampleStdDev, CalculateMAD, CalculateSPCC)
     * and apply unit conversions and solar-radiation filtering.
     * All methods are const — Calculator holds no mutable state.
     *
     * @author Deston
     * @version 2.0
     * @date 24/03/2026
     */
class Calculator
{
public:
    /** @brief Default constructor. */
    Calculator();

    /**
     * @brief Mean, sample stdev, and MAD of wind speed for one month/year.
     * @param store   Indexed weather data store.
     * @param month   Month number (1-12).
     * @param year    Calendar year.
     * @param mean    Output: mean wind speed in km/h.
     * @param stdDev  Output: sample standard deviation in km/h.
     * @param mad     Output: mean absolute deviation in km/h.
     * @return true if at least one record was found.
     */
    bool CalculateWindStats(const WeatherDataStore& store,
                            int month, int year,
                            float& mean, float& stdDev, float& mad) const;

    /**
     * @brief Mean, sample stdev, and MAD of temperature for one month/year.
     * @param store   Indexed weather data store.
     * @param month   Month number (1-12).
     * @param year    Calendar year.
     * @param mean    Output: mean temperature in degrees C.
     * @param stdDev  Output: sample standard deviation in degrees C.
     * @param mad     Output: mean absolute deviation in degrees C.
     * @return true if at least one record was found.
     */
    bool CalculateTempStats(const WeatherDataStore& store,
                            int month, int year,
                            float& mean, float& stdDev, float& mad) const;

    /**
     * @brief Total solar radiation for one month/year in kWh/m2.
     *
     * Only readings >= MIN_SOLAR_RADIATION W/m2 are included.
     * Conversion: W/m2 over 10 min = W/m2 / 6000 kWh/m2.
     *
     * @param store   Indexed weather data store.
     * @param month   Month number (1-12).
     * @param year    Calendar year.
     * @param total   Output: total solar radiation in kWh/m2.
     * @return true if at least one qualifying reading was found.
     */
    bool CalculateSolarTotal(const WeatherDataStore& store,
                             int month, int year,
                             float& total) const;

    /**
     * @brief Sample Pearson Correlation Coefficients for a given month
     *        across ALL loaded years.
     *
     * Combinations computed:
     *  - S_T: Wind Speed vs Temperature       (all records for the month)
     *  - S_R: Wind Speed vs Solar Radiation   (records where SR >= 100 W/m2 only)
     *  - T_R: Temperature vs Solar Radiation  (records where SR >= 100 W/m2 only)
     *
     * @param store  Indexed weather data store.
     * @param month  Month number (1-12).
     * @param s_t    Output: sPCC for Wind Speed vs Temperature.
     * @param s_r    Output: sPCC for Wind Speed vs Solar Radiation.
     * @param t_r    Output: sPCC for Temperature vs Solar Radiation.
     * @return true if enough data existed to compute at least S_T.
     */
    bool CalculateCorrelations(const WeatherDataStore& store, int month,
                                float& s_t, float& s_r, float& t_r) const;

private:
    /**
     * @brief Convert a 10-minute W/m2 solar reading to kWh/m2.
     * @param wm2  Solar radiation in W/m2.
     * @return Energy in kWh/m2.
     */
    static float ToKwhM2(float wm2);
};

#endif // CALCULATOR_H_INCLUDED
