#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

    /**
     * @file Constants.h
     * @brief Global constants used throughout the Weather Data Processor
     *
     * Centralising constants here prevents magic numbers scattered across
     * the codebase, and ensures a single point of change when values need
     * to be updated. All constants are in this file per the global constants
     * requirement.
     *
     * @author Deston
     * @version 1.0
     * @date 23/02/2026
     */

// ---- Solar Radiation ----
/** Minimum valid solar radiation (W/m2) to be included in calculations */
const float MIN_SOLAR_RADIATION = 100.0f;

/** Divisor for converting W/m2 over a 10-minute interval to kWh/m2.
 *  Formula: W/m2 * (10 min / 60 min) / 1000 = W/m2 / 6000 */
const float SOLAR_CONVERSION_DIVISOR = 6000.0f;

// ---- Wind Speed ----
/** Conversion factor from m/s to km/h (1 m/s = 3.6 km/h) */
const float WIND_MPS_TO_KMH = 3.6f;

// ---- Array Sizing ----
/** Maximum number of unique years the YearTracker will store */
const int MAX_YEARS = 200;

/** Maximum number of CSV fields expected per data line */
const int MAX_CSV_FIELDS = 30;

// ---- File Paths ----
/** Path to the data source configuration file */
const char DATA_SOURCE_FILE[] = "data/data_source.txt";

/** Folder prefix for all data CSV files */
const char DATA_FOLDER[] = "data/";

/** Output filename for menu option 4 */
const char OUTPUT_FILENAME[] = "WindTempSolar.csv";

#endif // CONSTANTS_H_INCLUDED
