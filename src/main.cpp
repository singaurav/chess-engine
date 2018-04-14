#include "adapter.h"
#include "gameinfo.hpp"
#include "io.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
  std::ifstream infile(argv[1]);
  std::ofstream outfile(argv[2]);

  TrainGame game;
  auto feature_names = game.feature_names;

  for (std::string prefix : std::vector<std::string>{
           "LeftWhite", "LeftBlack", "RightWhite", "RightBlack"}) {
    for (std::string fn : feature_names) {
      outfile << prefix + "---" + fn + ",";
    }
  }

  outfile << "Winner" << std::endl;

  while (get_game(game, infile)) {
    put_game<std::ostream>(game, outfile, "csv_lines");
  }

  return 0;
}
