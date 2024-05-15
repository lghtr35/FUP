
#include "serializer.hpp"

namespace fup
{
    namespace core
    {
        namespace entity
        {
            template <typename T>
            std::vector<T> serializer::concatenate_vectors(const std::vector<std::vector<T>> &vectors)
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

            // Serialization function for string payload
            std::vector<char> serializer::serialize_payload(const std::string &p)
            {
                return std::vector<char>(p.begin(), p.end());
            }

            // Serialization function for entities that are serializable
            template <typename T>
            std::vector<char> serializer::serialize_payload(const T &p)
            {
                return p.serialize();
            }
        };
    }
}
