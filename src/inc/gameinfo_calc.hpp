#ifndef GAME_INFO_CALC_INCLUDED
#define GAME_INFO_CALC_INCLUDED

#include "adapter.h"
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

class GameWithMoveConts : public GameWithAltMoves {
public:
  GameWithMoveConts() {}
  GameWithMoveConts(GameWithAltMoves const &g, unsigned cont_len)
      : GameWithAltMoves(g) {
    this->cont_len = cont_len;
    this->sampled_move_conts_map = get_sampled_move_conts_map(g, cont_len);
    this->alt_move_conts_map = get_alt_move_conts_map(g, cont_len);
  }

  std::map<unsigned, std::vector<std::string>> sampled_move_conts_map;
  std::map<unsigned, std::map<unsigned, std::vector<std::string>>>
      alt_move_conts_map;
  unsigned cont_len;

  std::map<unsigned, std::vector<std::string>>
  get_sampled_move_conts_map(GameWithAltMoves const &g, unsigned cont_len);
  std::map<unsigned, std::map<unsigned, std::vector<std::string>>>
  get_alt_move_conts_map(GameWithAltMoves const &g, unsigned cont_len);

  bool from_lines(std::vector<std::string>);
  std::vector<std::string> to_lines();
};

#endif // #ifndef GAME_INFO_CALC_INCLUDED
