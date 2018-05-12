#include "position.h"
#include "types.h"
#include <iostream>

void ValueFeat::add_bitboard(FeatureName f, Bitboard b) {
  while(b) {
    square_counts[f][pop_lsb(&b)] += 1;
    total_counts[f] += 1;
  }
}

const std::string ValueFeat::pretty(FeatureName name) {
  std::string s = "\n    +---+---+---+---+---+---+---+---+\n";

  for (Rank r = RANK_8; r >= RANK_1; --r) {
    s += "  " + std::to_string(r + 1) + " ";
    for (File f = FILE_A; f <= FILE_H; ++f) {
      int square_count = this->square_counts[name][make_square(f, r)];
      s += "| " + (square_count == 0 ? " " : std::to_string(square_count)) +
           " ";
    }
    s += "|\n    +---+---+---+---+---+---+---+---+\n";
  }

  s += "\n      a   b   c   d   e   f   g   h\n\n";

  return s;
}

bool ValueFeat::compare(FeatureName name, const ValueFeat &x) {
  if (total_counts[name] != x.total_counts[name])
    return false;

  for (unsigned s = 0; s < 64; ++s)
    if (square_counts[name][s] != x.square_counts[name][s])
      return false;

  return true;
}
