#ifndef BST_H
#define BST_H

#include <iostream>

    /**
     * @file Bst.h
     * @brief Generic Binary Search Tree with function-pointer traversal callbacks.
     *
     * @details
     * Stores any type T that supports operator< and operator==.
     * All tree operations are implemented recursively.
     * Duplicate values are silently rejected.
     *
     * Traversal methods accept a function pointer (f1Typ) so the caller
     * controls what happens at each visited node, enabling decoupled,
     * reusable callback behaviour as described in the textbook chapter
     * "Binary Tree Traversal and Functions as Parameters".
     *
     * @tparam T  Element type.  Must support operator< and operator==.
     *
     * @author Deston
     * @version 3.0
     * @date 24/03/2026
     */


/**
 * @brief Internal node for Bst<T>.
 *
 * Holds one data value and owning pointers to left and right subtrees.
 * nullptr represents an absent child.
 *
 * @tparam T  Same element type as the containing Bst<T>.
 */
template<typename T>
struct BstNode
{
    T          data;    ///< Value stored at this node.
    BstNode*   left;    ///< Pointer to left  child; nullptr if absent.
    BstNode*   right;   ///< Pointer to right child; nullptr if absent.
};


// =========================================================================
// Bst Generic Binary Search Tree
// =========================================================================

template<typename T>
class Bst
{
public:

    /**
     * @brief Function pointer type for traversal callbacks.
     *
     * Signature: void callback(T& data)
     * The parameter is T& so the callback may read or print the value.
     * The caller must NOT modify data in a way that breaks the BST ordering
     * invariant; enforcing this is the caller's responsibility.
     */
    typedef void (*f1Typ)(T&);

    /** @brief Default constructor.  Produces an empty tree. */
    Bst();

    /** @brief Destructor.  Frees every node via recursive post-order traversal. */
    ~Bst();

    /**
     * @brief Copy constructor.  Deep-copies other.
     * @param other  Source tree to copy from.
     */
    Bst(const Bst& other);

    /**
     * @brief Copy-assignment operator.  Self-assignment safe.
     * @param other  Source tree to copy from.
     * @return       Reference to *this.
     */
    Bst& operator=(const Bst& other);

    /**
     * @brief Insert value into the tree.
     * @param  value  Element to insert.
     * @return true   if inserted; false if value already existed (duplicate).
     */
    bool insert(const T& value);

    /**
     * @brief Remove the node containing value.
     *
     * Handles leaf, one-child, and two-children cases.
     * Prints a diagnostic message to std::cout if value is not found.
     *
     * @param value  Element to remove.
     */
    void deleteNode(const T& value);

    /**
     * @brief Search for value.
     * @param  value  Element to locate.
     * @return true if found, false otherwise.
     */
    bool search(const T& value) const;

    /** @brief Returns true when the tree contains no nodes. */
    bool isEmpty() const;

    /**
     * @brief Returns the number of nodes in the tree.
     * @note O(n) visits every node.
     */
    int size() const;

    /**
     * @brief In-order traversal (left, root, right); invokes f1 at each node.
     *
     * The callback controls all output or collection behaviour; this method
     * is responsible only for traversal order.
     *
     * @param f1  Callback invoked with a non-const reference to each node's data.
     */
    void inOrderTraversal(f1Typ f1) const;

    /**
     * @brief Pre-order traversal (root, left, right); invokes f1 at each node.
     * @param f1  Callback invoked with a non-const reference to each node's data.
     */
    void preOrderTraversal(f1Typ f1) const;

    /**
     * @brief Post-order traversal (left, right, root); invokes f1 at each node.
     * @param f1  Callback invoked with a non-const reference to each node's data.
     */
    void postOrderTraversal(f1Typ f1) const;

private:

    BstNode<T>* root;   ///< Pointer to root node; nullptr when the tree is empty.

    void        destroyTree    (BstNode<T>* node);
    BstNode<T>* copyTree       (BstNode<T>* node) const;
    BstNode<T>* insertHelper   (BstNode<T>* node, const T& value, bool& added);
    BstNode<T>* deleteHelper   (BstNode<T>* node, const T& value, bool& removed);
    bool        searchHelper   (BstNode<T>* node, const T& value) const;

    /** @brief Recursive in-order helper; invokes f1 at each visited node. */
    void        inOrderHelper  (f1Typ f1, BstNode<T>* node) const;

    /** @brief Recursive pre-order helper; invokes f1 at each visited node. */
    void        preOrderHelper (f1Typ f1, BstNode<T>* node) const;

    /** @brief Recursive post-order helper; invokes f1 at each visited node. */
    void        postOrderHelper(f1Typ f1, BstNode<T>* node) const;

    int         sizeHelper     (BstNode<T>* node) const;
};


// =========================================================================
// IMPLEMENTATION
// Order matches the declaration order above exactly.
// =========================================================================

// ---- Constructors / Destructor / Assignment ----

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

template<typename T>
bool Bst<T>::insert(const T& value)
{
    bool added = false;
    root = insertHelper(root, value, added);
    return added;
}

template<typename T>
void Bst<T>::deleteNode(const T& value)
{
    bool removed = false;
    root = deleteHelper(root, value, removed);
    if (!removed)
        std::cout << "deleteNode: value not found in tree." << std::endl;
}

template<typename T>
bool Bst<T>::search(const T& value) const
{
    return searchHelper(root, value);
}

template<typename T>
bool Bst<T>::isEmpty() const
{
    return root == nullptr;
}

template<typename T>
int Bst<T>::size() const
{
    return sizeHelper(root);
}

template<typename T>
void Bst<T>::inOrderTraversal(f1Typ f1) const
{
    inOrderHelper(f1, root);
}

template<typename T>
void Bst<T>::preOrderTraversal(f1Typ f1) const
{
    preOrderHelper(f1, root);
}

template<typename T>
void Bst<T>::postOrderTraversal(f1Typ f1) const
{
    postOrderHelper(f1, root);
}

// ---- Private helpers ----

template<typename T>
void Bst<T>::destroyTree(BstNode<T>* node)
{
    if (node == nullptr) return;
    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
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
BstNode<T>* Bst<T>::insertHelper(BstNode<T>* node, const T& value, bool& added)
{
    if (node == nullptr)
    {
        added = true;
        BstNode<T>* n = new BstNode<T>;
        n->data  = value;
        n->left  = nullptr;
        n->right = nullptr;
        return n;
    }
    if      (value < node->data) node->left  = insertHelper(node->left,  value, added);
    else if (node->data < value) node->right = insertHelper(node->right, value, added);
    // equal: duplicate � added stays false, tree unchanged
    return node;
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

        // Case 1 & 2: zero or one child
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

        // Case 3: two children � replace with in-order successor (smallest in right subtree)
        BstNode<T>* succ = node->right;
        while (succ->left != nullptr)
            succ = succ->left;
        node->data = succ->data;
        bool dummy = false;
        node->right = deleteHelper(node->right, succ->data, dummy);
    }
    return node;
}

template<typename T>
bool Bst<T>::searchHelper(BstNode<T>* node, const T& value) const
{
    if (node == nullptr)     return false;
    if (value == node->data) return true;
    if (value <  node->data) return searchHelper(node->left,  value);
    return                          searchHelper(node->right, value);
}

// In a const method, root is BstNode<T>* const (const pointer to non-const node),
// so node->data is T (non-const) and f1(node->data) compiles correctly.
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
