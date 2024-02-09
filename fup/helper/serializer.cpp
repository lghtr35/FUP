#pragma once

#include <vector>
#include <algorithm>
#include <numeric>

namespace fup
{
    namespace helper
    {
        class serializer
        {
        public:
            template <typename T>
            static std::vector<T> concatenate_vectors(const std::vector<std::vector<T>> &vectors)
            {
                size_t totalLength = std::accumulate(vectors.begin(), vectors.end(), 0,
                                                     [](size_t acc, const std::vector<T> &vec)
                                                     { return acc + vec.size(); });

                std::vector<T> combinedVector(totalLength);

                for (const auto &vec : vectors)
                {
                    combinedVector.insert(combinedVector.end(), vec.begin(), vec.end());
                }

                return combinedVector;
            }
        };
    }
}
