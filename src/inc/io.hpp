#ifndef IO_INCLUDED
#define IO_INCLUDED

#include "gameinfo.hpp"
#include <iostream>
#include <string>

static const std::string GAME_END = "----------------------------------------";

template <typename InputStream>
bool get_game(Game &game, InputStream &input_stream) {
  std::vector<std::string> lines;
  std::string line;

  while (getline(input_stream, line)) {
    if (line == GAME_END) {
      break;
    } else {
      lines.push_back(line);
    }
  }

  return game.from_lines(lines);
}

template <typename OutputStream>
void put_game(Game &game, OutputStream &output_stream) {
  std::vector<std::string> lines = game.to_lines();

  for (auto line : lines) {
    output_stream << line << std::endl;
  }

  output_stream << GAME_END << std::endl;
}

#endif // #ifndef IO_INCLUDED
