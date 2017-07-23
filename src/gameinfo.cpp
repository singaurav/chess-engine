#include "gameinfo.hpp"
#include <assert.h>
#include <iomanip>
#include <math.h>
#include <ostream>
#include <sstream>

bool Game::from_lines(const std::vector<std::string> lines) {
  bool valid_game = false;
  this->moves.clear();
  bool move_lines = false;

  for (auto line : lines) {
    if (line == "Moves") {
      move_lines = true;
    } else if (move_lines) {
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

std::vector<std::string> GameWithSampledMoves::to_lines() {
  auto lines = Game::to_lines();

  std::stringstream ss;
  ss << "MovesSampled" << std::endl;

  for (unsigned index : this->sampled_moves_indexes) {
    move_into_stringstream(this->moves[index], index + 1, ss);
  }
  stringstream_into_lines(ss, lines);

  return lines;
}

bool GameWithSampledMoves::from_lines(std::vector<std::string> lines) {
  std::vector<std::string> game_lines, sampled_moves_lines;

  bool sampled_moves = false;
  for (auto line : lines) {
    if (line == "MovesSampled") {
      sampled_moves = true;
    } else if (sampled_moves) {
      sampled_moves_lines.push_back(line);
    } else {
      game_lines.push_back(line);
    }
  }

  Game game;
  if (!game.from_lines(game_lines)) {
    return false;
  }

  *this = game;

  for (auto line : sampled_moves_lines) {
    std::stringstream ss(line);

    unsigned index;
    std::string white_move, black_move;

    ss >> index >> white_move >> black_move;

    assert(this->moves[index - 1].white_move == white_move);
    assert(this->moves[index - 1].black_move == black_move);

    this->sampled_moves_indexes.push_back(index - 1);
  }

  return true;
}

template <>
unsigned
GameWithSampledMoves::sample_count<EXACTLY_N, unsigned>(unsigned limit) {
  assert(limit <= this->moves.size());
  return limit;
}

template <>
unsigned GameWithSampledMoves::sample_count<PERC, double>(double perc) {
  unsigned count = floor(perc / 100.00 * this->moves.size());

  return GameWithSampledMoves::sample_count<EXACTLY_N, unsigned>(count);
}

template <>
void GameWithSampledMoves::sample_moves<RANDOM>(unsigned count) {
  auto moves(this->moves);

  for (unsigned i = 0; i < count; ++i) {
    unsigned index = rand() % moves.size();
    this->sampled_moves_indexes.push_back(index);
    moves.erase(moves.begin() + index);
  }
}
