#include "adapter.h"
#include "gameinfo.hpp"
#include "io.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
  std::ifstream infile(argv[1]);
  TrainGame game;

  while (get_game(game, infile)) {
    put_game<std::ostream>(game, std::cout);
  }

  return 0;
}
