#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED
#include <vector>

using std::vector;

    /**
     * @file Vector.h
     * @brief Template Vector class a generic dynamic array with automatic resizing
     *
     * Provides a minimal but complete dynamic-array implementation templated
     * on element type T. Internal storage is delegated to std::vector, which
     * handles capacity doubling automatically on insertion, giving amortised
     * O(1) append time.
     *
     * All index access is bounds-unsafe by design (no exceptions thrown) for
     * performance; callers are responsible for supplying valid indices.
     *
     * @author Deston
     * @version 2.0
     * @date 06/03/2026
     *
     * @tparam T Type of elements stored in the Vector
     */

template <class T>
class Vector
{
public:
    /**
     * @brief Default constructor
     *
     * Initialises an empty Vector with a reserved capacity of 5.
     * This avoids immediate reallocation for the first few appends
     * while keeping the initial memory footprint small.
     */
    Vector() {};

    /**
     * @brief Parameterised constructor with initial capacity hint
     * @param n Desired capacity hint; the internal capacity is reserved
     *          as n*2. If n <= 0 it defaults to 5 before doubling.
     *
     * Allows callers to specify expected usage, reducing the number of
     * reallocation events. Size is 0 after construction regardless of n.
     */
    explicit Vector(int n);

    /**
     * @brief Insert element at the specified index
     * @param index Position to insert (0 to Size(), inclusive)
     * @param data  Element to insert
     * @return true if the insertion succeeded, false if index is out of range
     *
     * Elements at and after @p index are shifted right by one position.
     * Resizing is automatic; callers do not need to request it.
     */
    bool Insert(int index, const T& data);

    /**
     * @brief Delete the element at a given index
     * @param index Zero-based position to remove (0 <= index < Size())
     * @return true if deletion succeeded, false if index is out of range
     *
     * Removes the element at @p index and shifts all subsequent elements
     * one position to the left.
     */
    bool Delete(int index);

    /**
     * @brief Read-only element access
     * @param index Zero-based position to access
     * @return Const reference to the element at @p index
     *
     * No bounds checking is performed; the caller must supply a valid index.
     */
    const T& operator[](int index) const;

    /**
     * @brief Read-write element access
     * @param index Zero-based position to access
     * @return Reference to the element at @p index
     *
     * No bounds checking is performed; the caller must supply a valid index.
     */
    T& operator[](int index);

    /**
     * @brief Check whether the Vector contains no elements
     * @return true if Size() == 0, false otherwise
     */
    bool Empty() const;

    /**
     * @brief Get the current number of stored elements
     * @return Number of elements currently in the Vector
     */
    int Size() const;

    /**
     * @brief Get the current allocated capacity
     * @return Number of elements the Vector can hold before reallocation
     */
    int Capacity() const;

    /**
     * @brief Remove all elements from the Vector
     *
     * Sets Size() to 0. The allocated capacity is preserved so that
     * subsequent appends do not immediately trigger reallocation.
     */
    void Clear();

    /**
     * @brief Append an element to the end of the Vector
     * @param data Element to append
     * @return true always (capacity is grown automatically if needed)
     *
     * Convenience method equivalent to Insert(Size(), data).
     */
    bool Append(const T& data);

    /**
     * @brief Pre-allocate storage for at least n elements
     * @param n Minimum capacity to reserve
     *
     * Avoids repeated reallocation when loading large datasets.
     * Has no effect if n <= Capacity().
     */
    void Reserve(int n);

private:
    vector<T> m_vector;
};

// ----- IMPLEMENTATION -----

template <class T>
Vector<T>::Vector(int n)
{
    if (n <= 0)
    {
        n = 5;
    }
    m_vector.reserve(n);
}

template <class T>
bool Vector<T>::Insert(int index, const T& data)
{
    if (index < 0 || index > Size())
    {
        return false;
    }
    else if (index == Size())
    {
        m_vector.push_back(data);
    }
    else
    {
        m_vector.insert(m_vector.begin() + index, data);
    }
    return true;
}

template <class T>
bool Vector<T>::Delete(int index)
{
    if (index < 0 || index >= Size())
    {
        return false;
    }
    m_vector.erase(m_vector.begin() + index);
    return true;
}

template <class T>
const T& Vector<T>::operator[](int index) const
{
    return m_vector[index];
}

template <class T>
T& Vector<T>::operator[](int index)
{
    return m_vector[index];
}

template <class T>
bool Vector<T>::Empty() const
{
    return m_vector.empty();
}

template <class T>
int Vector<T>::Size() const
{
    return m_vector.size();
}

template <class T>
int Vector<T>::Capacity() const
{
    return m_vector.capacity();
}

template <class T>
void Vector<T>::Clear()
{
    m_vector.clear();
}

template <class T>
bool Vector<T>::Append(const T& data)
{
    m_vector.push_back(data);
    return true;
}

template <class T>
void Vector<T>::Reserve(int n)
{
    if (n > 0)
    {
        m_vector.reserve(n);
    }
}

#endif // VECTOR_H_INCLUDED
