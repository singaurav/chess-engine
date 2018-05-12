#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

#include <random>
#include <vector>

class Utils {
public:
  // define a pure virtual destructor to make this class abstract
  virtual ~Utils() = 0;

  static std::normal_distribution<double>
  generate_normal_distribution(unsigned num_elements);

  template <typename T>
  static std::vector<unsigned>
  sample_indices(T distribution, unsigned num_elements, unsigned num_samples);

  static std::string uci_init_moves_cmd(std::vector<std::string> moves);

  static std::vector<std::string>
  get_move_cont(std::vector<std::string> init_moves, int count,
                unsigned movetime);

  static std::vector<std::string>
  get_alt_moves(std::vector<std::string> init_moves, std::string move);
};

#endif // #ifndef UTILS_INCLUDED
