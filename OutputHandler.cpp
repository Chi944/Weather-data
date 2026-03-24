#include "OutputHandler.h"
#include "Constants.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <locale>
#include <sstream>
#include <ctime>

OutputHandler::OutputHandler()
{
}

std::string OutputHandler::GetMonthName(int month)
{
    if (month < 1 || month > 12) return "Unknown";
    std::tm t = {};
    t.tm_mon  = month - 1;   // tm_mon is 0-based
    std::ostringstream oss;
    oss.imbue(std::locale());
    const std::time_put<char>& tp =
        std::use_facet<std::time_put<char> >(oss.getloc());
    tp.put(oss, oss, ' ', &t, 'B');
    return oss.str();
}

// ---------------------------------------------------------------------------
// Option 1 — wind speed
// ---------------------------------------------------------------------------

void OutputHandler::DisplayWindStats(int month, int year,
                                     float mean, float stdDev) const
{
    std::cout << std::endl;
    std::cout << GetMonthName(month) << " " << year << ":" << std::endl;
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "Average speed: " << mean   << " km/h" << std::endl;
    std::cout << "Sample stdev: "  << stdDev << std::endl;
}

void OutputHandler::DisplayNoData(int month, int year) const
{
    std::cout << std::endl;
    std::cout << GetMonthName(month) << " " << year << ": No Data" << std::endl;
}

// ---------------------------------------------------------------------------
// Option 2 — temperature
// ---------------------------------------------------------------------------

void OutputHandler::DisplayTempHeader(int year) const
{
    std::cout << std::endl;
    std::cout << year << std::endl;
}

void OutputHandler::DisplayTempForMonth(int month, float mean, float stdDev) const
{
    std::cout << std::fixed << std::setprecision(1);
    std::cout << GetMonthName(month)
              << ": average: " << mean
              << " degrees C, stdev: " << stdDev << std::endl;
}

void OutputHandler::DisplayNoDataForMonth(int month) const
{
    std::cout << GetMonthName(month) << ": No Data" << std::endl;
}

// ---------------------------------------------------------------------------
// Option 3 — Sample Pearson Correlation Coefficient
// ---------------------------------------------------------------------------

void OutputHandler::DisplaySPCCHeader(int month) const
{
    std::cout << std::endl;
    std::cout << "Sample Pearson Correlation Coefficient for "
              << GetMonthName(month) << std::endl;
}

void OutputHandler::DisplaySPCC(float s_t, float s_r, float t_r) const
{
    // A2 spec format: n.nn (2 decimal places)
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "S_T: " << s_t << std::endl;
    std::cout << "S_R: " << s_r << std::endl;
    std::cout << "T_R: " << t_r << std::endl;
}

void OutputHandler::DisplaySPCCNoData(int month) const
{
    std::cout << std::endl;
    std::cout << GetMonthName(month) << ": No Data" << std::endl;
}

// ---------------------------------------------------------------------------
// Option 4 — CSV file output
// ---------------------------------------------------------------------------

void OutputHandler::WriteCSVHeader(std::ofstream& file, int year) const
{
    file << year << std::endl;
}

void OutputHandler::WriteCSVRow(std::ofstream& file, int month,
                                bool hasWind,  float windMean,
                                float windStdDev,  float windMAD,
                                bool hasTemp,  float tempMean,
                                float tempStdDev,  float tempMAD,
                                bool hasSolar, float solarTotal) const
{
    // A2 spec example: "January, 5.5(1.2, 1.1),25.5(12.2, 11.3),196.4"
    // Space after month comma; spaces inside parentheses around stdev/mad.
    file << std::fixed << std::setprecision(1);
    file << GetMonthName(month) << ", ";    // space after month comma

    if (hasWind)
        file << windMean << "(" << windStdDev << ", " << windMAD << ")";

    file << ",";                            // no trailing space between field commas

    if (hasTemp)
        file << tempMean << "(" << tempStdDev << ", " << tempMAD << ")";

    file << ",";

    if (hasSolar)
        file << solarTotal;

    file << std::endl;
}

void OutputHandler::WriteCSVNoData(std::ofstream& file) const
{
    // A2 spec: "If the entire year's data is not available, output just
    // the year on the first line and the message 'No Data' on the second."
    file << "No Data" << std::endl;
}

void OutputHandler::DisplayFileWritten(const std::string& filename) const
{
    std::cout << std::endl;
    std::cout << "Output written to '" << filename << "'" << std::endl;
}

void OutputHandler::DisplayFileError(const std::string& filename) const
{
    std::cerr << "ERROR: Cannot create output file '"
              << filename << "'" << std::endl;
}
