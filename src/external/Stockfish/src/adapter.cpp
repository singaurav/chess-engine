#include "adapter.h"
#include "init.h"
#include "search.h"
#include "thread.h"
#include "uci.h"
#include <iostream>
#include <sstream>

void Adapter::hello_from_stockfish() {
  std::cout << "Hello from Stockfish!" << std::endl;
}

static bool INIT_RUN = false;

std::vector<std::string>
Adapter::run_uci_commands(std::vector<std::string> commands) {
  if (!INIT_RUN) {
    Init::init();
    INIT_RUN = true;
  }

  std::streambuf *cout_sbuf = std::cout.rdbuf();
  std::stringstream ss;
  std::cout.rdbuf(ss.rdbuf());

  Position pos;
  std::string token;

  UCI::newgame(); // Implied ucinewgame before the first position command
  UCI::set_start_fen(pos);

  for (auto cmd : commands) {
    std::istringstream is(cmd);

    token.clear(); // getline() could return empty or blank line
    is >> std::skipws >> token;

    UCI::run_command(token, is, pos);

    Threads.main()->wait_for_search_finished();
  }

  Threads.main()->wait_for_search_finished();

  std::cout.rdbuf(cout_sbuf);
  std::vector<std::string> output_lines;
  std::string line;

  while (getline(ss, line)) {
    output_lines.push_back(line);
  }

  return output_lines;
}
