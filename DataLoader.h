#ifndef DATA_LOADER_H_INCLUDED
#define DATA_LOADER_H_INCLUDED

#include "Vector.h"
#include "WeatherRecord.h"
#include <fstream>
#include <sstream>
#include <string>

    /**
     * @file DataLoader.h
     * @brief DataLoader class — parses weather records from a single CSV file
     *
     * Opens a CSV file, detects column positions from the header row, and
     * converts each data line into a WeatherRecord. Column order is determined
     * dynamically so that differently-ordered CSV files are supported.
     *
     * Error information is stored internally and retrieved via GetLastError().
     * This class contains no console output.
     *
     * @author Deston
     * @version 1.0
     * @date 18/02/2026
     */

class DataLoader
{
public:
    /**
     * @brief Default constructor
     */
    DataLoader();

    /**
     * @brief Load weather records from a CSV file
     * @param filename Path to the CSV file
     * @param records  Vector to append loaded records to
     * @return true if at least one valid record was loaded
     */
    bool LoadFromFile(const std::string& filename,
                      Vector<WeatherRecord>& records);

    /**
     * @brief Get the most recent error description
     * @return Error message string, or empty string if no error occurred
     */
    std::string GetLastError() const;

private:
    std::string m_lastError;  /**< Most recent error description */

    /**
     * @brief Parse the CSV header row to locate column indices
     * @param header   Header line string
     * @param wastIdx  Output: WAST column index (-1 if not found)
     * @param windIdx  Output: S (wind speed) column index
     * @param solarIdx Output: SR (solar radiation) column index
     * @param tempIdx  Output: T (temperature) column index
     * @return true if the mandatory WAST column was found
     */
    bool ParseHeader(const std::string& header,
                     int& wastIdx, int& windIdx,
                     int& solarIdx, int& tempIdx);

    /**
     * @brief Parse one CSV data line into a WeatherRecord
     * @param line     CSV data line
     * @param wastIdx  WAST column index
     * @param windIdx  Wind speed column index
     * @param solarIdx Solar radiation column index
     * @param tempIdx  Temperature column index
     * @param record   Output: populated WeatherRecord
     * @return true if the line was successfully parsed
     */
    bool ParseLine(const std::string& line,
                   int wastIdx, int windIdx,
                   int solarIdx, int tempIdx,
                   WeatherRecord& record);

    /**
     * @brief Parse the combined WAST date/time field
     * @param wast WAST string (format: "DD/MM/YYYY H:MM")
     * @param date Output: parsed Date
     * @param time Output: parsed Time
     * @return true if successfully parsed
     */
    bool ParseWAST(const std::string& wast, Date& date, Time& time);

    /**
     * @brief Trim leading and trailing whitespace from a string
     * @param str Input string
     * @return Trimmed copy
     */
    std::string Trim(const std::string& str);

    /**
     * @brief Convert a string to float, returning a default on failure
     * @param str        String to convert
     * @param defaultVal Value returned if conversion fails
     * @return Converted float or defaultVal
     */
    float SafeStringToFloat(const std::string& str, float defaultVal);
};

#endif // DATA_LOADER_H_INCLUDED
