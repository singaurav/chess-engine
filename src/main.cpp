#include "adapter.h"
#include "gameinfo.hpp"
#include "gameinfo_calc.hpp"
#include "io.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
  Game game;

  std::ifstream infile(argv[1]);

  while (get_game(game, infile)) {
    GameWithSampledMoves game_sampled_moves(game);
    game_sampled_moves.sample_moves<UNIFORM>(
        game_sampled_moves.sample_count<PERC, double>(10.0));
    GameWithAltMoves game_alt_moves(game_sampled_moves);
    put_game<std::ostream>(game_alt_moves, std::cout);
  }
  return 0;
}
