    /**
     * @file BstTest.cpp
     * @brief Comprehensive unit tests for Bst<T> — Test Plan tests 1-18
     *
     * Covers every public method declared in Bst.h:
     *   Default constructor, copy constructor, copy assignment,
     *   insert (new/duplicate), deleteNode (leaf/one-child/two-children/absent),
     *   search (found/not-found/empty), isEmpty (initial/after-all-deleted),
     *   size, inOrderTraversal (empty/ordered/callback-collected),
     *   preOrderTraversal, postOrderTraversal.
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
     *   deleteNode two-children case (delete 20):
     *     In-order successor of 20 = 30 (smallest in right subtree).
     *     20 is replaced with 30; 30 is removed from its original position.
     *     Remaining in-order: 10, 30, 40, 60
     *
     *   deleteNode one-child case (delete 70 from {50,30,70,60}):
     *     Tree:  50          ->  delete 70 (has only left child 60)
     *           /  \
     *          30   70
     *                /
     *               60
     *     60 slides up; remaining in-order: 30, 50, 60
     *
     *   Copy constructor independence: copy {5,3,7} -> insert 4 into copy only.
     *     Original in-order: 3, 5, 7   (unchanged)
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

// ---------------------------------------------------------------------------
// Shared collector — cleared before each traversal test
// ---------------------------------------------------------------------------
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
// Test 2: insert() new value returns true; size and search correct
// ---------------------------------------------------------------------------
static void test2()
{
    Bst<int> t;
    bool ok = t.insert(10);
    report(2, "insert(10): returns true; size=1; search(10)=true",
           ok && t.size() == 1 && t.search(10));
}

// ---------------------------------------------------------------------------
// Test 3: insert() duplicate returns false; size unchanged
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
// Test 4: search() returns false for value not in tree
// ---------------------------------------------------------------------------
static void test4()
{
    Bst<int> t;
    t.insert(5);
    report(4, "search(99) false when 99 not inserted",
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
// Test 6: inOrderTraversal delivers ascending order via function pointer
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
    report(6, "inOrderTraversal {30,10,20}: callback receives 10,20,30",
           ok);
}

// ---------------------------------------------------------------------------
// Test 7: inOrderTraversal on empty tree — callback never called (size=0)
// ---------------------------------------------------------------------------
static void test7()
{
    Bst<int> t;
    g_col.Clear();
    t.inOrderTraversal(collect);
    report(7, "inOrderTraversal on empty tree: callback never called (0 elements)",
           g_col.Size() == 0);
}

// ---------------------------------------------------------------------------
// Test 8: preOrderTraversal — root, left, right
//   Insert 40,20,60,10,30 -> pre-order: 40,20,10,30,60
//   Manual:
//     Visit 40 (root), recurse left subtree: visit 20 (root), recurse left:
//     visit 10 (leaf), back, recurse right: visit 30 (leaf), back up,
//     recurse right subtree: visit 60 (leaf)
// ---------------------------------------------------------------------------
static void test8()
{
    Bst<int> t;
    t.insert(40); t.insert(20); t.insert(60);
    t.insert(10); t.insert(30);

    g_col.Clear();
    t.preOrderTraversal(collect);

    bool ok = g_col.Size() == 5
           && g_col[0] == 40   // root first
           && g_col[1] == 20   // left subtree root
           && g_col[2] == 10   // left-left leaf
           && g_col[3] == 30   // left-right leaf
           && g_col[4] == 60;  // right subtree leaf
    report(8, "preOrderTraversal {40,20,60,10,30}: 40,20,10,30,60",
           ok);
}

// ---------------------------------------------------------------------------
// Test 9: postOrderTraversal — left, right, root
//   Same tree as test 8 -> post-order: 10,30,20,60,40
//   Manual:
//     Recurse left of 40 -> left of 20 -> visit 10 (leaf),
//     right of 20 -> visit 30 (leaf), visit 20,
//     recurse right of 40 -> visit 60 (leaf), visit 40 (root last)
// ---------------------------------------------------------------------------
static void test9()
{
    Bst<int> t;
    t.insert(40); t.insert(20); t.insert(60);
    t.insert(10); t.insert(30);

    g_col.Clear();
    t.postOrderTraversal(collect);

    bool ok = g_col.Size() == 5
           && g_col[0] == 10   // left-left leaf first
           && g_col[1] == 30   // left-right leaf
           && g_col[2] == 20   // left subtree root
           && g_col[3] == 60   // right leaf
           && g_col[4] == 40;  // root last
    report(9, "postOrderTraversal {40,20,60,10,30}: 10,30,20,60,40",
           ok);
}

// ---------------------------------------------------------------------------
// Test 10: deleteNode leaf — removes node with no children
//   Tree {7}, delete 7 -> empty
// ---------------------------------------------------------------------------
static void test10()
{
    Bst<int> t;
    t.insert(7);
    t.deleteNode(7);
    report(10, "deleteNode leaf: search(7)=false; isEmpty()=true",
           !t.search(7) && t.isEmpty());
}

// ---------------------------------------------------------------------------
// Test 11: deleteNode one-child — node with only a left child
//   Insert 50, 30, 70, 60 -> delete 70 (has only left child 60)
//   Tree before:         Tree after:
//        50                  50
//       /  \                /  \
//      30   70             30   60
//            /
//           60
//   In-order before: 30,50,60,70
//   In-order after:  30,50,60
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
    report(11, "deleteNode one-child (70 has left child 60): in-order 30,50,60",
           ok);
}

// ---------------------------------------------------------------------------
// Test 12: deleteNode two-children — in-order successor replacement
//   Insert 40,20,60,10,30 -> delete 20 (has two children: 10 and 30)
//   In-order successor of 20 = 30 (smallest in right subtree of 20).
//   20 is replaced with 30; 30 removed from its original leaf position.
//   In-order after delete: 10, 30, 40, 60
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
           && t.search(30)   // 30 promoted to where 20 was
           && t.search(10)
           && g_col.Size() == 4
           && g_col[0] == 10
           && g_col[1] == 30
           && g_col[2] == 40
           && g_col[3] == 60;
    report(12, "deleteNode two-children (20, successor=30): in-order 10,30,40,60",
           ok);
}

// ---------------------------------------------------------------------------
// Test 13: deleteNode absent value — tree unchanged, size unchanged
// ---------------------------------------------------------------------------
static void test13()
{
    Bst<int> t;
    t.insert(5); t.insert(3); t.insert(7);
    int sizeBefore = t.size();

    // Suppress the "not found" diagnostic cout output
    std::ostringstream sink;
    std::streambuf* old = std::cout.rdbuf(sink.rdbuf());
    t.deleteNode(999);
    std::cout.rdbuf(old);

    report(13, "deleteNode absent value: size unchanged; search(5)=true",
           t.size() == sizeBefore && t.search(5));
}

// ---------------------------------------------------------------------------
// Test 14: isEmpty() becomes true after deleting the only node
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
// Test 15: size() with multiple distinct values
// ---------------------------------------------------------------------------
static void test15()
{
    Bst<int> t;
    t.insert(1); t.insert(2); t.insert(3);
    t.insert(4); t.insert(5);
    report(15, "size() = 5 after inserting five distinct values",
           t.size() == 5);
}

// ---------------------------------------------------------------------------
// Test 16: Right-skewed insert (ascending order) — BST ordering still correct
//   Inserting in ascending order produces a right-skewed tree.
//   inOrderTraversal must still deliver values in ascending order.
// ---------------------------------------------------------------------------
static void test16()
{
    Bst<int> t;
    // Insert in ascending order -> each new value goes right
    t.insert(10); t.insert(20); t.insert(30);
    t.insert(40); t.insert(50);

    g_col.Clear();
    t.inOrderTraversal(collect);

    bool ok = g_col.Size() == 5;
    for (int i = 0; i < g_col.Size() && ok; i++)
        ok = (g_col[i] == (i + 1) * 10);

    report(16, "Right-skewed insert (ascending): in-order still 10,20,30,40,50",
           ok);
}

// ---------------------------------------------------------------------------
// Test 17: Copy constructor — deep copy; modifying copy leaves original intact
//   Build {5,3,7}, copy it, insert 4 into the copy only.
//   Original in-order: 3, 5, 7
//   Copy in-order:     3, 4, 5, 7
// ---------------------------------------------------------------------------
static void test17()
{
    Bst<int> original;
    original.insert(5); original.insert(3); original.insert(7);

    Bst<int> copy(original);   // copy constructor
    copy.insert(4);             // modify copy only

    // Original must be unchanged
    g_col.Clear();
    original.inOrderTraversal(collect);
    bool origOk = g_col.Size() == 3
               && g_col[0] == 3
               && g_col[1] == 5
               && g_col[2] == 7;

    // Copy must contain 4 in addition
    g_col.Clear();
    copy.inOrderTraversal(collect);
    bool copyOk = g_col.Size() == 4
               && g_col[0] == 3
               && g_col[1] == 4
               && g_col[2] == 5
               && g_col[3] == 7;

    report(17, "Copy constructor: original {3,5,7} unchanged; copy {3,4,5,7} after insert",
           origOk && copyOk);
}

// ---------------------------------------------------------------------------
// Test 18: Copy assignment — deep copy; modifying original after assign
//   does not corrupt the assigned copy.
//   Build {100,50,150}, assign to t2, then insert 200 into original only.
//   t2 in-order must still be: 50, 100, 150
// ---------------------------------------------------------------------------
static void test18()
{
    Bst<int> original;
    original.insert(100); original.insert(50); original.insert(150);

    Bst<int> t2;
    t2 = original;              // copy assignment
    original.insert(200);       // modify original only

    // t2 must be unchanged
    g_col.Clear();
    t2.inOrderTraversal(collect);

    bool ok = g_col.Size() == 3
           && g_col[0] == 50
           && g_col[1] == 100
           && g_col[2] == 150
           && !t2.search(200);  // 200 was NOT inserted into t2

    report(18, "Copy assignment: t2 unaffected by later insert into original",
           ok);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main()
{
    std::cout << "=== BstTest (comprehensive) ===" << std::endl;
    std::cout << std::endl;

    test1();  test2();  test3();  test4();  test5();
    test6();  test7();  test8();  test9();  test10();
    test11(); test12(); test13(); test14(); test15();
    test16(); test17(); test18();

    std::cout << std::endl
              << "Results: " << passed << " passed, "
              << failed    << " failed." << std::endl;
    return failed == 0 ? 0 : 1;
}
