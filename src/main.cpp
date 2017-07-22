#include "gameinfo.hpp"
#include "io.hpp"
#include <fstream>

int main(int argc, char *argv[]) {
  Game game;

  std::ifstream infile(argv[1]);

  while (get_game(game, infile)) {
    GameWithPickedMoves game_picked_moves(game);
    game_picked_moves.pick_moves<RANDOM>(
        game_picked_moves.pick_count<PERC, double>(10.0));
    put_game<std::ostream>(game_picked_moves, std::cout);
  }

  return 0;
}
