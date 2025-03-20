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

            void Insert(KEY key, VALUE value)
            {
                int keyExist = 0;
                auto it = lookupMap.find(key);
                keyExist = it != lookupMap.end();
                if (keyExist) { return; }

                valueVec.insert(value);
                int idx = valueVec.size() - 1;
                lookupMap[key] = idx;
            }

            void Remove(KEY)
            {
                
                //std::swap(valueVec)
            }

            int GetElementCount()
            {

            }

            int Contains(KEY key)
            {

            }

            VALUE *GetValue(KEY key)
            {
                return nullptr;
            }

        private:
            std::unordered_map<KEY, VALUE> lookupMap;
            std::vector<VALUE> valueVec;
            //Empty index stack for gaps
    };
}
