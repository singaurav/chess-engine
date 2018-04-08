#ifndef GAME_INFO_INCLUDED
#define GAME_INFO_INCLUDED

#include "utils.hpp"
#include <random>
#include <vector>

enum GameResult { BLACK_WON, WHITE_WON, DRAW };

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

struct GameFeature {
  double white_feature_val, black_feature_val;
};

//
// -> -> -> ->
//       -> ->
//       -> ->
//
struct MoveBranch {
  std::vector<std::string> init_move_line;
  std::vector<std::string> true_continuation;
  std::vector<GameFeature> true_continuation_features;

  std::vector<std::vector<std::string>> alt_continuations;
  std::vector<std::vector<GameFeature>> alt_continuations_features;
};

class TrainGame {
private:
  unsigned id;
  GameResult result;
  unsigned ply_count;
  unsigned white_elo;
  unsigned black_elo;

  std::vector<std::string> total_move_line;
  std::vector<unsigned> sampled_winner_moves_indices;
  std::vector<MoveBranch> sampled_moves_branches;

  void reset() {
    total_move_line.clear();
    sampled_moves_branches.clear();
    sampled_winner_moves_indices.clear();
  }

  std::vector<unsigned> get_winner_moves_indices() {
    std::vector<unsigned> indices;

    unsigned index = this->result == WHITE_WON ? 0 : 1;

    while (index < this->total_move_line.size()) {
      indices.push_back(index);
      index += 2;
    }

    return indices;
  }

  MoveBranch get_move_branch(unsigned index);

  template <CountSampleStrategy Strategy, typename Limit>
  unsigned sample_count(Limit);

  template <MoveSampleStrategy Strategy> void sample_moves(unsigned);

public:
  const unsigned continuation_size = 6;
  const unsigned movetime = 20;
  const std::vector<std::string> feature_names = Utils::get_feature_names();

  bool from_lines(std::vector<std::string>);
  std::vector<std::string> to_lines();
};

#endif // #ifndef GAME_INFO_INCLUDED
