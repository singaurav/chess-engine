#include "gameinfo.hpp"
#include <assert.h>
#include <iomanip>
#include <math.h>
#include <numeric>
#include <ostream>
#include <sstream>
#include <set>
#include <cmath>
#include <random>
#include <vector>
#include <iostream>
#include <ctime>

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
  std::vector<unsigned> moves_indexes(this->moves.size());
  std::iota(moves_indexes.begin(), moves_indexes.end(), 0);

  for (unsigned i = 0; i < count; ++i) {
    unsigned index = rand() % moves_indexes.size();
    this->sampled_moves_indexes.push_back(moves_indexes[index]);
    moves_indexes.erase(moves_indexes.begin() + index);
  }
}

template <>
void GameWithPickedMoves::pick_moves<NORMAL>(unsigned count) {

  unsigned num_moves, index,
           stdcover = 1;
  /* 'stdcover' = how many standard deviations are covered in half the number of moves
   * Higher the value of stdcover, lower the probability of starting and ending moves
   * getting picked.
   * */
  double mean, stddev, ulimit, llimit, number, fn, cn;
  std::set<int> indexes;

  /* Example to illustrate the idea.
   * If num_moves is 3.
   * mean = 1
   * stddev = 1.5 (assumingg stdcover = 1)
   * llimit = -0.5, ulimit = 2.5
   * [-0.5,0.5) -> 0
   * [0.5,1.5)  -> 1
   * [1.5,2.5)  -> 2
   */
  num_moves = moves.size();
  mean = (num_moves - 1) / 2;
  stddev = (num_moves / 2) / stdcover;
  std::default_random_engine generator;
  generator.seed(time(0));
  std::normal_distribution<double> distribution(mean, stddev);
  llimit = -0.5, ulimit = num_moves - 0.5;

  std::cout << "num_moves = " << num_moves << std::endl;
  std::cout << "mean = " << mean << std::endl;
  std::cout << "stddev = " << stddev << std::endl;
  std::cout << "llimit = " << llimit << std::endl;
  std::cout << "ulimit = " << ulimit << std::endl;

  while(count) {
    do {
        number = distribution(generator);
        std::cout << "Try: " << number << std::endl;
    } while(number <= llimit || number >= ulimit);
    std::cout << "Number: " << number << std::endl;
    fn = floor(number);
    cn = ceil(number);
    index = ((cn-number) < (fn-number)) ? cn : fn;
    if(indexes.find(index) != indexes.end())
        continue;
    indexes.insert(index);
    count -= 1;

  }

  /* print 1-based indexes */
  std::set<int>::iterator it;
  std::cout << "indexes = ";
  for(it=indexes.begin(); it!=indexes.end(); it++) {
    /*TODO: use cpp inserter instead of for loop*/
      this->picked_moves_indexes.push_back(*it);
    std::cout << *it+1 << " ";
  }
  std::cout << "\n";

}
