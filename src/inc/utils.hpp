#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

#include <random>
#include <vector>

namespace Utils {

    std::normal_distribution<double>
    generate_normal_distribution(unsigned num_elements);

    template <typename T>
    std::vector<unsigned> sample_indices(T distribution, unsigned num_elements,
                                     unsigned num_samples);
}; // namespace Utils

#endif // #ifndef UTILS_INCLUDED
