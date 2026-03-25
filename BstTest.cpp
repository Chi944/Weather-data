    /**
     * @file BstTest.cpp
     * @brief Comprehensive unit tests for Bst<T> — Test Plan tests 1-19
     *
     * Tests every public method in Bst.h:
     *   Default constructor, copy constructor, copy assignment,
     *   insert (new/duplicate), deleteNode (leaf/one-child/two-children/absent),
     *   search (found/not-found/empty), isEmpty (initial/after-deleted),
     *   size, inOrderTraversal (empty/ordered), preOrderTraversal,
     *   postOrderTraversal, and large ascending-order stress test.
     *
     * ITERATIVE vs RECURSIVE (Bst.h version 4.0)
     * --------------------------------------------------
     * insert()     — ITERATIVE (pointer loop, no call stack)
     * search()     — ITERATIVE (pointer loop, no call stack)
     * destroyTree  — ITERATIVE (heap-allocated pointer array stack)
     *
     * deleteNode, copyTree, all traversal helpers — still RECURSIVE.
     * These are only called on the small year BST (~15 nodes) so recursion
     * depth is never a concern.
     *
     * Test 19 specifically validates the iterative fix: it inserts 5,000
     * values in ascending order (producing a completely right-skewed tree)
     * and confirms that insert(), search(), and destroyTree() (triggered by
     * copy-assignment reset) all complete without a stack overflow.  The
     * old recursive versions would crash on ~10,000 values on Windows.
     *
     * Manual verification of all expected traversal orders:
     *
     *   Tree built by inserting 40, 20, 60, 10, 30:
     *
     *           40
     *          /  \
     *        20    60
     *       /  \
     *      10  30
     *
     *   In-order   (left, root, right): 10, 20, 30, 40, 60
     *   Pre-order  (root, left, right): 40, 20, 10, 30, 60
     *   Post-order (left, right, root): 10, 30, 20, 60, 40
     *
     *   deleteNode two-children (delete 20 from tree above):
     *     In-order successor = 30 (smallest in right subtree of 20).
     *     20 replaced with 30; 30 removed from leaf position.
     *     Remaining in-order: 10, 30, 40, 60
     *
     *   deleteNode one-child (delete 70 from {50,30,70,60}):
     *     70 has only left child 60; 60 slides up.
     *     Remaining in-order: 30, 50, 60
     *
     *   Copy constructor independence: copy {5,3,7}, insert 4 into copy only.
     *     Original in-order: 3, 5, 7
     *     Copy in-order:     3, 4, 5, 7
     *
     * @author Deston
     * @date 24/03/2026
     */

#include <iostream>
#include <string>
#include <sstream>
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

// Shared collector for traversal tests — cleared before each use
static Vector<int> g_col;
static void collect(int& v) { g_col.Append(v); }

// ---------------------------------------------------------------------------
// Test 1: Default constructor — empty state
// ---------------------------------------------------------------------------
static void test1()
{
    Bst<int> t;
    report(1, "Default constructor: isEmpty()=true, size()=0",
           t.isEmpty() && t.size() == 0);
}

// ---------------------------------------------------------------------------
// Test 2: insert() new value — returns true, size and search correct
// ---------------------------------------------------------------------------
static void test2()
{
    Bst<int> t;
    bool ok = t.insert(10);
    report(2, "insert(10): returns true; size=1; search(10)=true",
           ok && t.size() == 1 && t.search(10));
}

// ---------------------------------------------------------------------------
// Test 3: insert() duplicate — returns false, size unchanged
// ---------------------------------------------------------------------------
static void test3()
{
    Bst<int> t;
    t.insert(10);
    bool dup = t.insert(10);
    report(3, "insert duplicate: returns false; size stays 1",
           !dup && t.size() == 1);
}

// ---------------------------------------------------------------------------
// Test 4: search() false for value not in tree
// ---------------------------------------------------------------------------
static void test4()
{
    Bst<int> t;
    t.insert(5);
    report(4, "search(99) false when 99 not in tree",
           !t.search(99));
}

// ---------------------------------------------------------------------------
// Test 5: search() on empty tree returns false (edge case)
// ---------------------------------------------------------------------------
static void test5()
{
    Bst<int> t;
    report(5, "search() on empty tree returns false",
           !t.search(42));
}

// ---------------------------------------------------------------------------
// Test 6: inOrderTraversal — ascending order via function pointer
//   Insert 30, 10, 20 -> in-order: 10, 20, 30
// ---------------------------------------------------------------------------
static void test6()
{
    Bst<int> t;
    t.insert(30); t.insert(10); t.insert(20);

    g_col.Clear();
    t.inOrderTraversal(collect);

    bool ok = g_col.Size() == 3
           && g_col[0] == 10
           && g_col[1] == 20
           && g_col[2] == 30;
    report(6, "inOrderTraversal {30,10,20}: callback receives 10,20,30", ok);
}

// ---------------------------------------------------------------------------
// Test 7: inOrderTraversal on empty tree — callback never called
// ---------------------------------------------------------------------------
static void test7()
{
    Bst<int> t;
    g_col.Clear();
    t.inOrderTraversal(collect);
    report(7, "inOrderTraversal on empty tree: callback never invoked (size=0)",
           g_col.Size() == 0);
}

// ---------------------------------------------------------------------------
// Test 8: preOrderTraversal (root, left, right)
//   Insert 40,20,60,10,30 -> pre-order: 40,20,10,30,60
//   Manual: root 40 first; recurse left: 20, then 10, then 30;
//           recurse right: 60
// ---------------------------------------------------------------------------
static void test8()
{
    Bst<int> t;
    t.insert(40); t.insert(20); t.insert(60);
    t.insert(10); t.insert(30);

    g_col.Clear();
    t.preOrderTraversal(collect);

    bool ok = g_col.Size() == 5
           && g_col[0] == 40
           && g_col[1] == 20
           && g_col[2] == 10
           && g_col[3] == 30
           && g_col[4] == 60;
    report(8, "preOrderTraversal {40,20,60,10,30}: 40,20,10,30,60", ok);
}

// ---------------------------------------------------------------------------
// Test 9: postOrderTraversal (left, right, root)
//   Same tree as test 8 -> post-order: 10,30,20,60,40
//   Manual: leaves first: 10, 30; subtree root 20; right leaf 60; root 40
// ---------------------------------------------------------------------------
static void test9()
{
    Bst<int> t;
    t.insert(40); t.insert(20); t.insert(60);
    t.insert(10); t.insert(30);

    g_col.Clear();
    t.postOrderTraversal(collect);

    bool ok = g_col.Size() == 5
           && g_col[0] == 10
           && g_col[1] == 30
           && g_col[2] == 20
           && g_col[3] == 60
           && g_col[4] == 40;
    report(9, "postOrderTraversal {40,20,60,10,30}: 10,30,20,60,40", ok);
}

// ---------------------------------------------------------------------------
// Test 10: deleteNode leaf — no children, tree becomes empty
// ---------------------------------------------------------------------------
static void test10()
{
    Bst<int> t;
    t.insert(7);
    t.deleteNode(7);
    report(10, "deleteNode leaf {7}: search(7)=false; isEmpty()=true",
           !t.search(7) && t.isEmpty());
}

// ---------------------------------------------------------------------------
// Test 11: deleteNode one-child — node 70 has only left child 60
//   Tree {50,30,70,60}: delete 70; 60 slides up.
//   In-order after: 30, 50, 60
// ---------------------------------------------------------------------------
static void test11()
{
    Bst<int> t;
    t.insert(50); t.insert(30); t.insert(70); t.insert(60);
    t.deleteNode(70);

    g_col.Clear();
    t.inOrderTraversal(collect);

    bool ok = t.size() == 3
           && !t.search(70)
           && t.search(60)
           && g_col.Size() == 3
           && g_col[0] == 30
           && g_col[1] == 50
           && g_col[2] == 60;
    report(11, "deleteNode one-child (70 left-child=60): in-order 30,50,60", ok);
}

// ---------------------------------------------------------------------------
// Test 12: deleteNode two-children — in-order successor replacement
//   Tree {40,20,60,10,30}: delete 20 (children 10 and 30)
//   Successor = 30 (smallest in right subtree of 20)
//   20 replaced with 30; in-order after: 10,30,40,60
// ---------------------------------------------------------------------------
static void test12()
{
    Bst<int> t;
    t.insert(40); t.insert(20); t.insert(60);
    t.insert(10); t.insert(30);
    t.deleteNode(20);

    g_col.Clear();
    t.inOrderTraversal(collect);

    bool ok = t.size() == 4
           && !t.search(20)
           && t.search(30)
           && g_col.Size() == 4
           && g_col[0] == 10
           && g_col[1] == 30
           && g_col[2] == 40
           && g_col[3] == 60;
    report(12, "deleteNode two-children (20, successor=30): in-order 10,30,40,60", ok);
}

// ---------------------------------------------------------------------------
// Test 13: deleteNode absent value — tree and size unchanged
// ---------------------------------------------------------------------------
static void test13()
{
    Bst<int> t;
    t.insert(5); t.insert(3); t.insert(7);
    int sizeBefore = t.size();

    std::ostringstream sink;
    std::streambuf* old = std::cout.rdbuf(sink.rdbuf());
    t.deleteNode(999);
    std::cout.rdbuf(old);

    report(13, "deleteNode absent value: size unchanged; search(5)=true",
           t.size() == sizeBefore && t.search(5));
}

// ---------------------------------------------------------------------------
// Test 14: isEmpty() returns true after deleting the only node
// ---------------------------------------------------------------------------
static void test14()
{
    Bst<int> t;
    t.insert(42);
    t.deleteNode(42);
    report(14, "isEmpty()=true after deleting the only node",
           t.isEmpty() && t.size() == 0);
}

// ---------------------------------------------------------------------------
// Test 15: size() correct after five inserts
// ---------------------------------------------------------------------------
static void test15()
{
    Bst<int> t;
    t.insert(1); t.insert(2); t.insert(3);
    t.insert(4); t.insert(5);
    report(15, "size()=5 after five distinct inserts", t.size() == 5);
}

// ---------------------------------------------------------------------------
// Test 16: Right-skewed ascending insert — iterative insert keeps correct order
//   Insert 10,20,30,40,50 in ascending order.
//   Each value goes to the right child -> completely right-skewed tree.
//   insert() is ITERATIVE so this does not use any call-stack frames.
//   inOrderTraversal must still deliver values in ascending order.
// ---------------------------------------------------------------------------
static void test16()
{
    Bst<int> t;
    t.insert(10); t.insert(20); t.insert(30);
    t.insert(40); t.insert(50);

    g_col.Clear();
    t.inOrderTraversal(collect);

    bool ok = g_col.Size() == 5;
    for (int i = 0; i < g_col.Size() && ok; i++)
        ok = (g_col[i] == (i + 1) * 10);

    report(16, "Right-skewed ascending insert (iterative): in-order 10,20,30,40,50", ok);
}

// ---------------------------------------------------------------------------
// Test 17: Copy constructor — deep copy independence
//   Build {5,3,7}, copy it, insert 4 into copy only.
//   Original in-order: 3, 5, 7 (unchanged)
//   Copy in-order:     3, 4, 5, 7
// ---------------------------------------------------------------------------
static void test17()
{
    Bst<int> original;
    original.insert(5); original.insert(3); original.insert(7);

    Bst<int> copy(original);
    copy.insert(4);

    g_col.Clear();
    original.inOrderTraversal(collect);
    bool origOk = g_col.Size() == 3
               && g_col[0] == 3
               && g_col[1] == 5
               && g_col[2] == 7;

    g_col.Clear();
    copy.inOrderTraversal(collect);
    bool copyOk = g_col.Size() == 4
               && g_col[0] == 3
               && g_col[1] == 4
               && g_col[2] == 5
               && g_col[3] == 7;

    report(17, "Copy constructor: original {3,5,7} unchanged; copy {3,4,5,7}",
           origOk && copyOk);
}

// ---------------------------------------------------------------------------
// Test 18: Copy assignment — original modified after assignment
//   Build {100,50,150}, assign to t2, then insert 200 into original only.
//   t2 in-order: 50,100,150 (unchanged); t2.search(200)=false
// ---------------------------------------------------------------------------
static void test18()
{
    Bst<int> original;
    original.insert(100); original.insert(50); original.insert(150);

    Bst<int> t2;
    t2 = original;
    original.insert(200);

    g_col.Clear();
    t2.inOrderTraversal(collect);

    bool ok = g_col.Size() == 3
           && g_col[0] == 50
           && g_col[1] == 100
           && g_col[2] == 150
           && !t2.search(200);
    report(18, "Copy assignment: t2 unaffected by later insert into original", ok);
}

// ---------------------------------------------------------------------------
// Test 19: Large ascending insert stress test — validates iterative fix
//
//   This test inserts 5,000 values in ascending order (producing a
//   right-skewed tree of depth 5,000), then:
//     - checks size() == 5000
//     - verifies search() finds the first and last inserted values
//     - verifies search() correctly returns false for an absent value
//     - resets the tree via copy assignment (triggers iterative destroyTree)
//     - verifies the tree is empty afterwards
//
//   With recursive methods, this test would crash or produce a stack
//   overflow error.  With iterative insert/search/destroyTree it passes.
// ---------------------------------------------------------------------------
static void test19()
{
    const int N = 5000;
    Bst<int> t;

    // Insert 1, 2, 3, ..., N in ascending order -> right-skewed tree
    for (int i = 1; i <= N; i++)
        t.insert(i);

    bool sizeOk   = (t.size() == N);
    bool searchLo = t.search(1);          // first inserted value
    bool searchHi = t.search(N);          // last inserted value
    bool searchNo = !t.search(N + 1);     // absent value

    // Reset via copy assignment — triggers iterative destroyTree on N-node tree
    t = Bst<int>();
    bool emptyAfter = t.isEmpty();

    report(19,
        "Large ascending stress test: 5000 ascending inserts, "
        "search first/last/absent, destroyTree via copy-assign — "
        "all iterative, no stack overflow",
        sizeOk && searchLo && searchHi && searchNo && emptyAfter);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main()
{
    std::cout << "=== BstTest (comprehensive — 19 tests) ===" << std::endl;
    std::cout << "    insert/search/destroyTree: ITERATIVE (safe for large trees)" << std::endl;
    std::cout << "    deleteNode/traversals:     RECURSIVE (small year BST only)" << std::endl;
    std::cout << std::endl;

    test1();  test2();  test3();  test4();  test5();
    test6();  test7();  test8();  test9();  test10();
    test11(); test12(); test13(); test14(); test15();
    test16(); test17(); test18(); test19();

    std::cout << std::endl
              << "Results: " << passed << " passed, "
              << failed    << " failed." << std::endl;
    return failed == 0 ? 0 : 1;
}
