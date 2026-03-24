    /**
     * @file CalculatorTest.cpp
     * @brief Unit tests for Calculator (Assignment 2) — Test Plan tests 1-6
     *
     * Tests cover: CalculateWindStats with MAD, empty-store guards,
     * CalculateTempStats with MAD, CalculateSolarTotal threshold,
     * CalculateCorrelations empty guard, and CalculateCorrelations S_T near 1.0.
     *
     * @author Deston
     * @date 24/03/2026
     */

#include <iostream>
#include <string>
#include <cmath>
#include "Calculator.h"
#include "WeatherDataStore.h"
#include "WeatherRecord.h"
#include "Date.h"
#include "Time.h"

static int passed = 0;
static int failed = 0;
static const float EPS = 0.01f;

static void report(int testNum, const std::string& desc, bool ok)
{
    std::cout << "[Test " << testNum << "] " << (ok ? "PASS" : "FAIL")
              << " -- " << desc << std::endl;
    if (ok) passed++; else failed++;
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

// Test 1: CalculateWindStats mean, stdev, MAD
static void test1()
{
    // 1.0 m/s -> 3.6 km/h;  2.0 m/s -> 7.2 km/h
    // mean = 5.4;  stdev = sqrt(((3.6-5.4)^2+(7.2-5.4)^2)/1) = sqrt(6.48) ~ 2.546
    // mad = (|3.6-5.4|+|7.2-5.4|)/2 = 1.8
    Vector<WeatherRecord> recs;
    recs.Append(makeRecord(1,3,2020, 1.0f, 0.0f, 0.0f));
    recs.Append(makeRecord(2,3,2020, 2.0f, 0.0f, 0.0f));
    WeatherDataStore s; s.Build(recs);
    Calculator c;
    float mean=0,sd=0,mad=0;
    bool ok = c.CalculateWindStats(s,3,2020,mean,sd,mad);
    report(1, "CalculateWindStats: mean=5.4, stdev~2.546, mad=1.8",
           ok && std::fabs(mean-5.4f)<EPS && std::fabs(sd-2.546f)<EPS && std::fabs(mad-1.8f)<EPS);
}

// Test 2: CalculateWindStats returns false on empty store
static void test2()
{
    WeatherDataStore s;
    Calculator c;
    float mean=0,sd=0,mad=0;
    report(2, "CalculateWindStats on empty store returns false",
           !c.CalculateWindStats(s,1,2020,mean,sd,mad));
}

// Test 3: CalculateTempStats mean, stdev, MAD
static void test3()
{
    // temp 10 and 20: mean=15, stdev=sqrt(50)~7.071, mad=5
    Vector<WeatherRecord> recs;
    recs.Append(makeRecord(1,6,2021, 0.0f, 0.0f, 10.0f));
    recs.Append(makeRecord(2,6,2021, 0.0f, 0.0f, 20.0f));
    WeatherDataStore s; s.Build(recs);
    Calculator c;
    float mean=0,sd=0,mad=0;
    bool ok = c.CalculateTempStats(s,6,2021,mean,sd,mad);
    report(3, "CalculateTempStats: mean=15, stdev~7.071, mad=5.0",
           ok && std::fabs(mean-15.0f)<EPS && std::fabs(sd-7.071f)<EPS && std::fabs(mad-5.0f)<EPS);
}

// Test 4: CalculateSolarTotal excludes below threshold
static void test4()
{
    // solar=50 excluded; solar=150 included -> total=150/6000=0.025
    Vector<WeatherRecord> recs;
    recs.Append(makeRecord(1,4,2022, 0.0f, 50.0f, 0.0f));
    recs.Append(makeRecord(2,4,2022, 0.0f,150.0f, 0.0f));
    WeatherDataStore s; s.Build(recs);
    Calculator c;
    float total=0;
    bool ok = c.CalculateSolarTotal(s,4,2022,total);
    report(4, "CalculateSolarTotal: 50 excluded, 150 included -> 0.025 kWh/m2",
           ok && std::fabs(total-0.025f)<EPS);
}

// Test 5: CalculateCorrelations returns false on empty store
static void test5()
{
    WeatherDataStore s;
    Calculator c;
    float st=0,sr=0,tr=0;
    report(5, "CalculateCorrelations on empty store returns false",
           !c.CalculateCorrelations(s,1,st,sr,tr));
}

// Test 6: CalculateCorrelations S_T near 1.0 for matching wind+temp
static void test6()
{
    // Insert same month across 3 years with wind and temp increasing together
    Vector<WeatherRecord> recs;
    recs.Append(makeRecord(1,1,2020, 1.0f, 0.0f, 10.0f));
    recs.Append(makeRecord(1,1,2021, 2.0f, 0.0f, 20.0f));
    recs.Append(makeRecord(1,1,2022, 3.0f, 0.0f, 30.0f));
    WeatherDataStore s; s.Build(recs);
    Calculator c;
    float st=0,sr=0,tr=0;
    bool ok = c.CalculateCorrelations(s,1,st,sr,tr);
    report(6, "CalculateCorrelations S_T approx 1.0 for perfect wind-temp correlation",
           ok && std::fabs(st-1.0f)<EPS);
}

int main()
{
    std::cout << "=== CalculatorTest (A2) ===" << std::endl;
    test1(); test2(); test3(); test4(); test5(); test6();
    std::cout << std::endl << "Results: " << passed << " passed, "
              << failed << " failed." << std::endl;
    return failed == 0 ? 0 : 1;
}
