#include "utils.hpp"
#include "adapter.h"
#include <random>
#include <set>
#include <sstream>
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
  generator.seed(42);
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

std::string Utils::uci_init_moves_cmd(std::vector<std::string> moves) {
  std::string cmd = "position fen "
                    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
                    "moves";

  for (std::string m : moves)
    cmd += " " + m;

  return cmd;
}

std::vector<std::string>
Utils::get_move_cont(std::vector<std::string> init_moves, int count) {
  std::vector<std::string> cont;

  for (int c = 0; c < count; ++c) {
    auto init_cont = init_moves;
    init_cont.insert(init_cont.end(), cont.begin(), cont.end());

    std::vector<std::string> uci_commands{Utils::uci_init_moves_cmd(init_cont)};
    uci_commands.push_back("go movetime 100");

    auto uci_output_lines = Adapter::run_uci_commands(uci_commands);

    std::string last_line = uci_output_lines.back();

    std::stringstream ss(last_line);
    std::string token;

    ss >> token;
    ss >> token;

    if (token == "(none)")
      return cont;
    else
      cont.push_back(token);
  }

  return cont;
}
