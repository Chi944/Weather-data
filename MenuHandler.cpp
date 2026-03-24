#include "MenuHandler.h"
#include "Constants.h"

#include <iostream>
#include <fstream>
#include <string>
#include <limits>

MenuHandler::MenuHandler()
{
}

// ---------------------------------------------------------------------------
// Static visitor — signature void (*)(int&) matches Bst<int>::f1Typ exactly
// ---------------------------------------------------------------------------

void MenuHandler::PrintYear(int& year)
{
    std::cout << year << " ";
}

// ---------------------------------------------------------------------------
// Public display methods
// ---------------------------------------------------------------------------

int MenuHandler::DisplayMenu() const
{
    std::cout << std::endl;
    std::cout << "=== Weather Data Analysis Menu ===" << std::endl;
    std::cout << "1. Average wind speed and sample stdev for a month/year"
              << std::endl;
    std::cout << "2. Average temperature and sample stdev for each month of a year"
              << std::endl;
    std::cout << "3. Sample Pearson Correlation Coefficients for a selected month"
              << std::endl;
    std::cout << "4. Wind/Temp/Solar stats (stdev & MAD) for a year (save to CSV)"
              << std::endl;
    std::cout << "5. Exit" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter your choice (1-5): ";

    int choice;
    while (!(std::cin >> choice))
    {
        std::cin.clear();
        ClearInput();
        std::cout << "Invalid input. Enter your choice (1-5): ";
    }
    ClearInput();
    return choice;
}

void MenuHandler::DisplayWelcome(int recordCount) const
{
    std::cout << "=== Weather Data Processor ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Successfully loaded " << recordCount << " records." << std::endl;
}

void MenuHandler::DisplayAvailableYears(const WeatherDataStore& store) const
{
    std::cout << "Available years in data: ";
    // BST in-order traversal via function pointer PrintYear — prints ascending
    store.TraverseYears(PrintYear);
    std::cout << std::endl;
}

// ---------------------------------------------------------------------------
// Option handlers
// ---------------------------------------------------------------------------

void MenuHandler::HandleOption1(const WeatherDataStore& store,
                                const Calculator& calc,
                                const OutputHandler& output) const
{
    int month = GetMonthInput();
    int year  = GetYearInput(store);

    float mean = 0.0f, stdDev = 0.0f, mad = 0.0f;

    if (calc.CalculateWindStats(store, month, year, mean, stdDev, mad))
        output.DisplayWindStats(month, year, mean, stdDev);
    else
        output.DisplayNoData(month, year);
}

void MenuHandler::HandleOption2(const WeatherDataStore& store,
                                const Calculator& calc,
                                const OutputHandler& output) const
{
    int year = GetYearInput(store);
    output.DisplayTempHeader(year);

    for (int month = 1; month <= 12; month++)
    {
        float mean = 0.0f, stdDev = 0.0f, mad = 0.0f;
        if (calc.CalculateTempStats(store, month, year, mean, stdDev, mad))
            output.DisplayTempForMonth(month, mean, stdDev);
        else
            output.DisplayNoDataForMonth(month);
    }
}

void MenuHandler::HandleOption3(const WeatherDataStore& store,
                                const Calculator& calc,
                                const OutputHandler& output) const
{
    int month = GetMonthInput();

    float s_t = 0.0f, s_r = 0.0f, t_r = 0.0f;

    output.DisplaySPCCHeader(month);

    if (calc.CalculateCorrelations(store, month, s_t, s_r, t_r))
        output.DisplaySPCC(s_t, s_r, t_r);
    else
        output.DisplaySPCCNoData(month);
}

void MenuHandler::HandleOption4(const WeatherDataStore& store,
                                const Calculator& calc,
                                const OutputHandler& output) const
{
    int year = GetYearInput(store);

    std::ofstream file(OUTPUT_FILENAME);
    if (!file.is_open())
    {
        output.DisplayFileError(OUTPUT_FILENAME);
        return;
    }

    output.WriteCSVHeader(file, year);

    for (int month = 1; month <= 12; month++)
    {
        float wMean = 0.0f, wStd = 0.0f, wMAD = 0.0f;
        float tMean = 0.0f, tStd = 0.0f, tMAD = 0.0f;
        float solar = 0.0f;

        bool hasWind  = calc.CalculateWindStats (store, month, year,
                                                  wMean, wStd, wMAD);
        bool hasTemp  = calc.CalculateTempStats (store, month, year,
                                                  tMean, tStd, tMAD);
        bool hasSolar = calc.CalculateSolarTotal(store, month, year, solar);

        if (hasWind || hasTemp || hasSolar)
        {
            output.WriteCSVRow(file, month,
                               hasWind,  wMean, wStd, wMAD,
                               hasTemp,  tMean, tStd, tMAD,
                               hasSolar, solar);
        }
    }

    file.close();
    output.DisplayFileWritten(OUTPUT_FILENAME);
}

void MenuHandler::DisplayGoodbye() const
{
    std::cout << std::endl;
    std::cout << "Exiting program. Goodbye!" << std::endl;
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

int MenuHandler::GetIntInput(const std::string& prompt) const
{
    int value;
    std::cout << prompt;
    while (!(std::cin >> value))
    {
        std::cin.clear();
        ClearInput();
        std::cout << "Invalid input. " << prompt;
    }
    ClearInput();
    return value;
}

int MenuHandler::GetMonthInput() const
{
    int month;
    do
    {
        month = GetIntInput("Enter month (1-12): ");
        if (month < 1 || month > 12)
            std::cout << "Invalid month. Please enter 1-12." << std::endl;
    } while (month < 1 || month > 12);
    return month;
}

int MenuHandler::GetYearInput(const WeatherDataStore& store) const
{
    DisplayAvailableYears(store);
    int year;
    do
    {
        year = GetIntInput("Enter year: ");
        if (!store.YearExists(year))
            std::cout << "Year " << year
                      << " not found. Please try again." << std::endl;
    } while (!store.YearExists(year));
    return year;
}

void MenuHandler::ClearInput() const
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
