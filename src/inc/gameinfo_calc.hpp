#ifndef GAME_INFO_CALC_INCLUDED
#define GAME_INFO_CALC_INCLUDED

#include "gameinfo.hpp"
#include <map>
#include <random>

enum MoveSampleStrategy {
  // sample at random
  UNIFORM,
  // sample according to a normal distribution
  NORMAL
};
enum CountSampleStrategy {
  // sample exactly a fixed number of moves.
  EXACTLY_N,
  // sample a percentage of total number of moves in the game.
  PERC
};

class GameWithSampledMoves : public Game {
public:
  GameWithSampledMoves() {}
  GameWithSampledMoves(Game const &g) : Game(g) {}

  // move vector indexes of the sampled moves
  std::vector<unsigned> sampled_moves_indexes;

  template <CountSampleStrategy Strategy, typename Limit>
  unsigned sample_count(Limit);

  template <MoveSampleStrategy Strategy> void sample_moves(unsigned);

  bool from_lines(std::vector<std::string>);
  std::vector<std::string> to_lines();
};

class GameWithAltMoves : public GameWithSampledMoves {
public:
  GameWithAltMoves() {}
  GameWithAltMoves(GameWithSampledMoves const &g) : GameWithSampledMoves(g) {
    this->alt_moves_map = get_alt_moves_map(g);
  }

  // map sampled move index to alternative moves at source position of the
  // sampled move
  std::map<unsigned, std::vector<std::string>> alt_moves_map;

  std::map<unsigned, std::vector<std::string>>
  get_alt_moves_map(GameWithSampledMoves const &g);

  bool from_lines(std::vector<std::string>);
  std::vector<std::string> to_lines();
};

#endif // #ifndef GAME_INFO_CALC_INCLUDED
