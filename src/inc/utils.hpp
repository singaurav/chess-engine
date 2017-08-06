#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

#include <random>
#include <vector>

class Utils {
public:
  // define a pure virtual destructor to make the class abstract
  virtual ~Utils() = 0;

  static std::normal_distribution<double>
  generate_normal_distribution(unsigned num_elements);

  template <typename T>
  static std::vector<unsigned>
  sample_indices(T distribution, unsigned num_elements, unsigned num_samples);
};

#endif // #ifndef UTILS_INCLUDED
