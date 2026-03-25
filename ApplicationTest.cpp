    /**
     * @file ApplicationTest.cpp
     * @brief Comprehensive full-pipeline integration tests — Tests 1-25
     *
     * Tests the complete path from CSV file I/O through DataLoader,
     * DataFileManager (with Bst<long long> duplicate detection),
     * WeatherDataStore::Build, Calculator statistics, and
     * OutputHandler formatting.  Every expected numeric value is
     * manually derived below.
     *
     * @author Deston
     * @date 25/03/2026
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
#include "Bst.h"

static int passed = 0;
static int failed = 0;

// Tight epsilon — all expected values are exact to >= 3 decimal places
static const float EPS = 0.005f;

static void report(int testNum, const std::string& desc, bool ok)
{
    std::cout << "[Test " << testNum << "] " << (ok ? "PASS" : "FAIL")
              << " -- " << desc << std::endl;
    if (ok) passed++; else failed++;
}

// File-scope function pointer for TraverseYears output capture (Test 20)
// Signature matches Bst<int>::f1Typ: void (*)(int&)
static void printYearHelper(int& yr) { std::cout << yr << " "; }

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void writeCSV(const std::string& path, const std::string& content)
{
    std::ofstream f(path.c_str());
    f << content;
}

static std::string fileRead(const std::string& path)
{
    std::ifstream f(path.c_str());
    if (!f.is_open()) return "";
    return std::string((std::istreambuf_iterator<char>(f)),
                        std::istreambuf_iterator<char>());
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

// ============================================================
// Test 1: Multi-file load — total record count and file counters
//   2 CSV files, 5 records each, no duplicate timestamps.
//   Manual: 5 + 5 = 10 unique; 2 files loaded; 0 duplicates.
// ============================================================
static void test1()
{
    writeCSV("app_file1.csv",
        "WAST,S,SR,T\n"
        "1/1/2020 10:00,1.0,150.0,20.0\n"
        "1/1/2020 10:10,1.1,160.0,21.0\n"
        "1/1/2020 10:20,1.2,170.0,22.0\n"
        "1/1/2020 10:30,1.3,180.0,23.0\n"
        "1/1/2020 10:40,1.4,190.0,24.0\n");
    writeCSV("app_file2.csv",
        "WAST,S,SR,T\n"
        "1/1/2020 11:00,2.0,200.0,25.0\n"
        "1/1/2020 11:10,2.1,210.0,26.0\n"
        "1/1/2020 11:20,2.2,220.0,27.0\n"
        "1/1/2020 11:30,2.3,230.0,28.0\n"
        "1/1/2020 11:40,2.4,240.0,29.0\n");

    DataFileManager fm;
    DataLoader loader;
    Vector<std::string> files;
    files.Append("app_file1.csv");
    files.Append("app_file2.csv");
    Vector<WeatherRecord> records;
    int total = fm.LoadDataFiles(files, records, loader, "");

    report(1, "Multi-file load: totalRecords=10; 2/2 files; 0 duplicates",
           total == 10
           && fm.GetFilesLoaded()    == 2
           && fm.GetDuplicateCount() == 0);
}

// ============================================================
// Test 2: Option 1 — wind mean, stdev, MAD all verified
//   [1.0, 2.0] m/s -> [3.6, 7.2] km/h
//   mean=5.4; stdev=sqrt(6.48)=2.546; MAD=1.8
// ============================================================
static void test2()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,3,2020, 1.0f, 0.0f, 0.0f));
    records.Append(makeRecord(2,3,2020, 2.0f, 0.0f, 0.0f));
    WeatherDataStore store; store.Build(records);
    Calculator calc;
    float mean=0, sd=0, mad=0;
    bool ok = calc.CalculateWindStats(store, 3, 2020, mean, sd, mad);

    report(2, "Option 1: mean=5.4 km/h, stdev=2.546, MAD=1.8  [manual: sqrt(6.48)=2.546]",
           ok
           && std::fabs(mean - 5.4f)   < EPS
           && std::fabs(sd   - 2.546f) < EPS
           && std::fabs(mad  - 1.8f)   < EPS);
}

// ============================================================
// Test 3: Option 1 — no-data path for absent month/year
// ============================================================
static void test3()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,3,2020, 1.0f, 0.0f, 0.0f));
    WeatherDataStore store; store.Build(records);
    Calculator calc;
    float mean=0,sd=0,mad=0;
    report(3, "Option 1: CalculateWindStats(April 2020) returns false [no data]",
           !calc.CalculateWindStats(store, 4, 2020, mean, sd, mad));
}

// ============================================================
// Test 4: Option 2 — temperature mean, stdev, MAD verified
//   [10, 20, 30] C; mean=20; stdev=10; MAD=6.667
//   Manual: sum_sq=200; var=200/(3-1)=100; stdev=sqrt(100)=10
//           MAD=(10+0+10)/3=6.667
// ============================================================
static void test4()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,6,2021, 0.0f, 0.0f, 10.0f));
    records.Append(makeRecord(2,6,2021, 0.0f, 0.0f, 20.0f));
    records.Append(makeRecord(3,6,2021, 0.0f, 0.0f, 30.0f));
    WeatherDataStore store; store.Build(records);
    Calculator calc;
    float mean=0,sd=0,mad=0;
    bool ok = calc.CalculateTempStats(store, 6, 2021, mean, sd, mad);

    report(4, "Option 2: mean=20.0, stdev=10.0, MAD=6.667  [manual: sqrt(100)=10]",
           ok
           && std::fabs(mean - 20.0f)  < EPS
           && std::fabs(sd   - 10.0f)  < EPS
           && std::fabs(mad  - 6.667f) < EPS);
}

// ============================================================
// Test 5: Option 2 — full 12-month iteration; 1 has data, 11 do not
// ============================================================
static void test5()
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
    report(5, "Option 2: 1 month (Jan) has data; 11 return false",
           dataCount == 1);
}

// ============================================================
// Test 6: Option 3 — S_T=1.0, S_R=0, T_R=0 (no daytime solar records)
//   Solar is 0.0 W/m2 (below 100 threshold) so S_R and T_R cannot
//   be computed — both returned as 0.0.
// ============================================================
static void test6()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,1,2020, 1.0f, 0.0f, 10.0f));
    records.Append(makeRecord(1,1,2021, 2.0f, 0.0f, 20.0f));
    WeatherDataStore store; store.Build(records);
    Calculator calc;
    float st=0,sr=0,tr=0;
    bool ok = calc.CalculateCorrelations(store, 1, st, sr, tr);
    report(6, "Option 3: S_T=1.0; S_R=0 and T_R=0 (no solar >= 100 W/m2)",
           ok
           && std::fabs(st - 1.0f) < EPS
           && std::fabs(sr - 0.0f) < EPS
           && std::fabs(tr - 0.0f) < EPS);
}

// ============================================================
// Test 7: Option 3 — S_T, S_R, T_R all 1.0 with daytime solar
//   A: wind=1.0 m/s, temp=10 C, solar=100 W/m2
//   B: wind=2.0 m/s, temp=20 C, solar=200 W/m2
//   winds=[3.6,7.2]; temps=[10,20]; solar_kwh=[0.01667,0.03333]
//   All three pairs perfectly correlated => S_T=S_R=T_R=1.0
// ============================================================
static void test7()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,1,2020, 1.0f, 100.0f, 10.0f));
    records.Append(makeRecord(1,1,2021, 2.0f, 200.0f, 20.0f));
    WeatherDataStore store; store.Build(records);
    Calculator calc;
    float st=0,sr=0,tr=0;
    bool ok = calc.CalculateCorrelations(store, 1, st, sr, tr);
    report(7, "Option 3: S_T=S_R=T_R=1.0 (wind, temp, solar all correlated)  [manual verified]",
           ok
           && std::fabs(st - 1.0f) < EPS
           && std::fabs(sr - 1.0f) < EPS
           && std::fabs(tr - 1.0f) < EPS);
}

// ============================================================
// Test 8: Option 3 — no-data path (empty store)
// ============================================================
static void test8()
{
    WeatherDataStore store;
    Calculator calc;
    float st=0,sr=0,tr=0;
    report(8, "Option 3: CalculateCorrelations on empty store returns false",
           !calc.CalculateCorrelations(store, 1, st, sr, tr));
}

// ============================================================
// Test 9: Option 4 — exact CSV row values verified
//   1 record: wind=1.0 m/s, temp=20.0 C, solar=300 W/m2, May 2022
//   Single record: stdev=0.0, MAD=0.0
//   wind mean=3.6; solar=0.05->setprecision(1)->"0.1"
//   Expected row: "May, 3.6(0.0, 0.0),20.0(0.0, 0.0),0.1"
// ============================================================
static void test9()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,5,2022, 1.0f, 300.0f, 20.0f));
    WeatherDataStore store; store.Build(records);
    Calculator calc;
    OutputHandler output;

    std::ofstream f("app_opt4.csv");
    output.WriteCSVHeader(f, 2022);
    float wMean=0,wSd=0,wMAD=0, tMean=0,tSd=0,tMAD=0, solar=0;
    bool hasWind  = calc.CalculateWindStats (store,5,2022,wMean,wSd,wMAD);
    bool hasTemp  = calc.CalculateTempStats (store,5,2022,tMean,tSd,tMAD);
    bool hasSolar = calc.CalculateSolarTotal(store,5,2022,solar);
    output.WriteCSVRow(f,5, hasWind,wMean,wSd,wMAD, hasTemp,tMean,tSd,tMAD, hasSolar,solar);
    f.close();

    std::string content = fileRead("app_opt4.csv");
    bool ok = content.find("2022")           != std::string::npos
           && content.find("May")            != std::string::npos
           && content.find("3.6(0.0, 0.0)")  != std::string::npos
           && content.find("20.0(0.0, 0.0)") != std::string::npos
           && content.find(",0.1")           != std::string::npos;
    report(9, "Option 4 CSV: '2022','May','3.6(0.0, 0.0)','20.0(0.0, 0.0)',',0.1'",
           ok);
}

// ============================================================
// Test 10: Option 4 — blank temperature field gives ",," in CSV
//   Per A2 spec: "if data not available for a field, output field is blank"
// ============================================================
static void test10()
{
    OutputHandler output;
    std::ofstream f("app_blank.csv");
    output.WriteCSVHeader(f, 2023);
    output.WriteCSVRow(f, 1,
                       true,  3.6f, 0.5f, 0.3f,
                       false, 0.0f, 0.0f, 0.0f,
                       true,  100.0f);
    f.close();

    std::string content = fileRead("app_blank.csv");
    bool ok = content.find("January") != std::string::npos
           && content.find(",,")      != std::string::npos;
    report(10, "Option 4 blank field: hasTemp=false -> ',,' in CSV (per A2 spec)",
           ok);
}

// ============================================================
// Test 11: Option 4 — "No Data" written when entire year has no records
//   Per A2 spec: "output year on first line and 'No Data' on second line"
// ============================================================
static void test11()
{
    OutputHandler output;
    std::ofstream f("app_nodata.csv");
    output.WriteCSVHeader(f, 2099);
    output.WriteCSVNoData(f);
    f.close();

    std::string content = fileRead("app_nodata.csv");
    bool ok = content.find("2099")    != std::string::npos
           && content.find("No Data") != std::string::npos;
    report(11, "Option 4 'No Data': year on line 1, 'No Data' on line 2 (per A2 spec)",
           ok);
}

// ============================================================
// Test 12: Solar threshold boundary — 99 W/m2 excluded, 100 W/m2 included
//   Manual: 99 < 100 -> excluded (returns false)
//           100/6000 = 0.01667 kWh/m2 (returns true)
// ============================================================
static void test12()
{
    Vector<WeatherRecord> r99;
    r99.Append(makeRecord(1,4,2022, 0.0f, 99.0f, 0.0f));
    WeatherDataStore s99; s99.Build(r99);
    Calculator calc;
    float t99=0;
    bool excluded = !calc.CalculateSolarTotal(s99, 4, 2022, t99);

    Vector<WeatherRecord> r100;
    r100.Append(makeRecord(1,4,2023, 0.0f, 100.0f, 0.0f));
    WeatherDataStore s100; s100.Build(r100);
    float t100=0;
    bool included = calc.CalculateSolarTotal(s100, 4, 2023, t100);

    report(12, "Solar threshold: 99 W/m2 excluded; 100 W/m2 included (0.01667 kWh/m2)  [manual]",
           excluded && included && std::fabs(t100 - 100.0f/6000.0f) < EPS);
}

// ============================================================
// Test 13: Solar conversion — 300 W/m2 -> 0.05 kWh/m2
//   Manual: 300 / 6000 = 0.05
// ============================================================
static void test13()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,3,2020, 0.0f, 300.0f, 0.0f));
    WeatherDataStore store; store.Build(records);
    Calculator calc;
    float total=0;
    bool ok = calc.CalculateSolarTotal(store, 3, 2020, total);
    report(13, "Solar conversion: 300 W/m2 -> 0.05 kWh/m2  [manual: 300/6000=0.05]",
           ok && std::fabs(total - 0.05f) < EPS);
}

// ============================================================
// Test 14: Wind conversion — 10.0 m/s -> 36.0 km/h
//   Manual: 10.0 * 3.6 = 36.0
// ============================================================
static void test14()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,2,2020, 10.0f, 0.0f, 0.0f));
    WeatherDataStore store; store.Build(records);
    Calculator calc;
    float mean=0,sd=0,mad=0;
    bool ok = calc.CalculateWindStats(store, 2, 2020, mean, sd, mad);
    report(14, "Wind conversion: 10.0 m/s -> mean=36.0 km/h  [manual: 10*3.6=36]",
           ok && std::fabs(mean - 36.0f) < EPS);
}

// ============================================================
// Test 15: Negative temperatures — mean, stdev, MAD correct
//   [-5.0, -3.0] C; mean=-4.0; stdev=sqrt(2)=1.414; MAD=1.0
// ============================================================
static void test15()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,7,2022, 0.0f, 0.0f, -5.0f));
    records.Append(makeRecord(2,7,2022, 0.0f, 0.0f, -3.0f));
    WeatherDataStore store; store.Build(records);
    Calculator calc;
    float mean=0,sd=0,mad=0;
    bool ok = calc.CalculateTempStats(store, 7, 2022, mean, sd, mad);
    report(15, "Negative temps: mean=-4.0, stdev=1.414, MAD=1.0  [manual: sqrt(2)=1.414]",
           ok
           && std::fabs(mean - (-4.0f)) < EPS
           && std::fabs(sd   - 1.414f)  < EPS
           && std::fabs(mad  - 1.0f)    < EPS);
}

// ============================================================
// Test 16: Leap year — 29/2/2020 parsed and stored correctly
//   2020 is a leap year (divisible by 4, not by 100).
// ============================================================
static void test16()
{
    writeCSV("app_leapyear.csv",
        "WAST,S,SR,T\n"
        "29/2/2020 12:00,1.5,150.0,12.0\n");

    Vector<WeatherRecord> records;
    DataLoader loader;
    bool loaded = loader.LoadFromFile("app_leapyear.csv", records);

    bool ok = loaded
           && records.Size() == 1
           && records[0].GetDate().GetDay()   == 29
           && records[0].GetDate().GetMonth() == 2
           && records[0].GetDate().GetYear()  == 2020;
    report(16, "Leap year 29/2/2020: parsed correctly, record stored",
           ok);
}

// ============================================================
// Test 17: Column-order independence — T, WAST, S, SR
//   DataLoader detects column positions dynamically from the header row.
// ============================================================
static void test17()
{
    writeCSV("app_colorder.csv",
        "T,WAST,S,SR\n"
        "25.0,1/6/2020 12:00,3.0,200.0\n");

    Vector<WeatherRecord> records;
    DataLoader loader;
    loader.LoadFromFile("app_colorder.csv", records);

    bool ok = records.Size() == 1
           && std::fabs(records[0].GetTemperature() - 25.0f) < EPS
           && std::fabs(records[0].GetWindSpeed()   -  3.0f) < EPS
           && records[0].GetDate().GetMonth() == 6
           && records[0].GetDate().GetYear()  == 2020;
    report(17, "Column-order T,WAST,S,SR: temp=25.0, wind=3.0 m/s, June 2020",
           ok);
}

// ============================================================
// Test 18: Cross-file duplicate filtering via Bst<long long>
//   2 files, same timestamp in each.
//   Expected: totalRecords=1; duplicateCount=1.
// ============================================================
static void test18()
{
    writeCSV("app_dup1.csv",
        "WAST,S,SR,T\n"
        "10/10/2022 12:00,2.0,150.0,18.0\n");
    writeCSV("app_dup2.csv",
        "WAST,S,SR,T\n"
        "10/10/2022 12:00,2.0,150.0,18.0\n");

    DataFileManager fm;
    DataLoader loader;
    Vector<std::string> files;
    files.Append("app_dup1.csv");
    files.Append("app_dup2.csv");
    Vector<WeatherRecord> records;
    int total = fm.LoadDataFiles(files, records, loader, "");

    report(18, "Cross-file duplicate (Bst<long long>): totalRecords=1, duplicateCount=1",
           total == 1 && fm.GetDuplicateCount() == 1);
}

// ============================================================
// Test 19: Intra-file duplicate — same timestamp twice in one file
//   3 rows: rows 1 and 2 share timestamp, row 3 is distinct.
//   Expected: totalRecords=2; duplicateCount=1.
// ============================================================
static void test19()
{
    writeCSV("app_intradups.csv",
        "WAST,S,SR,T\n"
        "1/3/2021 09:00,1.0,100.0,15.0\n"
        "1/3/2021 09:00,1.0,100.0,15.0\n"   // duplicate
        "1/3/2021 10:00,2.0,150.0,16.0\n"); // distinct

    DataFileManager fm;
    DataLoader loader;
    Vector<std::string> files;
    files.Append("app_intradups.csv");
    Vector<WeatherRecord> records;
    int total = fm.LoadDataFiles(files, records, loader, "");

    report(19, "Intra-file duplicate: 3 rows, 1 dup -> totalRecords=2, duplicateCount=1",
           total == 2 && fm.GetDuplicateCount() == 1);
}

// ============================================================
// Test 20: BST TraverseYears — sorted output captured via cout redirect
//   Insert years 2021, 2019, 2020 out of order.
//   BST in-order = ascending: "2019" must appear before "2020" before "2021".
// ============================================================
static void test20()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,1,2021, 1.0f, 0.0f, 0.0f));
    records.Append(makeRecord(1,1,2019, 1.0f, 0.0f, 0.0f));
    records.Append(makeRecord(1,1,2020, 1.0f, 0.0f, 0.0f));
    WeatherDataStore store; store.Build(records);

    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    store.TraverseYears(printYearHelper);
    std::cout.rdbuf(old);
    std::string out = oss.str();

    size_t p19 = out.find("2019");
    size_t p20 = out.find("2020");
    size_t p21 = out.find("2021");

    bool ok = (p19 != std::string::npos)
           && (p20 != std::string::npos)
           && (p21 != std::string::npos)
           && (p19 < p20) && (p20 < p21);
    report(20, "BST TraverseYears: '2019' before '2020' before '2021' in output",
           ok);
}

// ============================================================
// Test 21: DataLoader — non-existent file returns false, error set
// ============================================================
static void test21()
{
    DataLoader loader;
    Vector<WeatherRecord> records;
    bool ok = !loader.LoadFromFile("does_not_exist_xyz.csv", records);
    report(21, "DataLoader non-existent file: returns false; GetLastError() non-empty",
           ok && !loader.GetLastError().empty());
}

// ============================================================
// Test 22: DataFileManager::ReadDataSourceFile — non-existent config returns 0
// ============================================================
static void test22()
{
    DataFileManager fm;
    Vector<std::string> filenames;
    int count = fm.ReadDataSourceFile("no_such_source_xyz.txt", filenames);
    report(22, "ReadDataSourceFile non-existent path: returns 0, filenames empty",
           count == 0 && filenames.Size() == 0);
}

// ============================================================
// Test 23: Zero wind speed — valid measurement, not filtered
//   2 records with wind=0.0 m/s; mean=0.0, stdev=0.0, MAD=0.0.
//   Manual: 0.0*3.6=0.0 km/h; mean=0.0; sum_sq_diffs=0; stdev=0.0; MAD=0.0
//   Zero wind must NOT be treated as missing or excluded.
// ============================================================
static void test23()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,1,2023, 0.0f, 0.0f, 0.0f));
    records.Append(makeRecord(2,1,2023, 0.0f, 0.0f, 0.0f));
    WeatherDataStore store; store.Build(records);
    Calculator calc;
    float mean=0,sd=0,mad=0;
    bool ok = calc.CalculateWindStats(store, 1, 2023, mean, sd, mad);
    report(23, "Zero wind: 2 records at 0.0 m/s -> mean=0.0, stdev=0.0, MAD=0.0",
           ok
           && std::fabs(mean) < EPS
           && std::fabs(sd)   < EPS
           && std::fabs(mad)  < EPS);
}

// ============================================================
// Test 24: Multi-year temperature isolation
//   2020/June: temp=10.0 C (1 record) -> mean=10.0, stdev=0.0 (n=1 guard)
//   2021/June: temp=20.0 C (1 record) -> mean=20.0, stdev=0.0
//   Query for 2020 must NOT include 2021 record.
//   If isolation breaks, query 2020 would return mean=15.0 (average of both).
// ============================================================
static void test24()
{
    Vector<WeatherRecord> records;
    records.Append(makeRecord(1,6,2020, 0.0f, 0.0f, 10.0f));
    records.Append(makeRecord(1,6,2021, 0.0f, 0.0f, 20.0f));
    WeatherDataStore store; store.Build(records);
    Calculator calc;

    float mean2020=0,sd2020=0,mad2020=0;
    float mean2021=0,sd2021=0,mad2021=0;
    bool has2020 = calc.CalculateTempStats(store, 6, 2020, mean2020, sd2020, mad2020);
    bool has2021 = calc.CalculateTempStats(store, 6, 2021, mean2021, sd2021, mad2021);

    report(24,
        "Multi-year isolation: 2020/June=10.0 and 2021/June=20.0 are independent",
        has2020 && std::fabs(mean2020 - 10.0f) < EPS
        && has2021 && std::fabs(mean2021 - 20.0f) < EPS);
}

// ============================================================
// Test 25: Bst<long long> ascending stress test — validates iterative fix
//
//   This test inserts 3,000 ascending keys into a Bst<long long>
//   (right-skewed tree of depth 3,000) and verifies:
//     - size() == 3000
//     - search finds first and last inserted key
//     - search correctly returns false for an absent key
//     - copy assignment resets the tree (triggers iterative destroyTree)
//     - tree is empty and size=0 after reset
//
//   With iterative insert/search/destroyTree: passes for any size.
// ============================================================
static void test25()
{
    const int N = 3000;
    Bst<long long> t;

    // Insert N ascending keys: 1, 2, 3, ..., N
    // Produces a completely right-skewed tree of depth N
    for (int i = 1; i <= N; i++)
        t.insert(static_cast<long long>(i));

    bool sizeOk    = (t.size() == N);
    bool searchLo  = t.search(1LL);         // first inserted key
    bool searchHi  = t.search((long long)N);// last inserted key
    bool searchNo  = !t.search((long long)(N + 1)); // absent key

    // Reset via copy assignment — calls iterative destroyTree on N-node tree
    t = Bst<long long>();
    bool emptyAfter = t.isEmpty() && t.size() == 0;

    report(25,
        "Bst<long long> ascending stress test: 3000 ascending inserts, "
        "search/destroyTree iterative — no stack overflow",
        sizeOk && searchLo && searchHi && searchNo && emptyAfter);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main()
{
    std::cout << "=== ApplicationTest (comprehensive — 25 tests) ===" << std::endl;
    std::cout << std::endl;

    test1();  test2();  test3();  test4();  test5();
    test6();  test7();  test8();  test9();  test10();
    test11(); test12(); test13(); test14(); test15();
    test16(); test17(); test18(); test19(); test20();
    test21(); test22(); test23(); test24(); test25();

    std::cout << std::endl
              << "Results: " << passed << " passed, "
              << failed    << " failed." << std::endl;
    return failed == 0 ? 0 : 1;
}
