#include "gameinfo.hpp"
#include "adapter.h"
#include "utils.cpp"
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <math.h>
#include <numeric>
#include <ostream>
#include <random>
#include <set>
#include <sstream>
#include <vector>

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

  auto it = lines.begin();
  while (*it != "MovesSampled") {
    game_lines.push_back(*it);
    ++it;
  }
  ++it;
  while (it != lines.end()) {
    sampled_moves_lines.push_back(*it);
    ++it;
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

template <> void GameWithSampledMoves::sample_moves<RANDOM>(unsigned count) {

  std::uniform_int_distribution<int> d(0, moves.size() - 1);
  std::vector<unsigned> indexes =
      Utils::sample_indices<std::uniform_int_distribution<int> >(d, moves.size(),
                                                                count);
  for (unsigned i = 0; i < count; ++i)
    sampled_moves_indexes.push_back(indexes[i]);
}

template <> void GameWithSampledMoves::sample_moves<NORMAL>(unsigned count) {

  std::normal_distribution<double> d =
      Utils::generate_normal_distribution(moves.size());
  std::vector<unsigned> indexes =
      Utils::sample_indices<std::normal_distribution<double> >(d, moves.size(),
                                                              count);
  for (unsigned i = 0; i < count; ++i)
    sampled_moves_indexes.push_back(indexes[i]);
}

std::map<unsigned, std::vector<std::string>>
GameWithAltMoves::get_alt_moves_map(GameWithSampledMoves const &g) {
  std::map<unsigned, std::vector<std::string>> alt_moves_map;

  for (unsigned index : g.sampled_moves_indexes) {

    std::vector<std::string> uci_commands;
    std::string set_pos_cmd =
        "position fen "
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
        "moves";

    for (unsigned i = 0; i < index; ++i) {
      set_pos_cmd += " " + g.moves[i].white_move;
      set_pos_cmd += " " + g.moves[i].black_move;
    }

    if (g.result == "0-1") {
      set_pos_cmd += " " + g.moves[index].white_move;
    }

    uci_commands.push_back(set_pos_cmd);
    uci_commands.push_back("genmoves");

    auto uci_output_lines = Adapter::run_uci_commands(uci_commands);

    std::string winner_move = g.result == "1-0" ? g.moves[index].white_move
                                                : g.moves[index].black_move;

    assert(std::find(uci_output_lines.begin(), uci_output_lines.end(),
                     winner_move) != uci_output_lines.end());

    std::vector<std::string> alt_moves;
    for (auto move : uci_output_lines) {
      assert(move.size() > 0);
      if (move != winner_move) {
        alt_moves.push_back(move);
      }
    }

    alt_moves_map.insert(
        std::pair<unsigned, std::vector<std::string>>(index, alt_moves));
  }

  return alt_moves_map;
}

std::vector<std::string> GameWithAltMoves::to_lines() {
  auto lines = GameWithSampledMoves::to_lines();

  std::stringstream ss;
  ss << "AltMoves" << std::endl;

  for (unsigned index : this->sampled_moves_indexes) {
    ss << std::setw(3) << index + 1;
    auto alt_moves = this->alt_moves_map.at(index);
    for (std::string move : alt_moves) {
      ss << std::setw(6) << move;
    }
    ss << std::endl;
  }
  stringstream_into_lines(ss, lines);

  return lines;
}

bool GameWithAltMoves::from_lines(std::vector<std::string> lines) {
  std::vector<std::string> game_with_sampled_moves_lines, alt_moves_lines;

  auto it = lines.begin();
  while (*it != "AltMoves") {
    game_with_sampled_moves_lines.push_back(*it);
    ++it;
  }
  ++it;
  while (it != lines.end()) {
    alt_moves_lines.push_back(*it);
    ++it;
  }

  GameWithSampledMoves game_with_sampled_moves;
  if (!game_with_sampled_moves.from_lines(game_with_sampled_moves_lines)) {
    return false;
  }

  *this = game_with_sampled_moves;

  for (auto line : alt_moves_lines) {
    std::stringstream ss(line);

    unsigned index;
    ss >> index;

    std::vector<std::string> alt_moves;

    std::string alt_move;
    ss >> alt_move;
    while (ss) {
      alt_moves.push_back(alt_move);
      ss >> alt_move;
    }

    this->alt_moves_map.insert(
        std::pair<unsigned, std::vector<std::string>>(index - 1, alt_moves));
  }

  return true;
}
