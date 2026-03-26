#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "Vector.h"

    /**
     * @file Map.h
     * @brief Minimal but complete generic Map (key->value associative container).
     *
     * @details
     * Internally stores keys and values in two parallel Vector<K> and Vector<V>
     * arrays.  No std::map dependency.  If the underlying storage is later changed
     * to a different structure, client code is completely unaffected because all
     * access goes through the public interface below.
     *
     * PUBLIC INTERFACE — exactly 4 methods (treating const/non-const overloads
     * of getPtr as one logical method):
     *
     *   operator[]   — primary insert-or-access (builds nested Maps in one line)
     *   getPtr()     — safe read: returns nullptr for absent key (replaces contains)
     *   size()       — entry count (size()==0 replaces isEmpty)
     *   getKey(i)    — key at index i for iteration (combined with getPtr replaces getValue)
     *
     * Design decisions:
     * - No duplicate keys.  Each key maps to exactly one value.  Year and month
     *   combinations are unique; duplicates would represent data corruption.
     *   Enforced inside operator[] (inserts only when absent).
     * - operator[] is the primary insert-or-access idiom.  A separate insert()
     *   is omitted because operator[] already creates-if-absent.
     * - getPtr() returns a raw pointer rather than copying the value.  Essential
     *   for the nested Map<int,Map<int,MonthlyData>> pattern in WeatherDataStore:
     *   copying the inner Map would be expensive and would lose the live reference.
     * - getKey(i) combined with getPtr(getKey(i)) gives full index-based iteration
     *   without a separate getValue() or iterator class.
     *
     * K must support operator== for findIndex.
     * V must be default-constructible for operator[].
     *
     * @tparam K  Key type.   Must support operator==.
     * @tparam V  Value type. Must be default-constructible.
     *
     * @author Deston
     * @version 2.0
     * @date 25/03/2026
     */
template<typename K, typename V>
class Map
{
public:

    /** @brief Default constructor.  Creates an empty Map. */
    Map();

    /**
     * @brief Insert-or-access operator.
     *
     * If key is not present, a default-constructed V is appended and a
     * reference to it is returned.  If key is already present, a reference
     * to the existing value is returned.
     *
     * This is the primary way to build the Map and to chain nested Maps:
     *   m_data[year][month].windSpeeds.Append(...)
     *
     * @param key  Key to look up or insert.
     * @return     Reference to the value associated with key.
     */
    V& operator[](const K& key);

    /**
     * @brief Non-const pointer to the value for key, or nullptr if absent.
     *
     * Replaces both the old contains() (test: getPtr(key) != nullptr) and
     * the old getValue(i) (access: getPtr(getKey(i))).
     * Returns nullptr rather than throwing so callers can guard naturally.
     *
     * @param key  Key to look up.
     * @return     Pointer to stored value, or nullptr if key not found.
     */
    V* getPtr(const K& key);

    /**
     * @brief Const pointer to the value for key, or nullptr if absent.
     *
     * Const overload of getPtr — required so const Map& references (used in
     * WeatherDataStore's const methods) can still perform safe key lookups
     * without returning a mutable pointer.
     *
     * @param key  Key to look up.
     * @return     Const pointer to stored value, or nullptr if not found.
     */
    const V* getPtr(const K& key) const;

    /**
     * @brief Number of key-value pairs currently stored.
     *
     * Replaces the old isEmpty(): size() == 0 is equivalent.
     *
     * @return Entry count.
     */
    int size() const;

    /**
     * @brief Read the key at a given index (for index-based iteration).
     *
     * Combined with getPtr(getKey(i)) and size(), this provides a complete
     * iteration pattern over all entries without a separate iterator class
     * or getValue() method:
     *
     *   for (int i = 0; i < m_data.size(); i++)
     *   {
     *       const V* val = m_data.getPtr(m_data.getKey(i));
     *       ...
     *   }
     *
     * @param index  Zero-based position (0 <= index < size()).
     * @return       Const reference to the key at that position.
     */
    const K& getKey(int index) const;

private:

    Vector<K> m_keys;   ///< Parallel array of keys.
    Vector<V> m_values; ///< Parallel array of values; m_values[i] belongs to m_keys[i].

    /**
     * @brief Linear search for key in m_keys.
     * @param key  Key to find.
     * @return     Zero-based index, or -1 if not found.
     */
    int findIndex(const K& key) const;
};


// ---------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------

template<typename K, typename V>
Map<K, V>::Map()
{
    // Vector default-constructs to empty
}

template<typename K, typename V>
int Map<K, V>::findIndex(const K& key) const
{
    for (int i = 0; i < m_keys.Size(); i++)
    {
        if (m_keys[i] == key)
            return i;
    }
    return -1;
}

template<typename K, typename V>
V& Map<K, V>::operator[](const K& key)
{
    int idx = findIndex(key);
    if (idx == -1)
    {
        m_keys.Append(key);
        m_values.Append(V());
        return m_values[m_values.Size() - 1];
    }
    return m_values[idx];
}

template<typename K, typename V>
V* Map<K, V>::getPtr(const K& key)
{
    int idx = findIndex(key);
    if (idx == -1) return nullptr;
    return &m_values[idx];
}

template<typename K, typename V>
const V* Map<K, V>::getPtr(const K& key) const
{
    int idx = findIndex(key);
    if (idx == -1) return nullptr;
    return &m_values[idx];
}

template<typename K, typename V>
int Map<K, V>::size() const
{
    return m_keys.Size();
}

template<typename K, typename V>
const K& Map<K, V>::getKey(int index) const
{
    return m_keys[index];
}

#endif // MAP_H_INCLUDED
