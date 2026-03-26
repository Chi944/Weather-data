    /**
     * @file MapTest.cpp
     * @brief Unit tests for Map<K,V> — Test Plan tests 1-10
     *
     * Tests cover all four public methods of Map (version 2.0):
     *   operator[]  — insert-or-access
     *   getPtr()    — safe read; nullptr for absent key
     *   size()      — entry count; size()==0 replaces the old isEmpty()
     *   getKey(i)   — key at index; combined with getPtr replaces old getValue(i)
     *
     * Previously tested methods that have been removed in version 2.0:
     *   contains()   — replaced by getPtr(key) != nullptr
     *   isEmpty()    — replaced by size() == 0
     *   remove()     — never called in the application; removed from interface
     *   getValue(i)  — replaced by getPtr(getKey(i))
     *
     * @author Deston
     * @date 25/03/2026
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

// ---------------------------------------------------------------------------
// Test 1: Empty Map — size()==0 and getPtr returns nullptr
//   size()==0 replaces the old isEmpty()==true check.
//   getPtr(1)==nullptr replaces the old contains(1)==false check.
// ---------------------------------------------------------------------------
static void test1()
{
    Map<int,int> m;
    bool ok = m.size() == 0
           && m.getPtr(1) == nullptr;
    report(1, "Empty Map: size()==0; getPtr(1)==nullptr (replaces isEmpty+contains)", ok);
}

// ---------------------------------------------------------------------------
// Test 2: operator[] inserts new key — value accessible via getPtr and []
// ---------------------------------------------------------------------------
static void test2()
{
    Map<int,int> m;
    m[5] = 99;
    int* ptr = m.getPtr(5);
    bool ok = ptr != nullptr
           && *ptr == 99
           && m.size() == 1
           && m[5] == 99;
    report(2, "operator[] inserts key 5; getPtr(5) non-null; *ptr=99; size=1", ok);
}

// ---------------------------------------------------------------------------
// Test 3: operator[] on existing key updates value — no duplicate created
// ---------------------------------------------------------------------------
static void test3()
{
    Map<int,int> m;
    m[5] = 99;
    m[5] = 42;   // update existing key
    bool ok = m.size() == 1
           && m[5] == 42;
    report(3, "operator[] on existing key: no duplicate; size=1; value updated to 42", ok);
}

// ---------------------------------------------------------------------------
// Test 4: getPtr() nullptr for absent key (replaces old contains() == false)
//   Before: !m.contains(99)
//   After:   m.getPtr(99) == nullptr
// ---------------------------------------------------------------------------
static void test4()
{
    Map<int,int> m;
    m[1] = 10;
    bool ok = m.getPtr(99) == nullptr;
    report(4, "getPtr(99)==nullptr for absent key (replaces contains()==false)", ok);
}

// ---------------------------------------------------------------------------
// Test 5: getPtr() non-null and correct value for existing key
// ---------------------------------------------------------------------------
static void test5()
{
    Map<int,int> m;
    m[3] = 10;
    int* ptr = m.getPtr(3);
    bool ok = ptr != nullptr && *ptr == 10;
    report(5, "getPtr(3) non-null; *ptr=10", ok);
}

// ---------------------------------------------------------------------------
// Test 6: const getPtr() nullptr for absent key on const Map
// ---------------------------------------------------------------------------
static void test6()
{
    const Map<int,int> m;   // const — no entries
    const int* ptr = m.getPtr(99);
    report(6, "const getPtr(99)==nullptr on empty const Map", ptr == nullptr);
}

// ---------------------------------------------------------------------------
// Test 7: size() grows correctly with each unique insert
//   size() == 0 replaces the old isEmpty() check.
//   Three distinct keys: size must be 3.
// ---------------------------------------------------------------------------
static void test7()
{
    Map<int,int> m;
    bool s0 = m.size() == 0;   // isEmpty() replacement
    m[10] = 1;
    bool s1 = m.size() == 1;
    m[20] = 2;
    bool s2 = m.size() == 2;
    m[30] = 3;
    bool s3 = m.size() == 3;
    report(7, "size() grows: 0 -> 1 -> 2 -> 3 (size()==0 replaces isEmpty())", s0 && s1 && s2 && s3);
}

// ---------------------------------------------------------------------------
// Test 8: getKey(i) returns keys in insertion order
//   Combined with size() this allows complete iteration without getValue().
// ---------------------------------------------------------------------------
static void test8()
{
    Map<int,int> m;
    m[2] = 20;
    m[7] = 70;
    m[4] = 40;
    bool ok = m.size() == 3
           && m.getKey(0) == 2
           && m.getKey(1) == 7
           && m.getKey(2) == 4;
    report(8, "getKey(i) returns keys in insertion order: 2,7,4", ok);
}

// ---------------------------------------------------------------------------
// Test 9: getPtr(getKey(i)) replaces old getValue(i)
//   Before: m.getValue(i)
//   After:  *(m.getPtr(m.getKey(i)))
//   Iterates all entries and verifies each value via getPtr.
// ---------------------------------------------------------------------------
static void test9()
{
    Map<int,int> m;
    m[2] = 20;
    m[7] = 70;

    bool ok = true;
    for (int i = 0; i < m.size(); i++)
    {
        int key        = m.getKey(i);
        int* valPtr    = m.getPtr(key);  // replaces getValue(i)
        if (valPtr == nullptr) { ok = false; break; }

        // Verify: key 2 -> 20, key 7 -> 70
        if (key == 2 && *valPtr != 20) ok = false;
        if (key == 7 && *valPtr != 70) ok = false;
    }
    report(9, "getPtr(getKey(i)) replaces getValue(i): key2=20, key7=70", ok);
}

// ---------------------------------------------------------------------------
// Test 10: Nested Map — only the 4 methods used
//   outer[5][3] = 99
//   Verify via getPtr() (replaces old inner->contains(3)).
//   Before: inner->contains(3)
//   After:  inner->getPtr(3) != nullptr
// ---------------------------------------------------------------------------
static void test10()
{
    Map<int, Map<int,int> > outer;
    outer[5][3] = 99;

    const Map<int,int>* inner = outer.getPtr(5);

    bool ok = false;
    if (inner != nullptr)
    {
        // getPtr(3)!=nullptr replaces the old inner->contains(3)
        const int* valPtr = inner->getPtr(3);
        ok = (valPtr != nullptr) && (*valPtr == 99);
    }
    report(10, "Nested Map[5][3]=99: inner getPtr(3)!=nullptr; value=99 (no contains used)", ok);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main()
{
    std::cout << "=== MapTest (4-method interface) ===" << std::endl;
    std::cout << "    Public methods: operator[], getPtr(), size(), getKey()" << std::endl;
    std::cout << "    Removed: contains(), isEmpty(), remove(), getValue()" << std::endl;
    std::cout << std::endl;

    test1(); test2(); test3(); test4(); test5();
    test6(); test7(); test8(); test9(); test10();

    std::cout << std::endl
              << "Results: " << passed << " passed, "
              << failed    << " failed." << std::endl;
    return failed == 0 ? 0 : 1;
}
