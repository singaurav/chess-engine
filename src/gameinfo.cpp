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

std::vector<std::string> Game::to_lines() {
  std::stringstream ss;

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
  for (unsigned i = 0; i < this->moves.size(); ++i) {
    ss << std::setw(3) << i + 1 << std::setw(10) << this->moves[i].white_move
       << std::setw(10) << this->moves[i].black_move << std::endl;
  }

  std::vector<std::string> lines;
  std::string line;

  while (std::getline(ss, line, '\n')) {
    lines.push_back(line);
  }

  return lines;
}
