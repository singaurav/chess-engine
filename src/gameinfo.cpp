#include "gameinfo.hpp"
#include "adapter.h"
#include <algorithm>
#include <assert.h>
#include <iomanip>
#include <sstream>

template <>
unsigned TrainGame::sample_count<EXACTLY_N, unsigned>(unsigned limit) {
  assert(limit <= this->get_winner_moves_indices().size());
  return limit;
}

template <> unsigned TrainGame::sample_count<PERC, double>(double perc) {
  unsigned count =
      floor(perc / 100.00 * this->get_winner_moves_indices().size());

  return TrainGame::sample_count<EXACTLY_N, unsigned>(count);
}

template <> void TrainGame::sample_moves<UNIFORM>(unsigned count) {
  auto winner_moves_indices = get_winner_moves_indices();

  std::uniform_int_distribution<int> d(0, winner_moves_indices.size() - 1);
  std::vector<unsigned> indices =
      Utils::sample_indices<std::uniform_int_distribution<int>>(
          d, winner_moves_indices.size(), count);

  for (unsigned i = 0; i < count; ++i)
    this->sampled_winner_moves_indices.push_back(
        winner_moves_indices[indices[i]]);
}

template <> void TrainGame::sample_moves<NORM>(unsigned count) {
  auto winner_moves_indices = get_winner_moves_indices();

  std::normal_distribution<double> d =
      Utils::generate_normal_distribution(winner_moves_indices.size());
  std::vector<unsigned> indices =
      Utils::sample_indices<std::normal_distribution<double>>(
          d, winner_moves_indices.size(), count);
  for (unsigned i = 0; i < count; ++i)
    this->sampled_winner_moves_indices.push_back(
        winner_moves_indices[indices[i]]);
}

std::vector<GameFeature>
get_game_features(std::vector<std::string> init_moves,
                  std::vector<std::string> feature_names) {
  std::vector<GameFeature> game_features;

  std::vector<std::string> uci_commands{Utils::uci_init_moves_cmd(init_moves),
                                        "featextract"};
  auto uci_output_lines = Adapter::run_uci_commands(uci_commands);

  for (unsigned i = 0; i < uci_output_lines.size(); ++i) {
    std::stringstream ss(uci_output_lines[i]);
    std::string token;
    int16_t val;

    ss >> token >> val;

    assert(token == feature_names[i]);

    GameFeature gf;
    gf.feature_val = val;

    game_features.push_back(gf);
  }

  return game_features;
}

MoveBranch TrainGame::get_move_branch(unsigned index) {
  MoveBranch mb;

  std::vector<std::string> init_moves;

  for (unsigned i = 0; i < index; ++i) {
    mb.init_move_line.push_back(this->total_move_line[i]);
    init_moves.push_back(this->total_move_line[i]);
  }

  mb.true_continuation.push_back(this->total_move_line[index]);
  init_moves.push_back(this->total_move_line[index]);

  auto extension =
      Utils::get_move_cont(init_moves, this->continuation_size, this->movetime);
  for (std::string m : extension)
    mb.true_continuation.push_back(m);

  mb.true_continuation_features =
      get_game_features(init_moves, this->feature_names);

  init_moves.pop_back();

  auto alt_moves =
      Utils::get_alt_moves(mb.init_move_line, mb.true_continuation[0]);

  for (std::string alt_move : alt_moves) {
    std::vector<std::string> alt_continuation{alt_move};

    init_moves.push_back(alt_move);

    auto extension = Utils::get_move_cont(init_moves, this->continuation_size,
                                          this->movetime);
    for (std::string m : extension)
      alt_continuation.push_back(m);

    mb.alt_continuations.push_back(alt_continuation);
    mb.alt_continuations_features.push_back(
        get_game_features(init_moves, this->feature_names));

    init_moves.pop_back();
  }

  return mb;
}

bool TrainGame::from_lines(const std::vector<std::string> lines) {
  bool valid_game = false;
  bool move_lines = false;

  this->reset();

  for (auto line : lines) {
    if (line == "Moves") {
      move_lines = true;
    } else if (move_lines) {
      std::stringstream ss(line);
      std::string move_num, move;

      ss >> move_num >> move;

      this->total_move_line.push_back(move);
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
        if (val == "1-0")
          this->result = WHITE_WON;
        else if (val == "0-1")
          this->result = BLACK_WON;
        else if (val == "1/2-1/2")
          this->result = DRAW;
        else
          assert(false);
      } else if (key == "WhiteElo") {
        this->white_elo = std::stoi(val);
      } else if (key == "BlackElo") {
        this->black_elo = std::stoi(val);
      } else {
        assert(false);
      }
    }
  }

  assert(this->ply_count == this->total_move_line.size());

  this->sample_moves<UNIFORM>(this->sample_count<PERC, double>(20.0));

  for (unsigned index : this->sampled_winner_moves_indices)
    this->sampled_moves_branches.push_back(this->get_move_branch(index));

  return valid_game;
}

std::vector<std::string> TrainGame::to_lines() {
  std::stringstream ss;
  std::vector<std::string> lines;

  std::string result_str;

  if (this->result == BLACK_WON)
    result_str = "0-1";
  else if (this->result == WHITE_WON)
    result_str = "1-0";
  else
    result_str = "1/2-1/2";

  ss << "GameId"
     << "        " << this->id << std::endl;
  ss << "Result"
     << "        " << result_str << std::endl;
  ss << "PlyCount"
     << "        " << this->ply_count << std::endl;
  ss << "WhiteElo"
     << "        " << this->white_elo << std::endl;
  ss << "BlackElo"
     << "        " << this->black_elo << std::endl;

  ss << "Moves" << std::endl;

  for (unsigned i = 0; i < this->total_move_line.size(); ++i) {
    ss << std::setw(3) << i + 1 << (i % 2 == 0 ? std::setw(12) : std::setw(24))
       << this->total_move_line[i] << std::endl;
  }

  ss << "SampledWinnerMoves" << std::endl;

  for (unsigned i = 0; i < this->sampled_winner_moves_indices.size(); ++i) {
    ss << std::setw(3) << this->sampled_winner_moves_indices[i] + 1
       << (this->sampled_winner_moves_indices[i] % 2 == 0 ? std::setw(12)
                                                          : std::setw(24))
       << this->total_move_line[this->sampled_winner_moves_indices[i]]
       << std::endl;
  }

  ss << "SampledMoveBranches" << std::endl;

  for (unsigned i = 0; i < this->sampled_winner_moves_indices.size(); ++i) {
    ss << std::setw(3) << this->sampled_winner_moves_indices[i] + 1
       << (this->sampled_winner_moves_indices[i] % 2 == 0 ? std::setw(15)
                                                          : std::setw(27));
    for (std::string m : this->sampled_moves_branches[i].true_continuation)
      ss << m << "  ";

    ss << std::endl;

    for (GameFeature gf :
         this->sampled_moves_branches[i].true_continuation_features)
      ss << std::setw(6) << gf.feature_val;

    ss << std::endl;

    for (unsigned j = 0;
         j < this->sampled_moves_branches[i].alt_continuations.size(); ++j) {
      ss << (this->sampled_winner_moves_indices[i] % 2 == 0 ? std::setw(15)
                                                            : std::setw(27));

      for (std::string am :
           this->sampled_moves_branches[i].alt_continuations[j])
        ss << am << "  ";

      ss << std::endl;

      for (GameFeature gf :
           this->sampled_moves_branches[i].alt_continuations_features[j])
        ss << std::setw(6) << gf.feature_val;

      ss << std::endl;
    }
  }

  std::string line;

  while (std::getline(ss, line, '\n')) {
    lines.push_back(line);
  }

  return lines;
}

std::vector<std::string> TrainGame::to_csv_lines() {
  std::vector<std::string> csv_lines;

  for (unsigned i = 0; i < this->sampled_winner_moves_indices.size(); ++i) {
    for (unsigned j = 0;
         j < this->sampled_moves_branches[i].alt_continuations.size(); ++j) {
      std::stringstream ss_left, ss_right;

      for (int f = 0;
           f <
           this->sampled_moves_branches[i].true_continuation_features.size();
           ++f) {
        int16_t left = this->sampled_moves_branches[i]
                           .true_continuation_features[f]
                           .feature_val -
                       this->sampled_moves_branches[i]
                           .alt_continuations_features[j][f]
                           .feature_val;
        ss_left << left << ',';
        ss_right << -left << ',';
      }

      csv_lines.push_back(ss_left.str() + "Left");
      csv_lines.push_back(ss_right.str() + "Right");
    }
  }

  return csv_lines;
}
