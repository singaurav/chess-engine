#ifndef GAME_INFO_INCLUDED
#define GAME_INFO_INCLUDED

#include <assert.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

class AbstractGame {
public:
  // need these two functions to be able to stream games to and from a file
  virtual bool from_lines(const std::vector<std::string>) = 0;
  virtual std::vector<std::string> to_lines() = 0;
};

struct GameMove {
  std::string white_move;
  std::string black_move;
  GameMove(std::string white_move, std::string black_move) {
    this->white_move = white_move;
    this->black_move = black_move;
  }

  std::string to_string() const {
    std::stringstream ss;
    ss << std::setw(12) << this->white_move << std::setw(12)
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
  std::vector<GameMove> moves;

  std::vector<std::string> get_firstk_plies(unsigned count) {
    assert(count <= this->ply_count);

    std::vector<std::string> firstk_plies;

    for (unsigned i = 0; i < count / 2; ++i) {
      firstk_plies.push_back(this->moves[i].white_move);
      firstk_plies.push_back(this->moves[i].black_move);
    }

    if (count % 2 == 1)
      firstk_plies.push_back(this->moves[count / 2].white_move);

    return firstk_plies;
  }

  bool from_lines(std::vector<std::string>);
  std::vector<std::string> to_lines();
};

void move_into_stringstream(const GameMove &move, unsigned index,
                            std::stringstream &ss);

void stringstream_into_lines(std::stringstream &ss,
                             std::vector<std::string> &lines);

#endif // #ifndef GAME_INFO_INCLUDED
