#include "gameinfo.hpp"
#include "io.hpp"
#include <fstream>

int main(int argc, char *argv[]) {
  Game g;

  std::ifstream infile(argv[1]);

  while (get_game(g, infile)) {
    put_game<std::ostream>(g, std::cout);
  }

  return 0;
}
