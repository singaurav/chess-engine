#ifndef GAME_INFO_INCLUDED
#define GAME_INFO_INCLUDED

#include <iomanip>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <vector>

class AbstractGame {
public:
  // need these two functions to be able to stream games to and from a file
  virtual bool from_lines(const std::vector<std::string>) = 0;
  virtual std::vector<std::string> to_lines() = 0;
};

struct Move {
  std::string white_move;
  std::string black_move;
  Move(std::string white_move, std::string black_move) {
    this->white_move = white_move;
    this->black_move = black_move;
  }

  std::string to_string() const {
    std::stringstream ss;
    ss << std::setw(10) << this->white_move << std::setw(10)
       << this->black_move;
    return ss.str();
  }
};

class Game : public AbstractGame {
public:
  // unique id for each game
  int id;
  // white won 1-0, black won 0-1 or draw 1/2-1/2
  std::string result;
  // total number of half moves in the game
  int ply_count;
  // elo of the white player
  int white_elo;
  // elo of the black player
  int black_elo;
  // moves played in the game
  std::vector<Move> moves;

  bool from_lines(std::vector<std::string>);
  std::vector<std::string> to_lines();
};

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

#endif // #ifndef GAME_INFO_INCLUDED
