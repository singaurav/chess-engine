#ifndef IO_INCLUDED
#define IO_INCLUDED

#include "gameinfo.hpp"
#include <assert.h>
#include <iostream>
#include <string>

static const std::string GAME_END = "----------------------------------------";

template <typename InputStream>
bool get_game(TrainGame &game, InputStream &input_stream) {
  std::vector<std::string> lines;
  std::string line;

  while (getline(input_stream, line)) {
    if (line == GAME_END) {
      break;
    } else {
      lines.push_back(line);
    }
  }

  return lines.size() > 0 ? game.from_lines(lines) : false;
}

template <typename OutputStream>
void put_game(TrainGame &game, OutputStream &output_stream, std::string type) {
  std::vector<std::string> lines;

  if (type == "lines")
    lines = game.to_lines();
  else if (type == "csv_lines")
    lines = game.to_csv_lines();
  else
    assert(false);

  for (auto line : lines) {
    output_stream << line << std::endl;
  }

  if (type == "lines")
    output_stream << GAME_END << std::endl;
}

#endif // #ifndef IO_INCLUDED
