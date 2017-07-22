#include "gameinfo.hpp"
#include <assert.h>
#include <iomanip>
#include <ostream>
#include <sstream>

bool Game::from_lines(const std::vector<std::string> lines) {
  bool valid_game = false;
  this->moves.clear();
  bool move_lines = false;

  for (auto line : lines) {
    if (line == "Moves") {
      move_lines = true;
    } else {
      if (move_lines) {
        std::stringstream ss(line);
        std::string move_num, white_move, black_move;

        ss >> move_num >> white_move >> black_move;

        Move m(white_move, black_move);
        this->moves.push_back(m);
      } else {
        std::stringstream ss(line);
        std::string key, val;

        ss >> key >> val;

        if (key == "GameId") {
          this->id = std::stoi(val);
          valid_game = true;
        } else if (key == "PlyCount") {
          this->ply_count = std::stoi(val);
        } else if (key == "Result") {
          this->result = val;
        } else if (key == "WhiteElo") {
          this->white_elo = std::stoi(val);
        } else if (key == "BlackElo") {
          this->black_elo = std::stoi(val);
        } else {
          assert(false);
        }
      }
    }
  }

  return valid_game;
}

void move_into_stringstream(const Move &move, unsigned index,
                            std::stringstream &ss) {
  ss << std::setw(3) << index << move.to_string() << std::endl;
}

void stringstream_into_lines(std::stringstream &ss,
                             std::vector<std::string> &lines) {
  std::string line;

  while (std::getline(ss, line, '\n')) {
    lines.push_back(line);
  }
}

std::vector<std::string> Game::to_lines() {
  std::stringstream ss;
  std::vector<std::string> lines;

  ss << "GameId"
     << "       " << this->id << std::endl;
  ss << "Result"
     << "       " << this->result << std::endl;
  ss << "PlyCount"
     << "       " << this->ply_count << std::endl;
  ss << "WhiteElo"
     << "       " << this->white_elo << std::endl;
  ss << "BlackElo"
     << "       " << this->black_elo << std::endl;

  ss << "Moves" << std::endl;

  unsigned index = 1;
  for (auto move : this->moves) {
    move_into_stringstream(move, index++, ss);
  }
  stringstream_into_lines(ss, lines);

  return lines;
}

std::vector<std::string> GameWithPickedMoves::to_lines() {
  auto lines = Game::to_lines();

  std::stringstream ss;
  ss << "MovesPicked" << std::endl;

  for (unsigned index : this->picked_moves_indexes) {
    move_into_stringstream(this->moves[index], index + 1, ss);
  }
  stringstream_into_lines(ss, lines);

  return lines;
}

bool GameWithPickedMoves::from_lines(std::vector<std::string>) { return false; }

template <>
unsigned GameWithPickedMoves::pick_count<EXACTLY_N, unsigned>(unsigned limit) {
  unsigned count;

  if (limit <= this->moves.size()) {
    count = limit;
  } else {
    assert(false);
  }

  return count;
}

template <>
void GameWithPickedMoves::pick_moves<RANDOM>(unsigned count) {
  auto moves(this->moves);

  for (unsigned i = 0; i < count; ++i) {
    unsigned index = rand() % moves.size();
    this->picked_moves_indexes.push_back(index);
    moves.erase(moves.begin() + index);
  }
}
