#include "catch.hpp"
#include "utils.hpp"
#include <iostream>

bool compare_alt_moves(std::vector<std::string> expected,
                       std::vector<std::string> actual) {
  if (expected.size() != actual.size()) {
    std::cout << "Sizes don't match. Expected: " << expected.size()
              << "Actual: " << actual.size();
    return false;
  }

  for (std::string m : actual) {
    if (std::find(expected.begin(), expected.end(), m) == expected.end()) {
      std::cout << "Move " << m << "found but not expected.";
      return false;
    }
  }

  for (std::string m : expected) {
    if (std::find(actual.begin(), actual.end(), m) == actual.end()) {
      std::cout << "Move " << m << "not found but expected.";
      return false;
    }
  }

  return true;
}

TEST_CASE("utils", "get_alt_moves") {
  REQUIRE(true ==
          compare_alt_moves(
              std::vector<std::string>{"a7a6", "a7a5", "b7b6", "b7b5", "c7c6",
                                       "c7c5", "d7d6", "d7d5", "e7e5", "f7f6",
                                       "f7f5", "g7g6", "g7g5", "h7h6", "h7h5",
                                       "b8a6", "b8c6", "g8f6", "g8h6"},
              Utils::get_alt_moves(std::vector<std::string>{"f2f3"}, "e7e6")));

  REQUIRE(true ==
          compare_alt_moves(
              std::vector<std::string>{"a7a6", "a7a5", "b7b6", "b7b5", "c7c6",
                                       "c7c5", "d7d6", "d7d5", "e6e5", "f7f6",
                                       "f7f5", "g7g6", "g7g5", "h7h6", "h7h5",
                                       "b8a6", "b8c6", "d8e7", "d8f6", "d8g5",
                                       "e8e7", "f8e7", "f8d6", "f8c5", "f8b4",
                                       "f8a3", "g8f6", "g8h6", "g8e7"},
              Utils::get_alt_moves(
                  std::vector<std::string>{"f2f3", "e7e6", "g2g4"}, "d8h4")));

  REQUIRE(true ==
          compare_alt_moves(
              std::vector<std::string>{"b2b4", "c2c3", "b1c3", "b1d2", "d1d2",
                                       "e1e2"},
              Utils::get_alt_moves(
                  std::vector<std::string>{"d2d3", "c7c6", "e2e4", "d8a5"},
                  "c1d2")));

  REQUIRE(
      true ==
      compare_alt_moves(
          std::vector<std::string>{
              "a2a3", "a2a4", "b2b3", "b2b4", "c2c3", "c2c4", "d3d4",
              "e4e5", "f2f3", "f2f4", "g2g3", "g2g4", "h2h3", "h2h4",
              "b1a3", "b1c3", "d1c1", "d1e2", "d1f3", "d1g4", "d1h5",
              "e1e2", "f1e2", "g1h3", "g1f3", "g1e2", "d2c3", "d2b4"},
          Utils::get_alt_moves(std::vector<std::string>{"d2d3", "c7c6", "e2e4",
                                                        "d8a5", "c1d2", "g8f6"},
                               "d2a5")));
}
