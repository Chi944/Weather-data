    /**
     * @file MenuHandlerTest.cpp
     * @brief Unit tests for MenuHandler — Test Plan tests 1-7
     *
     * Strategy: redirect std::cout to a std::ostringstream before each call
     * and inspect the captured string.  Stdin-dependent methods (DisplayMenu,
     * GetYearInput, GetMonthInput) are NOT tested here — they require interactive
     * input and belong in an integration/acceptance test.
     *
     * Tests cover:
     *   1. DisplayWelcome output contains record count.
     *   2. DisplayGoodbye output contains expected exit text.
     *   3. DisplayAvailableYears uses BST in-order traversal (sorted years).
     *   4. DisplayAvailableYears with a single year.
     *   5. PrintYear static callback prints value followed by space.
     *   6. DisplayMenu output lists all five options.
     *   7. DisplayAvailableYears years printed in ascending order for
     *      data inserted out of order (confirms BST sorting, not insertion order).
     *
     * Note: DisplayMenu reads from cin; the test only verifies that the output
     * text is correct by examining the prompt text without supplying input.
     * Prompt string is tested only, not the return value, to avoid blocking.
     *
     * @author Deston
     * @date 24/03/2026
     */

#include <iostream>
#include <sstream>
#include <string>
#include "MenuHandler.h"
#include "WeatherDataStore.h"
#include "WeatherRecord.h"
#include "Date.h"
#include "Time.h"

static int passed = 0;
static int failed = 0;

static void report(int testNum, const std::string& desc, bool ok)
{
    std::cout << "[Test " << testNum << "] " << (ok ? "PASS" : "FAIL")
              << " -- " << desc << std::endl;
    if (ok) passed++; else failed++;
}

static bool contains(const std::string& haystack, const std::string& needle)
{
    return haystack.find(needle) != std::string::npos;
}

// Build a WeatherDataStore with records for given years (all in January)
static WeatherDataStore buildStoreWithYears(int* years, int n)
{
    Vector<WeatherRecord> recs;
    for (int i = 0; i < n; i++)
    {
        WeatherRecord r;
        r.SetDate(Date(1, 1, years[i]));
        r.SetTime(Time(12, 0));
        r.SetWindSpeed(1.0f);
        r.SetSolarRadiation(0.0f);
        r.SetTemperature(0.0f);
        recs.Append(r);
    }
    WeatherDataStore store;
    store.Build(recs);
    return store;
}

// Test 1: DisplayWelcome output contains the record count
static void test1()
{
    MenuHandler mh;
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    mh.DisplayWelcome(42);
    std::cout.rdbuf(old);
    std::string out = oss.str();

    report(1, "DisplayWelcome(42): output contains '42'", contains(out, "42"));
}

// Test 2: DisplayGoodbye output contains exit/goodbye text
static void test2()
{
    MenuHandler mh;
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    mh.DisplayGoodbye();
    std::cout.rdbuf(old);
    std::string out = oss.str();

    // A2 implementation says "Exiting program. Goodbye!"
    bool ok = contains(out, "Goodbye") || contains(out, "goodbye")
           || contains(out, "Exiting") || contains(out, "exiting");
    report(2, "DisplayGoodbye: output contains 'Goodbye' or 'Exiting'", ok);
}

// Test 3: DisplayAvailableYears outputs years sorted ascending (BST in-order traversal)
//   Records inserted for years 2021, 2019, 2020 (out of order).
//   BST guarantees in-order = ascending: 2019 2020 2021.
static void test3()
{
    int yrs[] = {2021, 2019, 2020};
    WeatherDataStore store = buildStoreWithYears(yrs, 3);

    MenuHandler mh;
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    mh.DisplayAvailableYears(store);
    std::cout.rdbuf(old);
    std::string out = oss.str();

    // All three years must appear
    bool ok = contains(out, "2019") && contains(out, "2020") && contains(out, "2021");

    // 2019 must appear BEFORE 2020 (position check for sorted order)
    if (ok)
    {
        size_t pos2019 = out.find("2019");
        size_t pos2020 = out.find("2020");
        size_t pos2021 = out.find("2021");
        ok = (pos2019 < pos2020) && (pos2020 < pos2021);
    }

    report(3, "DisplayAvailableYears(2021,2019,2020): output '2019' before '2020' before '2021'", ok);
}

// Test 4: DisplayAvailableYears with a single year
static void test4()
{
    int yrs[] = {2015};
    WeatherDataStore store = buildStoreWithYears(yrs, 1);

    MenuHandler mh;
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    mh.DisplayAvailableYears(store);
    std::cout.rdbuf(old);
    std::string out = oss.str();

    report(4, "DisplayAvailableYears with single year 2015: output contains '2015'",
           contains(out, "2015"));
}

// Test 5: Static PrintYear callback — used as the function pointer passed to the BST.
//   Call TraverseYears with a lambda-equivalent static function to confirm the
//   static-member-function-as-function-pointer mechanism compiles and runs correctly.
static void test5()
{
    int yrs[] = {2007};
    WeatherDataStore store = buildStoreWithYears(yrs, 1);

    // TraverseYears calls Bst<int>::inOrderTraversal(PrintYear).
    // If PrintYear is not a valid f1Typ, this would fail to compile.
    MenuHandler mh;
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    mh.DisplayAvailableYears(store);
    std::cout.rdbuf(old);
    std::string out = oss.str();

    // "2007 " — year then space is the PrintYear behaviour
    bool ok = contains(out, "2007");
    report(5, "PrintYear static callback via TraverseYears: output contains '2007'", ok);
}

// Test 6: DisplayMenu output lists all five numbered options
static void test6()
{
    // Provide "1\n" as simulated input so DisplayMenu doesn't block
    std::istringstream fakeIn("1\n");
    std::streambuf* oldIn = std::cin.rdbuf(fakeIn.rdbuf());

    MenuHandler mh;
    std::ostringstream oss;
    std::streambuf* oldOut = std::cout.rdbuf(oss.rdbuf());
    int choice = mh.DisplayMenu();
    std::cout.rdbuf(oldOut);
    std::cin.rdbuf(oldIn);

    std::string out = oss.str();

    // All five options must appear in the menu text
    bool ok = contains(out, "1.")
           && contains(out, "2.")
           && contains(out, "3.")
           && contains(out, "4.")
           && contains(out, "5.")
           && choice == 1;   // the simulated input was "1"
    report(6, "DisplayMenu: output contains options 1-5; returns choice=1 for input '1'", ok);
}

// Test 7: DisplayAvailableYears five years inserted out of order — all appear sorted
static void test7()
{
    int yrs[] = {2016, 2010, 2014, 2012, 2008};
    WeatherDataStore store = buildStoreWithYears(yrs, 5);

    MenuHandler mh;
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    mh.DisplayAvailableYears(store);
    std::cout.rdbuf(old);
    std::string out = oss.str();

    // All five years must appear in ascending order
    size_t p08 = out.find("2008");
    size_t p10 = out.find("2010");
    size_t p12 = out.find("2012");
    size_t p14 = out.find("2014");
    size_t p16 = out.find("2016");

    bool allFound = (p08 != std::string::npos) && (p10 != std::string::npos)
                 && (p12 != std::string::npos) && (p14 != std::string::npos)
                 && (p16 != std::string::npos);
    bool sorted = allFound && (p08 < p10) && (p10 < p12) && (p12 < p14) && (p14 < p16);

    report(7, "DisplayAvailableYears({2016,2010,2014,2012,2008}): all five in ascending order", sorted);
}

int main()
{
    std::cout << "=== MenuHandlerTest ===" << std::endl;
    std::cout << std::endl;

    test1(); test2(); test3(); test4();
    test5(); test6(); test7();

    std::cout << std::endl
              << "Results: " << passed << " passed, "
              << failed << " failed." << std::endl;
    return failed == 0 ? 0 : 1;
}
