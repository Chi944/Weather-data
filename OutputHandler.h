#ifndef OUTPUT_HANDLER_H_INCLUDED
#define OUTPUT_HANDLER_H_INCLUDED

#include <fstream>
#include <string>

    /**
     * @file OutputHandler.h
     * @brief Formats and writes all program output for the Weather Data Processor.
     *
     * Handles console display (options 1-3) and CSV file output (option 4).
     * All methods are const — OutputHandler holds no mutable state.
     * CSV file methods accept std::ofstream& so the caller controls file lifetime.
     *
     * @author Deston
     * @version 2.1
     * @date 24/03/2026
     */
class OutputHandler
{
public:
    /** @brief Default constructor. */
    OutputHandler();

    /**
     * @brief Display wind speed statistics for a single month/year (option 1).
     * @param month  Month number (1-12).
     * @param year   Calendar year.
     * @param mean   Mean wind speed in km/h.
     * @param stdDev Sample standard deviation in km/h.
     */
    void DisplayWindStats(int month, int year, float mean, float stdDev) const;

    /**
     * @brief Display a no-data message for a month/year (option 1).
     * @param month Month number (1-12).
     * @param year  Calendar year.
     */
    void DisplayNoData(int month, int year) const;

    /**
     * @brief Display the temperature section year header (option 2).
     * @param year Year being reported.
     */
    void DisplayTempHeader(int year) const;

    /**
     * @brief Display temperature statistics for one month (option 2).
     * @param month  Month number (1-12).
     * @param mean   Mean temperature in degrees C.
     * @param stdDev Sample standard deviation in degrees C.
     */
    void DisplayTempForMonth(int month, float mean, float stdDev) const;

    /**
     * @brief Display a no-data message for one month in a multi-month listing.
     * @param month Month number (1-12).
     */
    void DisplayNoDataForMonth(int month) const;

    /**
     * @brief Display the sPCC section header showing the selected month (option 3).
     * @param month Month number (1-12).
     */
    void DisplaySPCCHeader(int month) const;

    /**
     * @brief Display the three sPCC values: S_T, S_R, T_R (option 3).
     * @param s_t  sPCC for Wind Speed vs Temperature.
     * @param s_r  sPCC for Wind Speed vs Solar Radiation.
     * @param t_r  sPCC for Temperature vs Solar Radiation.
     */
    void DisplaySPCC(float s_t, float s_r, float t_r) const;

    /**
     * @brief Display a no-data message when sPCC cannot be computed (option 3).
     * @param month Month number (1-12).
     */
    void DisplaySPCCNoData(int month) const;

    /**
     * @brief Write the year header line to an open CSV file (option 4).
     * @param file Open output stream.
     * @param year Year being reported.
     */
    void WriteCSVHeader(std::ofstream& file, int year) const;

    /**
     * @brief Write one month data row to the CSV file with stdev AND MAD (option 4).
     *
     * Output format per A2 spec:
     * MonthName, windMean(windStdDev, windMAD),tempMean(tempStdDev, tempMAD),solar
     *
     * If a field has no data (hasWind/hasTemp/hasSolar = false) the field
     * is left blank between its surrounding commas, e.g.:
     * February, 4.5(3.1, 2.9),,200.3
     *
     * @param file        Open output stream.
     * @param month       Month number (1-12).
     * @param hasWind     true if wind data is valid.
     * @param windMean    Mean wind speed in km/h.
     * @param windStdDev  Wind sample standard deviation.
     * @param windMAD     Wind mean absolute deviation.
     * @param hasTemp     true if temperature data is valid.
     * @param tempMean    Mean temperature in degrees C.
     * @param tempStdDev  Temperature sample standard deviation.
     * @param tempMAD     Temperature mean absolute deviation.
     * @param hasSolar    true if solar data is valid.
     * @param solarTotal  Total solar radiation in kWh/m2.
     */
    void WriteCSVRow(std::ofstream& file, int month,
                     bool hasWind,  float windMean,  float windStdDev,  float windMAD,
                     bool hasTemp,  float tempMean,  float tempStdDev,  float tempMAD,
                     bool hasSolar, float solarTotal) const;

    /**
     * @brief Write "No Data" to the CSV file when an entire year has no records.
     *
     * @param file Open output stream.
     */
    void WriteCSVNoData(std::ofstream& file) const;

    /**
     * @brief Display confirmation that the CSV file was written successfully.
     * @param filename Path of the written file.
     */
    void DisplayFileWritten(const std::string& filename) const;

    /**
     * @brief Display an error message when the CSV file cannot be opened.
     * @param filename Path of the file that could not be opened.
     */
    void DisplayFileError(const std::string& filename) const;

private:
    /**
     * @brief Convert a month number to its full name.
     * @param month Month number (1-12).
     * @return Full month name (e.g. "March"), or "Unknown" for invalid input.
     */
    static std::string GetMonthName(int month);
};

#endif // OUTPUT_HANDLER_H_INCLUDED
