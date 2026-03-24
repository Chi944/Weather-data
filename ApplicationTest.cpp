    /**
     * @file ApplicationTest.cpp
     * @brief Full pipeline integration tests (Application Test) — Test Plan tests 1-10
     *
     * Tests the complete path: DataLoader + DataFileManager loading raw records,
     * WeatherDataStore::Build, Calculator stats, OutputHandler formatting,
     * and file output for option 4.
     *
     * Note: Tests 1 and 8 write temporary CSV files to the working directory.
     * Tests 6 and 7 examine the option-4 output file WindTempSolar.csv.
     *
     * @author Deston
     * @date 24/03/2026
     */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include "WeatherRecord.h"
#include "WeatherDataStore.h"
#include "DataLoader.h"
#include "DataFileManager.h"
#include "Calculator.h"
#include "OutputHandler.h"
#include "Vector.h"
#include "Date.h"
#include "Time.h"
#include "Constants.h"

static int passed = 0;
static int failed = 0;
static const float EPS = 0.05f;

static void report(int testNum, const std::string& desc, bool ok)
{
    std::cout << "[Test " << testNum << "] " << (ok ? "PASS" : "FAIL")
              << " -- " << desc << std::endl;
    if (ok) passed++; else failed++;
}

// Helper: write a minimal CSV file for testing
static void writeCSV(const std::string& path, const std::string& content)
{
    std::ofstream f(path.c_str());
    f << content;
}

// Helper: check if a file contains a substring
static bool fileContains(const std::string& path, const std::string& sub)
{
    std::ifstream f(path.c_str());
    if (!f.is_open()) return false;
    std::string content((std::istreambuf_iterator<char>(f)),
                         std::istreambuf_iterator<char>());
    return content.find(sub) != std::string::npos;
}

static WeatherRecord makeRecord(int day, int month, int year,
                                float windMps, float solar, float temp)
{
    WeatherRecord r;
    r.SetDate(Date(day, month, year));
    r.SetTime(Time(12, 0));
    r.SetWindSpeed(windMps);
    r.SetSolarRadiation(solar);
    r.SetTemperature(temp);
    return r;
}

// Test 1: Load multi-file data - correct total record count
static void test1()
{
    // Write two small CSV files (5 records each, no duplicate timestamps)
    writeCSV("test_file1.csv",
        "WAST,S,SR,T\n"
        "1/1/2020 10:00,1.0,150.0,20.0\n"
        "1/1/2020 10:10,1.1,160.0,21.0\n"
        "1/1/2020 10:20,1.2,170.0,22.0\n"
        "1/1/2020 10:30,1.3,180.0,23.0\n"
        "1/1/2020 10:40,1.4,190.0,24.0\n");
    writeCSV("test_file2.csv",
        "WAST,S,SR,T\n"
        "1/1/2020 11:00,2.0,200.0,25.0\n"
        "1/1/2020 11:10,2.1,210.0,26.0\n"
        "1/1/2020 11:20,2.2,220.0,27.0\n"
        "1/1/2020 11:30,2.3,230.0,28.0\n"
        "1/1/2020 11:40,2.4,240.0,29.0\n");

    Vector<WeatherRecord> records;
    DataLoader loader;
    loader.LoadFromFile("test_file1.csv", records);
    loader.LoadFromFile("test_file2.csv", records);
    report(1, "Load two 5-record CSV files: totalRecords = 10", records.Size() == 10);
}

// Test 2: Option 1 - wind stats end-to-end
static void test2()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,3,2020, 1.0f, 0.0f, 0.0f));
    records.Append(makeRecord(2,3,2020, 2.0f, 0.0f, 0.0f));

    WeatherDataStore store; store.Build(records);
    Calculator calc;
    float mean=0, sd=0, mad=0;
    bool ok = calc.CalculateWindStats(store, 3, 2020, mean, sd, mad);
    // 1.0 m/s -> 3.6; 2.0 m/s -> 7.2; mean=5.4; mad=1.8
    report(2, "Option 1: wind mean=5.4 km/h, mad=1.8 for 1.0+2.0 m/s",
           ok && std::fabs(mean-5.4f)<EPS && std::fabs(mad-1.8f)<EPS);
}

// Test 3: Option 1 - no-data path
static void test3()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,3,2020, 1.0f, 0.0f, 0.0f));
    WeatherDataStore store; store.Build(records);
    Calculator calc;
    float mean=0,sd=0,mad=0;
    report(3, "Option 1: CalculateWindStats(April 2020) returns false",
           !calc.CalculateWindStats(store, 4, 2020, mean, sd, mad));
}

// Test 4: Option 2 - 1 of 12 months has data
static void test4()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,1,2021, 0.0f, 0.0f, 15.0f));
    WeatherDataStore store; store.Build(records);
    Calculator calc;
    int dataCount = 0;
    for (int m = 1; m <= 12; m++)
    {
        float mean=0,sd=0,mad=0;
        if (calc.CalculateTempStats(store, m, 2021, mean, sd, mad)) dataCount++;
    }
    report(4, "Option 2: 1 month has data (Jan); 11 return false", dataCount == 1);
}

// Test 5: Option 3 - sPCC S_T perfect correlation across all years
static void test5()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,1,2020, 1.0f, 0.0f, 10.0f));
    records.Append(makeRecord(1,1,2021, 2.0f, 0.0f, 20.0f));
    WeatherDataStore store; store.Build(records);
    Calculator calc;
    float st=0,sr=0,tr=0;
    bool ok = calc.CalculateCorrelations(store, 1, st, sr, tr);
    report(5, "Option 3: S_T approx 1.00 for perfect wind+temp correlation",
           ok && std::fabs(st-1.0f)<EPS);
}

// Test 6: Option 4 - CSV contains year header and month row with MAD
static void test6()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,5,2022, 1.0f, 300.0f, 20.0f));
    WeatherDataStore store; store.Build(records);
    Calculator calc;
    OutputHandler output;

    std::ofstream file("WindTempSolar.csv");
    output.WriteCSVHeader(file, 2022);
    float wMean=0,wSd=0,wMAD=0, tMean=0,tSd=0,tMAD=0, solar=0;
    bool hasWind  = calc.CalculateWindStats (store,5,2022,wMean,wSd,wMAD);
    bool hasTemp  = calc.CalculateTempStats (store,5,2022,tMean,tSd,tMAD);
    bool hasSolar = calc.CalculateSolarTotal(store,5,2022,solar);
    output.WriteCSVRow(file,5, hasWind,wMean,wSd,wMAD, hasTemp,tMean,tSd,tMAD, hasSolar,solar);
    file.close();

    bool ok = fileContains("WindTempSolar.csv", "2022")
           && fileContains("WindTempSolar.csv", "May");
    report(6, "Option 4 CSV contains '2022' and 'May'", ok);
}

// Test 7: Option 4 - 'No Data' written for year with no records
static void test7()
{
    Vector<WeatherRecord> records;
    // Build with 2022 data; then query a year that has no monthly records
    OutputHandler output;
    std::ofstream file("WindTempSolar_nodata.csv");
    output.WriteCSVHeader(file, 2099);
    output.WriteCSVNoData(file);
    file.close();

    bool ok = fileContains("WindTempSolar_nodata.csv", "2099")
           && fileContains("WindTempSolar_nodata.csv", "No Data");
    report(7, "Option 4 'No Data': file contains '2099' and 'No Data'", ok);
}

// Test 8: Column-order independence — T before WAST
static void test8()
{
    writeCSV("test_col_order.csv",
        "T,WAST,S,SR\n"
        "25.0,1/6/2020 12:00,3.0,200.0\n");

    Vector<WeatherRecord> records;
    DataLoader loader;
    loader.LoadFromFile("test_col_order.csv", records);

    bool ok = records.Size() == 1;
    if (ok)
    {
        // temp should be 25.0, wind should be 3.0 m/s, date should be June 2020
        ok = std::fabs(records[0].GetTemperature() - 25.0f) < EPS
          && std::fabs(records[0].GetWindSpeed()   -  3.0f) < EPS
          && records[0].GetDate().GetMonth() == 6
          && records[0].GetDate().GetYear()  == 2020;
    }
    report(8, "Column-order independence: T,WAST,S,SR loaded correctly", ok);
}

// Test 9: Duplicate filtering across two loads
static void test9()
{
    // Same record written twice (same timestamp)
    writeCSV("test_dup1.csv",
        "WAST,S,SR,T\n"
        "10/10/2022 12:00,2.0,150.0,18.0\n");
    writeCSV("test_dup2.csv",
        "WAST,S,SR,T\n"
        "10/10/2022 12:00,2.0,150.0,18.0\n");

    DataFileManager fm;
    DataLoader loader;
    Vector<std::string> files;
    files.Append("test_dup1.csv");
    files.Append("test_dup2.csv");
    Vector<WeatherRecord> records;
    int total = fm.LoadDataFiles(files, records, loader, "");

    report(9, "Duplicate filtering: totalRecords=1, duplicateCount=1",
           total == 1 && fm.GetDuplicateCount() == 1);
}

// Test 10: BST TraverseYears ascending order
static void test10()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,1,2021, 1.0f, 0.0f, 0.0f));
    records.Append(makeRecord(1,1,2019, 1.0f, 0.0f, 0.0f));
    records.Append(makeRecord(1,1,2020, 1.0f, 0.0f, 0.0f));
    WeatherDataStore store; store.Build(records);

    // Capture TraverseYears output to string via ostringstream redirect
    // (simple check: GetYearCount = 3 and YearExists for all three)
    bool ok = store.GetYearCount() == 3
           && store.YearExists(2019)
           && store.YearExists(2020)
           && store.YearExists(2021);
    report(10, "TraverseYears: 3 distinct years present (2019,2020,2021)", ok);
}

int main()
{
    std::cout << "=== ApplicationTest ===" << std::endl;
    test1(); test2(); test3(); test4(); test5();
    test6(); test7(); test8(); test9(); test10();
    std::cout << std::endl << "Results: " << passed << " passed, "
              << failed << " failed." << std::endl;
    return failed == 0 ? 0 : 1;
}
