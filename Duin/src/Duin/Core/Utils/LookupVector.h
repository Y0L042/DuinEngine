#pragma once

#include <unordered_map>
#include <vector>
#include <algorithm>

namespace duin
{
/**
 * @brief LookupVector class. An unordered_map & vector pair that allows for
 * quick lookup, and quick iteration.
 *
 * NOT stable! Use KEY as the handle for an element.
 *
 */
template <typename KEY, typename VALUE>
class LookupVector
{
  public:
    LookupVector()
    {
    }

    ~LookupVector()
    {
    }

    /**
     * @brief Returns reference to vector of values.
     *
     * @return Vector of values
     */
    std::vector<VALUE> &Vector()
    {
        return valueVec;
    }

    /**
     * @brief Returns reference of map of keys:indices
     *
     * @return Map
     */
    std::unordered_map<KEY, size_t> &Map()
    {
        return lookupMap;
    }

    /**
     * @brief Inserts Value into vector, and inserts Key:Index into
     * hashmap.
     *
     * @param key
     * @param value
     */
    void Insert(KEY key, VALUE value)
    {
        auto it = lookupMap.find(key);
        if (it != lookupMap.end())
        {
            return;
        }

        valueVec.insert(value);
        int idx = valueVec.size() - 1;
        lookupMap[key] = idx;
    }

    /**
     * @brief Removes Value from vector, and Key:Index from hashmap.
     *
     * @param key
     */
    void Remove(KEY key)
    {
        auto it = lookupMap.find(key);
        if (it == lookupMap.end())
        {
            return;
        }

        int idx = -1;
        idx = lookupMap.at(key);
        std::swap(valueVec[idx], --valueVec.end());
        valueVec.pop_back();
        lookupMap.erase(key);
    }

    /**
     * @brief Get count of elements.
     *
     * @return Count
     */
    int GetElementCount()
    {
        return valueVec.size();
    }

    /**
     * @brief Returns if map contains key.
     *
     * @param key
     * @return 1 if map has key, 0 if map does not have key
     */
    int Contains(KEY key)
    {
        return lookupMap.has(key);
    }

    /**
     * @brief Returns pointer to value for key. Returns nullptr if key
     * is not found.
     *
     * @param key
     * @return Pointer to value/nullptr
     */
    VALUE *GetValue(KEY key)
    {
        VALUE *value = nullptr;

        auto it = lookupMap.find(key);
        if (it == lookupMap.end())
        {
            return value;
        }

        int idx = lookupMap.find(key);
        value = &valueVec[idx];
        return value;
    }

  private:
    std::unordered_map<KEY, size_t> lookupMap;
    std::vector<VALUE> valueVec;
};
} // namespace duin
