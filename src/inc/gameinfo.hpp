#ifndef GAME_INFO_INCLUDED
#define GAME_INFO_INCLUDED

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

class AbstractGame {
public:
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
  int id;
  std::string result;
  int ply_count;
  int white_elo;
  int black_elo;
  std::vector<Move> moves;

  bool from_lines(std::vector<std::string>);
  std::vector<std::string> to_lines();
};

enum MovePickStrategy { RANDOM, NORMAL };
enum CountPickStrategy { EXACTLY_N, PERC };

class GameWithPickedMoves : public Game {
public:
  GameWithPickedMoves(Game const &g) : Game(g) {}

  std::vector<unsigned> picked_moves_indexes;

  template <CountPickStrategy Strategy, typename Limit>
  unsigned pick_count(Limit);

  template <MovePickStrategy Strategy>
  void pick_moves(unsigned);

  bool from_lines(std::vector<std::string>);
  std::vector<std::string> to_lines();
};

#endif // #ifndef GAME_INFO_INCLUDED
