    /**
     * @file StatsTest.cpp
     * @brief Unit tests for Stats free functions — Test Plan tests 1-11
     *
     * Tests cover: CalculateMean, CalculateSampleStdDev, CalculateMAD,
     * CalculateSPCC edge cases (n<2, mismatched size, perfect +/- correlation).
     *
     * Manual verification of all expected values:
     *
     *   CalculateMean({2,4,6})      = 12/3 = 4.0
     *
     *   CalculateSampleStdDev({2,4,6}, mean=4):
     *     sum_sq_diffs = (2-4)^2 + (4-4)^2 + (6-4)^2 = 4+0+4 = 8
     *     sample variance = 8 / (3-1) = 8/2 = 4.0
     *     stdev = sqrt(4) = 2.0        <-- NOTE: NOT 2.828
     *     2.828 = sqrt(8) which is wrong (omits the /2 denominator)
     *
     *   CalculateMAD({1,3,5}, mean=3):
     *     MAD = (|1-3| + |3-3| + |5-3|) / 3 = (2+0+2)/3 = 4/3 = 1.333
     *
     *   CalculateSPCC perfect positive {x=y}:
     *     num = sum((xi-mx)(yi-my)) = sum((xi-mx)^2) -> denom = dX = dY
     *     result = 1.0
     *
     *   CalculateSPCC perfect negative {x={1,2,3}, y={3,2,1}}:
     *     mx=2, my=2; num = (-1)(1)+(0)(0)+(1)(-1) = -2
     *     dX=2, dY=2, denom=2; spcc = -2/2 = -1.0
     *
     * @author Deston
     * @date 24/03/2026
     */

#include <iostream>
#include <string>
#include <cmath>
#include "Stats.h"
#include "Vector.h"

static int passed = 0;
static int failed = 0;

static const float EPSILON = 0.001f;

static bool approx(float a, float b)
{
    return std::fabs(a - b) < EPSILON;
}

static void report(int testNum, const std::string& desc, bool ok)
{
    std::cout << "[Test " << testNum << "] " << (ok ? "PASS" : "FAIL")
              << " -- " << desc << std::endl;
    if (ok) passed++; else failed++;
}

// Test 1: CalculateMean on empty Vector returns 0
static void test1()
{
    Vector<float> v;
    report(1, "CalculateMean(empty) = 0.0",
           approx(CalculateMean(v), 0.0f));
}

// Test 2: CalculateMean single element returns that element
static void test2()
{
    Vector<float> v; v.Append(15.0f);
    report(2, "CalculateMean({15.0}) = 15.0",
           approx(CalculateMean(v), 15.0f));
}

// Test 3: CalculateMean {2,4,6} = (2+4+6)/3 = 4.0
static void test3()
{
    Vector<float> v;
    v.Append(2.0f); v.Append(4.0f); v.Append(6.0f);
    report(3, "CalculateMean({2,4,6}) = 4.0",
           approx(CalculateMean(v), 4.0f));
}

// Test 4: CalculateSampleStdDev single element returns 0 (n-1 guard)
static void test4()
{
    Vector<float> v; v.Append(5.0f);
    report(4, "CalculateSampleStdDev({5.0}, mean=5.0) = 0.0  [n=1, n-1 guard]",
           approx(CalculateSampleStdDev(v, 5.0f), 0.0f));
}

// Test 5: CalculateSampleStdDev {2,4,6} mean=4
//   sum_sq_diffs = 4+0+4 = 8
//   sample variance = 8 / (3-1) = 8 / 2 = 4
//   stdev = sqrt(4) = 2.0   (NOT 2.828 which is sqrt(8) without the /2 denominator)
static void test5()
{
    Vector<float> v;
    v.Append(2.0f); v.Append(4.0f); v.Append(6.0f);
    float sd = CalculateSampleStdDev(v, 4.0f);
    report(5, "CalculateSampleStdDev({2,4,6}, mean=4) = 2.0  [8/(n-1)=8/2=4; sqrt(4)=2.0]",
           approx(sd, 2.0f));
}

// Test 6: CalculateMAD empty Vector returns 0
static void test6()
{
    Vector<float> v;
    report(6, "CalculateMAD(empty, mean=0) = 0.0",
           approx(CalculateMAD(v, 0.0f), 0.0f));
}

// Test 7: CalculateMAD {1,3,5} mean=3
//   MAD = (|1-3| + |3-3| + |5-3|) / 3 = (2+0+2)/3 = 4/3 = 1.333
static void test7()
{
    Vector<float> v;
    v.Append(1.0f); v.Append(3.0f); v.Append(5.0f);
    float mad = CalculateMAD(v, 3.0f);
    report(7, "CalculateMAD({1,3,5}, mean=3) = 1.333  [(2+0+2)/3]",
           approx(mad, 1.333f));
}

// Test 8: CalculateSPCC perfect positive correlation (x=y) = 1.0
//   x=y=1,2,3 -> num = 2, dX=dY=2, spcc=2/2=1.0
static void test8()
{
    Vector<float> x, y;
    x.Append(1.0f); x.Append(2.0f); x.Append(3.0f);
    y.Append(1.0f); y.Append(2.0f); y.Append(3.0f);
    report(8, "CalculateSPCC(x=y={1,2,3}) = 1.0  [perfect positive]",
           approx(CalculateSPCC(x, y), 1.0f));
}

// Test 9: CalculateSPCC perfect negative correlation = -1.0
//   x={1,2,3}, y={3,2,1}; mx=my=2
//   num = (-1)(1)+(0)(0)+(1)(-1) = -2
//   dX=2, dY=2, denom=sqrt(4)=2; spcc = -2/2 = -1.0
static void test9()
{
    Vector<float> x, y;
    x.Append(1.0f); x.Append(2.0f); x.Append(3.0f);
    y.Append(3.0f); y.Append(2.0f); y.Append(1.0f);
    report(9, "CalculateSPCC(x={1,2,3}, y={3,2,1}) = -1.0  [perfect negative]",
           approx(CalculateSPCC(x, y), -1.0f));
}

// Test 10: CalculateSPCC n < 2 returns 0 (guard condition)
static void test10()
{
    Vector<float> x, y;
    x.Append(5.0f); y.Append(5.0f);
    report(10, "CalculateSPCC n=1 returns 0.0  [n<2 guard]",
           approx(CalculateSPCC(x, y), 0.0f));
}

// Test 11: CalculateSPCC mismatched sizes returns 0 (guard condition)
static void test11()
{
    Vector<float> x, y;
    x.Append(1.0f); x.Append(2.0f);
    y.Append(1.0f);   // only 1 element vs x's 2
    report(11, "CalculateSPCC mismatched sizes (x=2, y=1) returns 0.0  [size guard]",
           approx(CalculateSPCC(x, y), 0.0f));
}

int main()
{
    std::cout << "=== StatsTest ===" << std::endl;
    std::cout << "Note: CalculateSampleStdDev uses sample formula (divide by n-1)" << std::endl;
    std::cout << std::endl;

    test1(); test2(); test3(); test4(); test5();
    test6(); test7(); test8(); test9(); test10(); test11();

    std::cout << std::endl
              << "Results: " << passed << " passed, "
              << failed << " failed." << std::endl;
    return failed == 0 ? 0 : 1;
}
