#include "utils.hpp"
#include <random>
#include <set>
#include <vector>

std::normal_distribution<double>
Utils::generate_normal_distribution(unsigned num_elements) {
  unsigned stdcover = 1;
  /* 'stdcover' = how many standard deviations are covered in half the number of
   * moves Higher the value of stdcover, lower the probability of starting and
   * ending moves getting picked.
   * */
  double mean, stddev;
  double _num_elements = (double)num_elements;
  mean = (_num_elements - 1) / 2;
  stddev = (_num_elements / 2) / stdcover;
  return std::normal_distribution<double>(mean, stddev);
}

template <typename T>
std::vector<unsigned> _sample_indices(T distribution, unsigned num_elements,
                                      unsigned num_samples) {

  if (num_samples > num_elements) {
    num_samples = num_elements;
  }

  unsigned index;
  double ulimit, llimit, number, fn, cn;
  std::set<unsigned> indexes;

  /* Example to illustrate the idea.
   * If num_elements is 3.
   * llimit = -0.5, ulimit = 2.5
   * (-0.5, 0.5) -> 0
   * ( 0.5, 1.5) -> 1
   * ( 1.5, 2.5) -> 2
   */
  std::default_random_engine generator;
  generator.seed(time(0));
  llimit = -0.5, ulimit = num_elements - 0.5;

  while (num_samples) {
    /* sample from distribution and get index */
    do {
      number = distribution(generator);
    } while (number <= llimit || number >= ulimit);

    fn = floor(number);
    cn = ceil(number);
    index = ((cn - number) < (number - fn)) ? cn : fn;

    /* no repetetion of indices */
    if (indexes.find(index) != indexes.end())
      continue;

    indexes.insert(index);
    num_samples -= 1;
  }

  return std::vector<unsigned>(indexes.begin(), indexes.end());
}

template <>
std::vector<unsigned>
Utils::sample_indices(std::normal_distribution<double> distribution,
                      unsigned num_elements, unsigned num_samples) {
  return _sample_indices(distribution, num_elements, num_samples);
}

template <>
std::vector<unsigned>
Utils::sample_indices(std::uniform_int_distribution<int> distribution,
                      unsigned num_elements, unsigned num_samples) {
  return _sample_indices(distribution, num_elements, num_samples);
}
