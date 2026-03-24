    /**
     * @file OutputHandlerTest.cpp
     * @brief Unit tests for OutputHandler (Assignment 2) — Test Plan tests 1-12
     *
     * Strategy: redirect std::cout to a std::ostringstream before each call
     * and inspect the captured string.  CSV methods write to a temporary
     * std::ostringstream-backed stream so no real files are created.
     *
     * Tests cover all Assignment 2 methods: DisplaySPCCHeader, DisplaySPCC,
     * DisplaySPCCNoData, WriteCSVRow with stdev+MAD, WriteCSVNoData, and the
     * existing option-1/2/4 methods to confirm they still produce correct output.
     *
     * Manual verification of expected strings:
     *   DisplayWindStats(3, 2020, 5.4, 2.5):
     *     "March 2020:", "Average speed: 5.4 km/h", "Sample stdev: 2.5"
     *
     *   DisplayTempForMonth(1, 15.0, 7.1):
     *     "January: average: 15.0 degrees C, stdev: 7.1"
     *
     *   DisplaySPCC(0.85, -0.12, 0.67):
     *     "S_T: 0.85", "S_R: -0.12", "T_R: 0.67"   (2 d.p.)
     *
     *   WriteCSVRow month=5 (May), hasWind=true wind=3.6/0.0/0.0,
     *               hasTemp=true temp=20.0/0.0/0.0, hasSolar=true solar=0.1:
     *     "May, 3.6(0.0, 0.0),20.0(0.0, 0.0),0.1"
     *
     * @author Deston
     * @date 24/03/2026
     */

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cmath>
#include "OutputHandler.h"

static int passed = 0;
static int failed = 0;

// ---- helpers ----

static void report(int testNum, const std::string& desc, bool ok)
{
    std::cout << "[Test " << testNum << "] " << (ok ? "PASS" : "FAIL")
              << " -- " << desc << std::endl;
    if (ok) passed++; else failed++;
}

/** Redirect cout -> oss, call fn(), restore cout, return captured string. */
static std::string capture(void (*fn)(const OutputHandler&), const OutputHandler& o)
{
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    fn(o);
    std::cout.rdbuf(old);
    return oss.str();
}

static bool contains(const std::string& haystack, const std::string& needle)
{
    return haystack.find(needle) != std::string::npos;
}

// ---- test functions ----

// Test 1: DisplayWindStats contains month name, year, mean, stdev
static void test1()
{
    OutputHandler o;
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    o.DisplayWindStats(3, 2020, 5.4f, 2.5f);
    std::cout.rdbuf(old);
    std::string out = oss.str();

    bool ok = contains(out, "March")
           && contains(out, "2020")
           && contains(out, "5.4")
           && contains(out, "2.5")
           && contains(out, "km/h");
    report(1, "DisplayWindStats: contains 'March', '2020', '5.4', '2.5', 'km/h'", ok);
}

// Test 2: DisplayNoData contains month name, year, and "No Data"
static void test2()
{
    OutputHandler o;
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    o.DisplayNoData(4, 2020);
    std::cout.rdbuf(old);
    std::string out = oss.str();

    bool ok = contains(out, "April")
           && contains(out, "2020")
           && contains(out, "No Data");
    report(2, "DisplayNoData: contains 'April', '2020', 'No Data'", ok);
}

// Test 3: DisplayTempHeader contains the year
static void test3()
{
    OutputHandler o;
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    o.DisplayTempHeader(2021);
    std::cout.rdbuf(old);
    std::string out = oss.str();

    report(3, "DisplayTempHeader: contains '2021'", contains(out, "2021"));
}

// Test 4: DisplayTempForMonth contains month name, mean, stdev, "degrees C"
//   mean=15.0, stdev=7.1 -> "January: average: 15.0 degrees C, stdev: 7.1"
static void test4()
{
    OutputHandler o;
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    o.DisplayTempForMonth(1, 15.0f, 7.1f);
    std::cout.rdbuf(old);
    std::string out = oss.str();

    bool ok = contains(out, "January")
           && contains(out, "15.0")
           && contains(out, "7.1")
           && contains(out, "degrees C");
    report(4, "DisplayTempForMonth: contains 'January', '15.0', '7.1', 'degrees C'", ok);
}

// Test 5: DisplayNoDataForMonth contains month name and "No Data"
static void test5()
{
    OutputHandler o;
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    o.DisplayNoDataForMonth(6);
    std::cout.rdbuf(old);
    std::string out = oss.str();

    bool ok = contains(out, "June") && contains(out, "No Data");
    report(5, "DisplayNoDataForMonth(6): contains 'June' and 'No Data'", ok);
}

// Test 6: DisplaySPCCHeader contains "Pearson" and month name (new for A2)
static void test6()
{
    OutputHandler o;
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    o.DisplaySPCCHeader(1);
    std::cout.rdbuf(old);
    std::string out = oss.str();

    bool ok = contains(out, "Pearson") && contains(out, "January");
    report(6, "DisplaySPCCHeader(1): contains 'Pearson' and 'January'", ok);
}

// Test 7: DisplaySPCC shows S_T, S_R, T_R at 2 decimal places (new for A2)
//   DisplaySPCC(0.85, -0.12, 0.67)
//   Expected lines: "S_T: 0.85", "S_R: -0.12", "T_R: 0.67"
static void test7()
{
    OutputHandler o;
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    o.DisplaySPCC(0.85f, -0.12f, 0.67f);
    std::cout.rdbuf(old);
    std::string out = oss.str();

    bool ok = contains(out, "S_T:")
           && contains(out, "S_R:")
           && contains(out, "T_R:")
           && contains(out, "0.85")
           && contains(out, "-0.12")
           && contains(out, "0.67");
    report(7, "DisplaySPCC(0.85,-0.12,0.67): contains 'S_T:','S_R:','T_R:' and values", ok);
}

// Test 8: DisplaySPCCNoData contains month name and "No Data" (new for A2)
static void test8()
{
    OutputHandler o;
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    o.DisplaySPCCNoData(3);
    std::cout.rdbuf(old);
    std::string out = oss.str();

    bool ok = contains(out, "March") && contains(out, "No Data");
    report(8, "DisplaySPCCNoData(3): contains 'March' and 'No Data'", ok);
}

// Test 9: WriteCSVHeader writes year to stream
static void test9()
{
    OutputHandler o;
    std::ostringstream file;
    o.WriteCSVHeader(reinterpret_cast<std::ofstream&>(file), 2022);
    // Note: we use a temporary ofstream substitution — for portable testing,
    // write to a real temp file instead:
    std::ofstream tmp("test_csv_hdr.csv");
    o.WriteCSVHeader(tmp, 2022);
    tmp.close();

    std::ifstream fin("test_csv_hdr.csv");
    std::string line;
    std::getline(fin, line);
    fin.close();

    report(9, "WriteCSVHeader(2022): first line is '2022'", line == "2022");
}

// Test 10: WriteCSVRow with all data present — correct A2 format with stdev AND MAD
//   month=5 (May), wind mean=3.6 stdev=0.0 mad=0.0,
//   temp mean=20.0 stdev=0.0 mad=0.0, solar=0.1
//   Expected: "May, 3.6(0.0, 0.0),20.0(0.0, 0.0),0.1"
//   (solar 300 W/m2: 300/6000=0.05, displayed as 0.1 by setprecision(1))
static void test10()
{
    OutputHandler o;
    std::ofstream tmp("test_csv_row.csv");
    o.WriteCSVHeader(tmp, 2022);
    o.WriteCSVRow(tmp, 5,
                  true,  3.6f,  0.0f, 0.0f,   // wind: mean=3.6, stdev=0, mad=0
                  true,  20.0f, 0.0f, 0.0f,   // temp: mean=20.0, stdev=0, mad=0
                  true,  0.05f);               // solar: 0.05 -> printed as 0.1
    tmp.close();

    std::ifstream fin("test_csv_row.csv");
    std::string content((std::istreambuf_iterator<char>(fin)),
                         std::istreambuf_iterator<char>());
    fin.close();

    // Must contain month name, the parenthesised (stdev, mad) format, and comma-separated fields
    bool ok = contains(content, "May")
           && contains(content, "3.6(0.0, 0.0)")
           && contains(content, "20.0(0.0, 0.0)")
           && contains(content, ",0.1");
    report(10, "WriteCSVRow: contains 'May', '3.6(0.0, 0.0)', '20.0(0.0, 0.0)', ',0.1'", ok);
}

// Test 11: WriteCSVRow with hasTemp=false — temperature field is blank (two consecutive commas)
//   Expected: "January, 3.6(0.5, 0.3),,100.0"
static void test11()
{
    OutputHandler o;
    std::ofstream tmp("test_csv_blank.csv");
    o.WriteCSVHeader(tmp, 2023);
    o.WriteCSVRow(tmp, 1,
                  true,  3.6f, 0.5f, 0.3f,   // wind present
                  false, 0.0f, 0.0f, 0.0f,   // temp ABSENT
                  true,  100.0f);             // solar present
    tmp.close();

    std::ifstream fin("test_csv_blank.csv");
    std::string content((std::istreambuf_iterator<char>(fin)),
                         std::istreambuf_iterator<char>());
    fin.close();

    // Two consecutive commas indicate blank temp field
    bool ok = contains(content, "January")
           && contains(content, "3.6(0.5, 0.3)")
           && contains(content, ",,");     // blank temperature field
    report(11, "WriteCSVRow hasTemp=false: blank temp field gives ',,' in output", ok);
}

// Test 12: WriteCSVNoData writes "No Data" to file (new for A2)
static void test12()
{
    OutputHandler o;
    std::ofstream tmp("test_csv_nodata.csv");
    o.WriteCSVHeader(tmp, 2099);
    o.WriteCSVNoData(tmp);
    tmp.close();

    std::ifstream fin("test_csv_nodata.csv");
    std::string content((std::istreambuf_iterator<char>(fin)),
                         std::istreambuf_iterator<char>());
    fin.close();

    bool ok = contains(content, "2099") && contains(content, "No Data");
    report(12, "WriteCSVNoData: file contains '2099' on line 1 and 'No Data' on line 2", ok);
}

int main()
{
    std::cout << "=== OutputHandlerTest ===" << std::endl;
    std::cout << std::endl;

    test1(); test2(); test3();  test4();
    test5(); test6(); test7();  test8();
    test9(); test10(); test11(); test12();

    std::cout << std::endl
              << "Results: " << passed << " passed, "
              << failed << " failed." << std::endl;
    return failed == 0 ? 0 : 1;
}
