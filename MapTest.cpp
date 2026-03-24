    /**
     * @file MapTest.cpp
     * @brief Unit tests for Map<K,V> — Test Plan tests 1-10
     *
     * Tests cover: default state, operator[] insert/access, no-duplicate invariant,
     * contains(), getPtr() (null and non-null), remove(), index iteration,
     * and nested Map usage.
     *
     * @author Deston
     * @date 24/03/2026
     */

#include <iostream>
#include <string>
#include "Map.h"

static int passed = 0;
static int failed = 0;

static void report(int testNum, const std::string& desc, bool ok)
{
    std::cout << "[Test " << testNum << "] " << (ok ? "PASS" : "FAIL")
              << " -- " << desc << std::endl;
    if (ok) passed++; else failed++;
}

// Test 1: empty Map state
static void test1()
{
    Map<int,int> m;
    bool ok = m.isEmpty() && m.size() == 0 && !m.contains(1);
    report(1, "Empty Map: isEmpty()=true, size()=0, contains(1)=false", ok);
}

// Test 2: operator[] inserts new key
static void test2()
{
    Map<int,int> m;
    m[5] = 99;
    bool ok = m.contains(5) && m.size() == 1 && m[5] == 99;
    report(2, "operator[] inserts new key; contains=true, size=1, value=99", ok);
}

// Test 3: operator[] on existing key does not duplicate
static void test3()
{
    Map<int,int> m;
    m[5] = 99;
    m[5] = 42;
    bool ok = m.size() == 1 && m[5] == 42;
    report(3, "operator[] on existing key: no duplicate, value updated to 42", ok);
}

// Test 4: contains() false for non-existent key
static void test4()
{
    Map<int,int> m;
    m[1] = 10;
    bool ok = !m.contains(99);
    report(4, "contains() returns false for absent key", ok);
}

// Test 5: getPtr() non-null for existing key
static void test5()
{
    Map<int,int> m;
    m[3] = 10;
    int* ptr = m.getPtr(3);
    bool ok = (ptr != nullptr) && (*ptr == 10);
    report(5, "getPtr() returns non-null pointer to value 10", ok);
}

// Test 6: getPtr() nullptr for absent key (const version)
static void test6()
{
    const Map<int,int> m;   // const Map, no entries
    const int* ptr = m.getPtr(99);
    report(6, "const getPtr() returns nullptr for absent key", ptr == nullptr);
}

// Test 7: remove() existing key
static void test7()
{
    Map<int,int> m;
    m[4] = 7;
    bool ok = m.remove(4);
    bool ok2 = !m.contains(4) && m.size() == 0;
    report(7, "remove() returns true; size=0; contains=false after removal", ok && ok2);
}

// Test 8: remove() absent key returns false
static void test8()
{
    Map<int,int> m;
    bool ok = !m.remove(55);
    report(8, "remove() absent key returns false and size stays 0", ok && m.size() == 0);
}

// Test 9: index-based iteration via getKey/getValue
static void test9()
{
    Map<int,int> m;
    m[2] = 20;
    m[7] = 70;
    bool ok = (m.size() == 2)
           && (m.getKey(0) == 2)   && (m.getValue(0) == 20)
           && (m.getKey(1) == 7)   && (m.getValue(1) == 70);
    report(9, "getKey(i)/getValue(i): first entry key=2,val=20; second key=7,val=70", ok);
}

// Test 10: nested Map<int, Map<int,int>>
static void test10()
{
    Map<int, Map<int,int> > outer;
    outer[5][3] = 99;
    const Map<int,int>* inner = outer.getPtr(5);
    bool ok = (inner != nullptr) && inner->contains(3);
    int val = 0;
    if (inner) { const int* p = inner->getPtr(3); if (p) val = *p; }
    report(10, "Nested Map[5][3]=99: inner non-null, value=99", ok && val == 99);
}

int main()
{
    std::cout << "=== MapTest ===" << std::endl;
    test1(); test2(); test3(); test4(); test5();
    test6(); test7(); test8(); test9(); test10();
    std::cout << std::endl << "Results: " << passed << " passed, "
              << failed << " failed." << std::endl;
    return failed == 0 ? 0 : 1;
}
