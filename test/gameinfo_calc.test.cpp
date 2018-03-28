#include "catch.hpp"
#include "gameinfo_calc.hpp"
#include <iostream>

struct TestCase {
  unsigned id;
  std::vector<std::string> game_lines;
  std::map<unsigned, std::vector<std::string>> expected_alt_moves_map;
};

// clang-format off
TestCase TEST_CASES[] =
{
  {
    1,
    {
      "GameId       1",
      "Result       0-1",
      "PlyCount       4",
      "WhiteElo       1000",
      "BlackElo       1000",
      "Moves",
      "1      f2f3      e7e6",
      "2      g2g4      d8h4",
      "MovesSampled",
      "1      f2f3      e7e6",
      "2      g2g4      d8h4"
    },
    {
      {1, {
            "a7a6", "a7a5", "b7b6", "b7b5", "c7c6",
            "c7c5", "d7d6", "d7d5", "e7e5", "f7f6",
            "f7f5", "g7g6", "g7g5", "h7h6", "h7h5",
            "b8a6", "b8c6", "g8f6", "g8h6"
          }
      },
      {2, {
            "a7a6", "a7a5", "b7b6", "b7b5", "c7c6",
            "c7c5", "d7d6", "d7d5", "e6e5", "f7f6",
            "f7f5", "g7g6", "g7g5", "h7h6", "h7h5",
            "b8a6", "b8c6", "d8e7", "d8f6", "d8g5",
            "e8e7", "f8e7", "f8d6", "f8c5", "f8b4",
            "f8a3", "g8f6", "g8h6", "g8e7"
          }
      },
    }
  }
};
// clang-format on

bool compare_move_maps(
    unsigned id,
    std::map<unsigned, std::vector<std::string>> expected_alt_moves_map,
    std::map<unsigned, std::vector<std::string>> actual_alt_moves_map) {
  unsigned expected_map_size = expected_alt_moves_map.size();
  unsigned actual_map_size = actual_alt_moves_map.size();

  if (expected_map_size != actual_map_size) {
    std::cout << "test case: " << id << std::endl
              << "map sizes don't match" << std::endl
              << "actual: " << actual_map_size << std::endl
              << "expected: " << expected_map_size << std::endl;
    return false;
  }

  for (const auto &p : expected_alt_moves_map) {
    if (actual_alt_moves_map.find(p.first - 1) == actual_alt_moves_map.end()) {
      std::cout << "test case: " << id << std::endl
                << "key not found: " << p.first << std::endl;
      return false;
    } else {
      auto expected_alt_moves = expected_alt_moves_map.at(p.first);
      auto actual_alt_moves = actual_alt_moves_map.at(p.first - 1);

      if (expected_alt_moves.size() != actual_alt_moves.size()) {
        std::cout << "test case: " << id << std::endl
                  << "key: " << p.first << std::endl
                  << "alt moves sizes don't match" << std::endl
                  << "actual: " << actual_alt_moves.size() << std::endl
                  << "expected: " << expected_alt_moves.size() << std::endl;

        return false;
      } else {
        for (const auto &m : expected_alt_moves) {
          if (std::find(actual_alt_moves.begin(), actual_alt_moves.end(), m) ==
              actual_alt_moves.end()) {
            std::cout << "test case: " << id << std::endl
                      << "key: " << p.first << std::endl
                      << "Expected move not found: " << m << std::endl;

            return false;
          }
        }
      }
    }
  }

  return true;
}

TEST_CASE("gameinfo_calc", "get_alt_moves_map") {
  for (TestCase t : TEST_CASES) {
    GameWithSampledMoves gs;
    gs.from_lines(t.game_lines);

    GameWithAltMoves ga(gs);

    auto actual_alt_moves_map = ga.alt_moves_map;

    REQUIRE(true == compare_move_maps(t.id, t.expected_alt_moves_map,
                                      actual_alt_moves_map));
  }
}
