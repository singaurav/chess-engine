#include "gameinfo.hpp"
#include "io.hpp"
#include <fstream>

int main(int argc, char *argv[]) {
  Game game;

  std::ifstream infile(argv[1]);

  while (get_game(game, infile)) {
    GameWithSampledMoves game_sampled_moves(game);
    game_sampled_moves.sample_moves<NORMAL>(
        game_sampled_moves.sample_count<PERC, double>(10.0));
    put_game<std::ostream>(game_sampled_moves, std::cout);
    break;
  }

  return 0;
}
