    /**
     * @file BstTest.cpp
     * @brief Unit tests for Bst<T> — Test Plan tests 1-7
     *
     * Tests cover: empty state, insert (new and duplicate), search,
     * inOrderTraversal via function pointer (ascending order),
     * deleteNode, and size().
     *
     * @author Deston
     * @date 24/03/2026
     */

#include <iostream>
#include <string>
#include "Bst.h"
#include "Vector.h"

static int passed = 0;
static int failed = 0;

static void report(int testNum, const std::string& desc, bool ok)
{
    std::cout << "[Test " << testNum << "] " << (ok ? "PASS" : "FAIL")
              << " -- " << desc << std::endl;
    if (ok) passed++; else failed++;
}

// Collector used as function pointer for inOrderTraversal
static Vector<int> g_collected;
static void collectYear(int& val)
{
    g_collected.Append(val);
}

// Test 1: empty BST
static void test1()
{
    Bst<int> t;
    report(1, "Empty BST: isEmpty()=true, size()=0", t.isEmpty() && t.size() == 0);
}

// Test 2: insert new value
static void test2()
{
    Bst<int> t;
    bool ins = t.insert(10);
    report(2, "insert(10) returns true; size=1; search(10)=true",
           ins && t.size() == 1 && t.search(10));
}

// Test 3: insert duplicate returns false, size unchanged
static void test3()
{
    Bst<int> t;
    t.insert(10);
    bool dup = t.insert(10);
    report(3, "insert duplicate returns false; size stays 1", !dup && t.size() == 1);
}

// Test 4: search() false for absent value
static void test4()
{
    Bst<int> t;
    t.insert(5);
    report(4, "search(99) returns false when 99 not in tree", !t.search(99));
}

// Test 5: inOrderTraversal delivers ascending order via function pointer
static void test5()
{
    Bst<int> t;
    t.insert(30);
    t.insert(10);
    t.insert(20);

    g_collected.Clear();
    t.inOrderTraversal(collectYear);   // function pointer passed here

    bool ok = g_collected.Size() == 3
           && g_collected[0] == 10
           && g_collected[1] == 20
           && g_collected[2] == 30;
    report(5, "inOrderTraversal via function pointer: 10,20,30 in order", ok);
}

// Test 6: deleteNode removes existing node
static void test6()
{
    Bst<int> t;
    t.insert(7);
    t.deleteNode(7);
    report(6, "deleteNode(7): search(7)=false; size=0", !t.search(7) && t.size() == 0);
}

// Test 7: size() after multiple inserts
static void test7()
{
    Bst<int> t;
    t.insert(1); t.insert(2); t.insert(3);
    report(7, "size() = 3 after three distinct inserts", t.size() == 3);
}

int main()
{
    std::cout << "=== BstTest ===" << std::endl;
    test1(); test2(); test3(); test4(); test5(); test6(); test7();
    std::cout << std::endl << "Results: " << passed << " passed, "
              << failed << " failed." << std::endl;
    return failed == 0 ? 0 : 1;
}
