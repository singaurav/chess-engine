#include "data_gen.hpp"
#include "learn.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
  std::string mode = argv[1];

  std::ifstream in(argv[2]);
  std::ofstream out(argv[3]);

  if (mode == "train")
    train(in, out);
  else if (mode == "generate")
    generate(in, out);
  else
    assert(false);

  return 0;
}
