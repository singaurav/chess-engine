#include "gameinfo_calc.hpp"
#include "utils.hpp"
#include <algorithm>
#include <assert.h>

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

template <> void GameWithSampledMoves::sample_moves<UNIFORM>(unsigned count) {

  std::uniform_int_distribution<int> d(0, moves.size() - 1);
  std::vector<unsigned> indexes =
      Utils::sample_indices<std::uniform_int_distribution<int>>(d, moves.size(),
                                                                count);
  for (unsigned i = 0; i < count; ++i)
    sampled_moves_indexes.push_back(indexes[i]);
}

template <> void GameWithSampledMoves::sample_moves<NORMAL>(unsigned count) {

  std::normal_distribution<double> d =
      Utils::generate_normal_distribution(moves.size());
  std::vector<unsigned> indexes =
      Utils::sample_indices<std::normal_distribution<double>>(d, moves.size(),
                                                              count);
  for (unsigned i = 0; i < count; ++i)
    sampled_moves_indexes.push_back(indexes[i]);
}

std::map<unsigned, std::vector<std::string>>
GameWithAltMoves::get_alt_moves_map(GameWithSampledMoves const &g) {
  std::map<unsigned, std::vector<std::string>> alt_moves_map;

  for (unsigned index : g.sampled_moves_indexes) {

    std::vector<std::string> init_moves;

    for (unsigned i = 0; i < index; ++i) {
      init_moves.push_back(g.moves[i].white_move);
      init_moves.push_back(g.moves[i].black_move);
    }

    if (g.result == "0-1") {
      init_moves.push_back(g.moves[index].white_move);
    }

    std::vector<std::string> uci_commands{
        Utils::uci_init_moves_cmd(init_moves)};

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
    auto alt_moves = this->alt_moves_map.at(index);
    ss << std::setw(3) << index + 1 << std::setw(4) << alt_moves.size();

    if (this->result == "1-0")
      ss << std::setw(12) << this->moves[index].white_move << std::endl;
    else if (this->result == "0-1")
      ss << std::setw(24) << this->moves[index].black_move << std::endl;
    else
      assert(false);

    for (std::string move : alt_moves) {
      if (this->result == "1-0")
        ss << std::setw(15) << move << std::endl;
      else if (this->result == "0-1")
        ss << std::setw(27) << move << std::endl;
    }
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

  unsigned index, count;
  std::vector<std::string> alt_moves;
  unsigned i = 0;

  while (i < alt_moves_lines.size()) {
    std::stringstream ss(alt_moves_lines[i]);
    ss >> index >> count;

    for (unsigned m = 0; m < count; ++m) {
      ++i;
      std::stringstream ss(alt_moves_lines[i]);

      std::string alt_move;
      ss >> alt_move;

      alt_moves.push_back(alt_move);
    }

    this->alt_moves_map.insert(
        std::pair<unsigned, std::vector<std::string>>(index - 1, alt_moves));

    alt_moves.clear();
    ++i;
  }

  return true;
}

std::map<unsigned, std::vector<std::string>>
GameWithMoveConts::get_sampled_move_conts_map(GameWithAltMoves const &g,
                                              unsigned cont_len) {
  std::map<unsigned, std::vector<std::string>> sampled_move_conts_map;

  for (unsigned index : g.sampled_moves_indexes) {

    std::vector<std::string> init_moves;

    for (unsigned i = 0; i < index; ++i) {
      init_moves.push_back(g.moves[i].white_move);
      init_moves.push_back(g.moves[i].black_move);
    }

    if (g.result == "0-1") {
      init_moves.push_back(g.moves[index].white_move);
      init_moves.push_back(g.moves[index].black_move);
    } else if (g.result == "1-0") {
      init_moves.push_back(g.moves[index].white_move);
    } else {
      assert(false);
    }

    sampled_move_conts_map.insert(std::pair<unsigned, std::vector<std::string>>(
        index, Utils::get_move_cont(init_moves, cont_len)));
  }

  return sampled_move_conts_map;
}

std::map<unsigned, std::map<unsigned, std::vector<std::string>>>
GameWithMoveConts::get_alt_move_conts_map(GameWithAltMoves const &g,
                                          unsigned cont_len) {
  std::map<unsigned, std::map<unsigned, std::vector<std::string>>>
      alt_moves_map;

  return alt_moves_map;
}

std::vector<std::string> GameWithMoveConts::to_lines() {
  auto lines = GameWithSampledMoves::to_lines();

  std::stringstream ss;
  ss << "Continuations"
     << "       " << this->cont_len << std::endl;

  for (unsigned index : this->sampled_moves_indexes) {
    auto alt_moves = this->alt_moves_map.at(index);
    ss << std::setw(3) << index + 1 << std::setw(4) << alt_moves.size();

    if (this->result == "1-0")
      ss << std::setw(12) << this->moves[index].white_move;
    else if (this->result == "0-1")
      ss << std::setw(24) << this->moves[index].black_move;
    else
      assert(false);

    ss << std::setw(6) << " -> ";

    for (auto c : this->sampled_move_conts_map.at(index))
      ss << std::setw(6) << c;

    ss << std::endl;

    for (std::string move : alt_moves) {
      if (this->result == "1-0")
        ss << std::setw(15) << move << std::endl;
      else if (this->result == "0-1")
        ss << std::setw(27) << move << std::endl;
    }
  }
  stringstream_into_lines(ss, lines);

  return lines;
}

bool GameWithMoveConts::from_lines(std::vector<std::string> lines) {
  return true;
}
