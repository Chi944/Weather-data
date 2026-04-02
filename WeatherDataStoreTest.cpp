    /**
     * @file WeatherDataStoreTest.cpp
     * @brief Unit tests for WeatherDataStore — Test Plan tests 1-8
     *
     * Tests cover: Build from empty, year BST insert, HasData, GetMonthData,
     * GetMonthDataAllYears, TraverseYears (function pointer), YearExists,
     * and wind speed km/h conversion on insert.
     *
     * @author Deston
     * @date 24/03/2026
     */

#include <iostream>
#include <string>
#include <cmath>
#include "WeatherDataStore.h"
#include "WeatherRecord.h"
#include "Date.h"
#include "Time.h"
#include "Vector.h"

static int passed = 0;
static int failed = 0;
static const float EPSILON = 0.001f;

static void report(int testNum, const std::string& desc, bool ok)
{
    std::cout << "[Test " << testNum << "] " << (ok ? "PASS" : "FAIL")
              << " -- " << desc << std::endl;
    if (ok) passed++; else failed++;
}

// Helper: build a WeatherRecord with specific date/time and sensor values
static WeatherRecord makeRecord(int day, int month, int year,
                                float windMps, float solar, float temp)
{
    WeatherRecord r;
    Date d(day, month, year);
    Time t(12, 0);
    r.SetDate(d);
    r.SetTime(t);
    r.SetWindSpeed(windMps);
    r.SetSolarRadiation(solar);
    r.SetTemperature(temp);
    return r;
}

// Collector for TraverseYears
static Vector<int> g_years;
static void collectYear(int& yr) { g_years.Append(yr); }

// Test 1: Build from empty Vector
static void test1()
{
    Vector<WeatherRecord> records;
    WeatherDataStore s;
    s.Build(records);
    report(1, "Build from empty: IsEmpty()=true", s.IsEmpty());
}

// Test 2: Build with one record — year in BST
static void test2()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1, 3, 2020, 2.0f, 150.0f, 20.0f));
    WeatherDataStore s;
    s.Build(records);
    report(2, "Build with year=2020: YearExists=true, GetYearCount=1",
           s.YearExists(2020) && s.GetYearCount() == 1);
}

// Test 3: HasData true for loaded year/month
static void test3()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1, 5, 2020, 1.0f, 200.0f, 15.0f));
    WeatherDataStore s;
    s.Build(records);
    report(3, "HasData(2020, 5) = true", s.HasData(2020, 5));
}

// Test 4: HasData false for non-existent year
static void test4()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1, 3, 2020, 1.0f, 100.0f, 10.0f));
    WeatherDataStore s;
    s.Build(records);
    report(4, "HasData(1999, 1) = false (year not loaded)", !s.HasData(1999, 1));
}

// Test 5: GetMonthData non-null and correct wind count
static void test5()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1, 3, 2020, 2.0f, 150.0f, 22.0f));
    records.Append(makeRecord(2, 3, 2020, 3.0f, 200.0f, 24.0f));
    WeatherDataStore s;
    s.Build(records);
    const MonthlyData* md = s.GetMonthData(2020, 3);
    bool ok = (md != nullptr) && (md->windSpeeds.Size() == 2);
    report(5, "GetMonthData(2020,3): non-null, windSpeeds.Size()==2", ok);
}

// Test 6: GetMonthDataAllYears collects across two years
static void test6()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1, 1, 2020, 1.0f, 50.0f, 10.0f));
    records.Append(makeRecord(1, 1, 2021, 2.0f, 60.0f, 12.0f));
    WeatherDataStore s;
    s.Build(records);

    Vector<float> winds, temps, solar;
    s.GetMonthDataAllYears(1, winds, temps, solar);
    report(6, "GetMonthDataAllYears(Jan): winds.Size()==2", winds.Size() == 2);
}

// Test 7: TraverseYears via function pointer delivers ascending order
static void test7()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1, 1, 2021, 1.0f, 0.0f, 0.0f));
    records.Append(makeRecord(1, 1, 2019, 1.0f, 0.0f, 0.0f));
    records.Append(makeRecord(1, 1, 2020, 1.0f, 0.0f, 0.0f));
    WeatherDataStore s;
    s.Build(records);

    g_years.Clear();
    s.TraverseYears(collectYear);

    bool ok = g_years.Size() == 3
           && g_years[0] == 2019
           && g_years[1] == 2020
           && g_years[2] == 2021;
    report(7, "TraverseYears: 2019,2020,2021 in ascending order", ok);
}

// Test 8: Wind stored in km/h (conversion on Build)
static void test8()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1, 3, 2020, 1.0f, 0.0f, 0.0f));  // 1.0 m/s
    WeatherDataStore s;
    s.Build(records);

    const MonthlyData* md = s.GetMonthData(2020, 3);
    bool ok = (md != nullptr)
           && (std::fabs(md->windSpeeds[0] - 3.6f) < EPSILON);
    report(8, "Wind converted to km/h on Build: 1.0 m/s -> 3.6 km/h", ok);
}

int main()
{
    std::cout << "=== WeatherDataStoreTest ===" << std::endl;
    test1(); test2(); test3(); test4(); test5(); test6(); test7(); test8();
    std::cout << std::endl << "Results: " << passed << " passed, "
              << failed << " failed." << std::endl;
    return failed == 0 ? 0 : 1;
}
