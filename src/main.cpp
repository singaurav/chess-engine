#include "gameinfo.hpp"
#include "gameinfo_calc.hpp"
#include "io.hpp"
#include "adapter.h"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
  auto lines = Adapter::run_uci_commands(std::vector<std::string>{"go"});

  for (auto line: lines) {
    std::cout << line << std::endl;
  }
  return 0;
}
