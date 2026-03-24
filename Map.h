#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "Vector.h"

    /**
     * @file Map.h
     * @brief Minimal but complete generic Map (key->value associative container).
     *
     * @details
     * Internally stores keys and their associated values in two parallel
     * Vector<K> and Vector<V> arrays (no std::map dependency).
     * If the underlying storage is later changed to a different structure,
     * client code is completely unaffected because all access goes through
     * the public interface below.
     *
     * Design decisions:
     * - No duplicate keys. Each key maps to exactly one value. In this
     *   assignment, year and month combinations are unique, so duplicate
     *   keys would represent data corruption. Duplicate detection is
     *   enforced inside operator[] (inserts only when absent).
     * - operator[] is the primary insert-or-access idiom (same semantic
     *   as std::map). A separate insert() is omitted because operator[]
     *   already creates-if-absent, and the assignment never needs to
     *   distinguish "first insertion" from "update".
     * - getPtr() returns a raw pointer rather than copying the value.
     *   This is essential for the nested Map<int,Map<int,MonthlyData>>
     *   pattern in WeatherDataStore: copying the inner Map would be
     *   expensive and would lose the reference to the live data.
     * - getKey(i) / getValue(i) provide index-based iteration so
     *   GetMonthDataAllYears can walk every year entry without needing
     *   a full iterator class.
     *
     * K must support operator== for findIndex.
     * V must be default-constructible for operator[].
     *
     * @tparam K  Key type.   Must support operator==.
     * @tparam V  Value type. Must be default-constructible.
     *
     * @author Deston
     * @version 1.0
     * @date 23/03/2026
     */
template<typename K, typename V>
class Map
{
public:

    /** @brief Default constructor. Creates an empty Map. */
    Map();

    /**
     * @brief Insert-or-access operator.
     *
     * If key is not present, a default-constructed V is appended and a
     * reference to it is returned. If key is already present, a reference
     * to the existing value is returned.
     * This is the primary way to build the Map and to chain nested Maps
     * (e.g. m_data[year][month]).
     *
     * @param key  Key to look up or insert.
     * @return     Reference to the value associated with key.
     */
    V& operator[](const K& key);

    /**
     * @brief Test whether a key is present.
     * @param key  Key to look up.
     * @return true if the key exists; false otherwise.
     */
    bool contains(const K& key) const;

    /**
     * @brief Remove the entry for key.
     *
     * Needed to complete the create/read/update/delete contract.
     * Not currently called by WeatherDataStore but provided for completeness.
     *
     * @param key  Key to remove.
     * @return true if the key was found and removed; false if not present.
     */
    bool remove(const K& key);

    /**
     * @brief Non-const pointer to the value for key, or nullptr.
     *
     * Avoids copying large values (e.g., inner Map<int,MonthlyData>)
     * and gives a mutable reference when the caller needs to append data.
     *
     * @param key  Key to look up.
     * @return Pointer to the stored value, or nullptr if key not found.
     */
    V* getPtr(const K& key);

    /**
     * @brief Const pointer to the value for key, or nullptr.
     *
     * Same as the non-const overload but safe for const Map& references.
     * Used in WeatherDataStore::HasData and GetMonthData.
     *
     * @param key  Key to look up.
     * @return Const pointer to the stored value, or nullptr if not found.
     */
    const V* getPtr(const K& key) const;

    /**
     * @brief Number of key-value pairs currently stored.
     * @return Entry count.
     */
    int size() const;

    /**
     * @brief Check whether the Map contains no entries.
     * @return true if size() == 0.
     */
    bool isEmpty() const;

    /**
     * @brief Read the key at a given index (for iteration).
     *
     * Combined with getValue(i) and size(), this gives a simple
     * index-based loop over all entries — needed by
     * WeatherDataStore::GetMonthDataAllYears.
     *
     * @param index  Zero-based position (0 <= index < size()).
     * @return       Const reference to the key at that position.
     */
    const K& getKey(int index) const;

    /**
     * @brief Read-write access to the value at a given index.
     * @param index  Zero-based position (0 <= index < size()).
     * @return       Reference to the value at that position.
     */
    V& getValue(int index);

    /**
     * @brief Read-only access to the value at a given index.
     * @param index  Zero-based position (0 <= index < size()).
     * @return       Const reference to the value at that position.
     */
    const V& getValue(int index) const;

private:

    Vector<K> m_keys;    ///< Parallel array of keys.
    Vector<V> m_values;  ///< Parallel array of values; m_values[i] belongs to m_keys[i].

    /**
     * @brief Linear search for key in m_keys.
     * @param key  Key to find.
     * @return     Zero-based index of the key, or -1 if not found.
     */
    int findIndex(const K& key) const;
};


// ---------------------------------------------------------------------------
// Implementation (header-only because Map is a template)
// ---------------------------------------------------------------------------

template<typename K, typename V>
Map<K, V>::Map()
{
    // Vector default-constructs to empty; nothing extra needed
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
        // Key absent: insert a default-constructed value
        m_keys.Append(key);
        m_values.Append(V());
        return m_values[m_values.Size() - 1];
    }
    return m_values[idx];
}

template<typename K, typename V>
bool Map<K, V>::contains(const K& key) const
{
    return findIndex(key) != -1;
}

template<typename K, typename V>
bool Map<K, V>::remove(const K& key)
{
    int idx = findIndex(key);
    if (idx == -1)
        return false;

    m_keys.Delete(idx);
    m_values.Delete(idx);
    return true;
}

template<typename K, typename V>
V* Map<K, V>::getPtr(const K& key)
{
    int idx = findIndex(key);
    if (idx == -1)
        return nullptr;

    return &m_values[idx];
}

template<typename K, typename V>
const V* Map<K, V>::getPtr(const K& key) const
{
    int idx = findIndex(key);
    if (idx == -1)
        return nullptr;

    return &m_values[idx];
}

template<typename K, typename V>
int Map<K, V>::size() const
{
    return m_keys.Size();
}

template<typename K, typename V>
bool Map<K, V>::isEmpty() const
{
    return m_keys.Empty();
}

template<typename K, typename V>
const K& Map<K, V>::getKey(int index) const
{
    return m_keys[index];
}

template<typename K, typename V>
V& Map<K, V>::getValue(int index)
{
    return m_values[index];
}

template<typename K, typename V>
const V& Map<K, V>::getValue(int index) const
{
    return m_values[index];
}

#endif // MAP_H_INCLUDED
