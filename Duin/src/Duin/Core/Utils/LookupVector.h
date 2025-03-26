#pragma once

#include <unordered_map>
#include <vector>
#include <algorithm>

namespace duin {
    template <typename KEY, typename VALUE>
    class LookupVector 
    {
        public:
            LookupVector() {}
            ~LookupVector() {}

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
                if (it != lookupMap.end()) { return; }

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
                if (it == lookupMap.end()) { return; }

                int idx = -1;
                idx = lookupMap.at(key);
                std::swap(valueVec[idx], --valueVec.end());
                valueVec.pop_back();
                lookupMap.erase(key);
            }

            int GetElementCount()
            {
                return valueVec.size();
            }

            int Contains(KEY key)
            {
                return lookupMap.has(key);
            }

            VALUE *GetValue(KEY key)
            {
                VALUE *value = nullptr;

                auto it = lookupMap.find(key);
                if (it == lookupMap.end()) { return value; }

                int idx = lookupMap.find(key);
                value = &valueVec[idx];
                return value;
            }

        private:
            std::unordered_map<KEY, VALUE> lookupMap;
            std::vector<VALUE> valueVec;
            //Empty index stack for gaps
    };
}
