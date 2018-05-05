#ifndef DATA_GEN_INCLUDED
#define DATA_GEN_INCLUDED

#include "adapter.h"
#include "gameinfo.hpp"
#include "io.hpp"
#include <fstream>
#include <iostream>

void generate(std::istream &in, std::ostream &out) {
  TrainGame game;
  auto feature_names = game.feature_names;

  for (std::string fn : feature_names) {
    out << fn + ",";
  }

  out << "Winner" << std::endl;

  unsigned count = 0;
  while (get_game(game, in)) {
    put_game<std::ostream>(game, out, "csv_lines");
    std::cout << " --- processed game: " << ++count << std::endl;
  }
}

#endif // #ifndef DATA_GEN_INCLUDED
