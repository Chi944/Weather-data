#ifndef BST_H
#define BST_H

#include <iostream>

    /**
     * @file Bst.h
     * @brief Generic Binary Search Tree with function-pointer traversal callbacks.
     *
     * @details
     * Stores any type T that supports operator< and operator==.
     * Duplicate values are silently rejected.
     *
     * Traversal methods accept a function pointer (f1Typ) so the caller
     * controls what happens at each visited node, enabling decoupled,
     * reusable callback behaviour.
     *
     *
     *   Making insert(), search(), and destroyTree() iterative eliminates the
     *   call-stack dependency entirely. The three traversal methods (inOrder,
     *   preOrder, postOrder) and deleteNode are only called on the 15-node year
     *   BST, so they remain recursive without any risk.
     *
     * @tparam T  Element type.  Must support operator< and operator==.
     *
     * @author Deston
     * @version 4.0
     * @date 24/03/2026
     */


/**
 * @brief Internal node for Bst<T>.
 *
 * @tparam T  Same element type as the containing Bst<T>.
 */
template<typename T>
struct BstNode
{
    T          data;   ///< Value stored at this node.
    BstNode*   left;   ///< Left  child; nullptr if absent.
    BstNode*   right;  ///< Right child; nullptr if absent.
};


// =========================================================================
// Bst — Generic Binary Search Tree
// =========================================================================

template<typename T>
class Bst
{
public:

    /**
     * @brief Function pointer type for traversal callbacks.
     *
     * Signature: void callback(T& data)
     * The parameter is T& (non-const) so the callback may read or print
     * the value.  The caller must NOT modify data in a way that breaks
     * BST ordering; enforcing this is the caller's responsibility.
     */
    typedef void (*f1Typ)(T&);

    /** @brief Default constructor.  Produces an empty tree. */
    Bst();

    /** @brief Destructor.  Frees every node iteratively (safe for large trees). */
    ~Bst();

    /**
     * @brief Copy constructor.  Deep-copies other.
     * @param other  Source tree.
     */
    Bst(const Bst& other);

    /**
     * @brief Copy-assignment operator.  Self-assignment safe.
     * @param other  Source tree.
     * @return       Reference to *this.
     */
    Bst& operator=(const Bst& other);

    /**
     * @brief Insert value into the tree (ITERATIVE).
     *
     * Walks the tree with a pointer loop rather than recursion.
     * Safe for right-skewed trees of 500,000+ nodes.
     *
     * @param  value  Element to insert.
     * @return true   if inserted; false if value already existed (duplicate).
     */
    bool insert(const T& value);

    /**
     * @brief Remove the node containing value (RECURSIVE).
     *
     * Only called on the small year BST (~15 nodes). Recursion depth
     * is never a concern there.
     *
     * @param value  Element to remove.
     */
    void deleteNode(const T& value);

    /**
     * @brief Search for value (ITERATIVE).
     *
     * Walks the tree with a pointer loop. Safe for right-skewed
     * trees of 500,000+ nodes.
     *
     * @param  value  Element to locate.
     * @return true if found, false otherwise.
     */
    bool search(const T& value) const;

    /** @brief Returns true when the tree contains no nodes. */
    bool isEmpty() const;

    /**
     * @brief Returns the number of nodes in the tree.
     * @note O(n) recursive count.
     */
    int size() const;

    /**
     * @brief In-order traversal (left, root, right); invokes f1 at each node.
     * @param f1  Callback for each node's data.
     */
    void inOrderTraversal(f1Typ f1) const;

    /**
     * @brief Pre-order traversal (root, left, right); invokes f1 at each node.
     * @param f1  Callback for each node's data.
     */
    void preOrderTraversal(f1Typ f1) const;

    /**
     * @brief Post-order traversal (left, right, root); invokes f1 at each node.
     * @param f1  Callback for each node's data.
     */
    void postOrderTraversal(f1Typ f1) const;

private:

    BstNode<T>* root;  ///< Root node; nullptr when empty.

    /**
     * @brief Free every node iteratively using an explicit stack array.
     *
     * Allocates a fixed-size pointer array on the heap to simulate a
     * stack.  The array capacity starts at 64 and doubles on overflow,
     * so even a 500,000-node right-skewed tree is freed without touching
     * the call stack.
     *
     * @param node  Root of the subtree to destroy.
     */
    void        destroyTree    (BstNode<T>* node);

    BstNode<T>* copyTree       (BstNode<T>* node) const;
    BstNode<T>* deleteHelper   (BstNode<T>* node, const T& value, bool& removed);
    void        inOrderHelper  (f1Typ f1, BstNode<T>* node) const;
    void        preOrderHelper (f1Typ f1, BstNode<T>* node) const;
    void        postOrderHelper(f1Typ f1, BstNode<T>* node) const;
    int         sizeHelper     (BstNode<T>* node) const;
};


// =========================================================================
// IMPLEMENTATION
// =========================================================================

// ---- Constructor / Destructor / Assignment ----

template<typename T>
Bst<T>::Bst() : root(nullptr)
{
}

template<typename T>
Bst<T>::~Bst()
{
    destroyTree(root);
    root = nullptr;
}

template<typename T>
Bst<T>::Bst(const Bst& other) : root(nullptr)
{
    root = copyTree(other.root);
}

template<typename T>
Bst<T>& Bst<T>::operator=(const Bst& other)
{
    if (this != &other)
    {
        destroyTree(root);
        root = copyTree(other.root);
    }
    return *this;
}

// ---- Public interface ----

/**
 * Iterative insert.
 * Walks left/right with a plain pointer loop until it finds an empty slot.
 * No recursive frames — depth-independent.
 */
template<typename T>
bool Bst<T>::insert(const T& value)
{
    // Allocate new node before walking so we only allocate once
    BstNode<T>* newNode = new BstNode<T>;
    newNode->data  = value;
    newNode->left  = nullptr;
    newNode->right = nullptr;

    if (root == nullptr)
    {
        root = newNode;
        return true;
    }

    BstNode<T>* curr = root;
    while (true)
    {
        if (value < curr->data)
        {
            if (curr->left == nullptr)
            {
                curr->left = newNode;
                return true;
            }
            curr = curr->left;
        }
        else if (curr->data < value)
        {
            if (curr->right == nullptr)
            {
                curr->right = newNode;
                return true;
            }
            curr = curr->right;
        }
        else
        {
            // Duplicate — silently rejected
            delete newNode;
            return false;
        }
    }
}

template<typename T>
void Bst<T>::deleteNode(const T& value)
{
    bool removed = false;
    root = deleteHelper(root, value, removed);
    if (!removed)
        std::cout << "deleteNode: value not found in tree." << std::endl;
}

/**
 * Iterative search.
 * Walks left/right with a plain pointer loop.
 * No recursive frames — depth-independent.
 */
template<typename T>
bool Bst<T>::search(const T& value) const
{
    BstNode<T>* curr = root;
    while (curr != nullptr)
    {
        if (value == curr->data) return true;
        if (value <  curr->data) curr = curr->left;
        else                     curr = curr->right;
    }
    return false;
}

template<typename T>
bool Bst<T>::isEmpty() const { return root == nullptr; }

template<typename T>
int Bst<T>::size() const { return sizeHelper(root); }

template<typename T>
void Bst<T>::inOrderTraversal(f1Typ f1) const { inOrderHelper(f1, root); }

template<typename T>
void Bst<T>::preOrderTraversal(f1Typ f1) const { preOrderHelper(f1, root); }

template<typename T>
void Bst<T>::postOrderTraversal(f1Typ f1) const { postOrderHelper(f1, root); }

// ---- Private helpers ----

/**
 * Iterative destroyTree using a heap-allocated pointer stack.
 *
 * Performs a pre-order walk: push root, then loop:
 *   pop node -> push right child -> push left child -> delete node.
 *
 * The stack is a plain heap array (new/delete[]) that doubles in capacity
 * when full.  Starting at 64 slots it reaches 524,288 slots after 13
 * doublings — enough to hold the entire 509,000-node timestamp tree in
 * memory at once.  No call-stack frames are used.
 */
template<typename T>
void Bst<T>::destroyTree(BstNode<T>* node)
{
    if (node == nullptr) return;

    // Initial stack capacity; will double as needed
    int capacity = 64;
    int top      = 0;
    BstNode<T>** stk = new BstNode<T>*[capacity];

    stk[top++] = node;

    while (top > 0)
    {
        BstNode<T>* curr = stk[--top];

        // Push children before deleting current node
        if (curr->right != nullptr)
        {
            if (top == capacity)
            {
                // Double the stack capacity
                int newCap = capacity * 2;
                BstNode<T>** bigger = new BstNode<T>*[newCap];
                for (int i = 0; i < top; i++)
                    bigger[i] = stk[i];
                delete[] stk;
                stk      = bigger;
                capacity = newCap;
            }
            stk[top++] = curr->right;
        }

        if (curr->left != nullptr)
        {
            if (top == capacity)
            {
                int newCap = capacity * 2;
                BstNode<T>** bigger = new BstNode<T>*[newCap];
                for (int i = 0; i < top; i++)
                    bigger[i] = stk[i];
                delete[] stk;
                stk      = bigger;
                capacity = newCap;
            }
            stk[top++] = curr->left;
        }

        delete curr;
    }

    delete[] stk;
}

template<typename T>
BstNode<T>* Bst<T>::copyTree(BstNode<T>* node) const
{
    if (node == nullptr) return nullptr;
    BstNode<T>* n = new BstNode<T>;
    n->data  = node->data;
    n->left  = copyTree(node->left);
    n->right = copyTree(node->right);
    return n;
}

template<typename T>
BstNode<T>* Bst<T>::deleteHelper(BstNode<T>* node, const T& value, bool& removed)
{
    if (node == nullptr) return nullptr;

    if      (value < node->data) node->left  = deleteHelper(node->left,  value, removed);
    else if (node->data < value) node->right = deleteHelper(node->right, value, removed);
    else
    {
        removed = true;

        if (node->left == nullptr)
        {
            BstNode<T>* r = node->right;
            delete node;
            return r;
        }
        if (node->right == nullptr)
        {
            BstNode<T>* l = node->left;
            delete node;
            return l;
        }

        // Two children: replace with in-order successor
        BstNode<T>* succ = node->right;
        while (succ->left != nullptr)
            succ = succ->left;
        node->data = succ->data;
        bool dummy = false;
        node->right = deleteHelper(node->right, succ->data, dummy);
    }
    return node;
}

// In a const method, root is BstNode<T>* const so node->data is T (non-const)
// and f1(node->data) compiles correctly with f1Typ = void(*)(T&).
template<typename T>
void Bst<T>::inOrderHelper(f1Typ f1, BstNode<T>* node) const
{
    if (node == nullptr) return;
    inOrderHelper(f1, node->left);
    f1(node->data);
    inOrderHelper(f1, node->right);
}

template<typename T>
void Bst<T>::preOrderHelper(f1Typ f1, BstNode<T>* node) const
{
    if (node == nullptr) return;
    f1(node->data);
    preOrderHelper(f1, node->left);
    preOrderHelper(f1, node->right);
}

template<typename T>
void Bst<T>::postOrderHelper(f1Typ f1, BstNode<T>* node) const
{
    if (node == nullptr) return;
    postOrderHelper(f1, node->left);
    postOrderHelper(f1, node->right);
    f1(node->data);
}

template<typename T>
int Bst<T>::sizeHelper(BstNode<T>* node) const
{
    if (node == nullptr) return 0;
    return 1 + sizeHelper(node->left) + sizeHelper(node->right);
}

#endif // BST_H
