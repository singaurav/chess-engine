#ifndef FEAT_EXTRACT_INCLUDED
#define FEAT_EXTRACT_INCLUDED

#include "position.h"
#include <map>
#include <sstream>
#include <vector>

const std::vector<std::pair<std::string, std::string>> FEATURES = {
    {"bishop", "minor-behind-pawn"},
    {"bishop", "pawn-supported-occupied-outpost"},
    {"bishop", "pawn-supported-reachable-outpost"},
    {"bishop", "pawn-unsupported-occupied-outpost"},
    {"bishop", "pawn-unsupported-reachable-outpost"},
    {"bishop", "pawns-on-same-color-squares"},
    {"king", "castle-king-side"},
    {"king", "castle-queen-side"},
    {"king", "close-enemies-one"},
    {"king", "close-enemies-two"},
    {"king", "enemy-other-bishop-check"},
    {"king", "enemy-other-knight-check"},
    {"king", "enemy-other-rook-check"},
    {"king", "enemy-safe-bishop-check"},
    {"king", "enemy-safe-knight-check"},
    {"king", "enemy-safe-queen-check"},
    {"king", "enemy-safe-rook-check"},
    {"king", "king-adj-zone-attacks-count"},
    {"king", "king-attackers-count"},
    {"king", "king-only-defended"},
    {"king", "min-king-pawn-distance"},
    {"king", "not-defended-larger-king-ring"},
    {"king", "pawnless-flank"},
    {"king", "shelter-rank-us"},
    {"king", "shelter-storm-edge-distance"},
    {"king", "storm-rank-them"},
    {"king", "storm-type-blocked-by-king"},
    {"king", "storm-type-blocked-by-pawn"},
    {"king", "storm-type-unblocked"},
    {"king", "storm-type-unopposed"},
    {"knight", "minor-behind-pawn"},
    {"knight", "pawn-supported-occupied-outpost"},
    {"knight", "pawn-supported-reachable-outpost"},
    {"knight", "pawn-unsupported-occupied-outpost"},
    {"knight", "pawn-unsupported-reachable-outpost"},
    {"material", "bishop"},
    {"material", "knight"},
    {"material", "pawn"},
    {"material", "queen"},
    {"material", "rook"},
    {"mobility", "all"},
    {"mobility", "bishop"},
    {"mobility", "knight"},
    {"mobility", "queen"},
    {"mobility", "rook"},
    {"passed-pawns", "average-candidate-passers"},
    {"passed-pawns", "blocksq-our-king-distance"},
    {"passed-pawns", "blocksq-their-king-distance"},
    {"passed-pawns", "defended-block-square"},
    {"passed-pawns", "empty-blocksq"},
    {"passed-pawns", "friendly-occupied-blocksq"},
    {"passed-pawns", "fully-defended-path"},
    {"passed-pawns", "hindered-passed-pawn"},
    {"passed-pawns", "no-unsafe-blocksq"},
    {"passed-pawns", "no-unsafe-squares"},
    {"passed-pawns", "two-blocksq-our-king-distance"},
    {"queen", "weak"},
    {"rook", "castle"},
    {"rook", "rook-on-open-file"},
    {"rook", "rook-on-pawn"},
    {"rook", "rook-on-semi-open-file"},
    {"rook", "trapped"},
    {"space", "extra-safe-squares"},
    {"space", "safe-squares"},
    {"threats", "hanging"},
    {"threats", "hanging-pawn"},
    {"threats", "king-threat-by-minor"},
    {"threats", "king-threat-by-rook"},
    {"threats", "minor-threat-by-minor"},
    {"threats", "minor-threat-by-rook"},
    {"threats", "pawn-push"},
    {"threats", "pawn-threat-by-minor"},
    {"threats", "pawn-threat-by-rook"},
    {"threats", "queen-threat-by-minor"},
    {"threats", "queen-threat-by-rook"},
    {"threats", "rook-threat-by-minor"},
    {"threats", "rook-threat-by-rook"},
    {"threats", "safe-pawn"},
    {"threats", "threat-by-king"},
    {"threats", "threat-by-minor-rank"},
    {"threats", "threat-by-rook-rank"}};

class Feature {
public:
  double value();
};

class FeatureSquareList : public Feature {
private:
  int square_count[64];
  int total_count;

public:
  void reset() {
    for (int i = 0; i < 64; ++i) {
      square_count[i] = 0;
    }
    total_count = 0;
  }

  FeatureSquareList() : FeatureSquareList(std::vector<std::string>()) {}

  FeatureSquareList(std::vector<std::string> squares) {
    reset();

    for (std::string square : squares) {
      square_count[make_square(File(square[0] - 'a'), Rank(square[1] - '1'))] +=
          1;
      total_count += 1;
    }
  }

  FeatureSquareList(Bitboard b) {
    reset();
    add_bitboard(b);
  }

  double value() { return total_count; }

  void add_square(int square) {
    square_count[square] += 1;
    total_count += 1;
  }

  void add_count(int count) {
    square_count[0] += count;
    total_count += count;
  }

  void add_bitboard(Bitboard b) {
    while (b) {
      square_count[pop_lsb(&b)] += 1;
      total_count += 1;
    }
  }

  bool operator==(const FeatureSquareList &rhs) const {
    for (int i = 0; i < 64; ++i) {
      if (square_count[i] != rhs.square_count[i]) {
        return false;
      }
    }

    return true;
  }

  bool operator!=(const FeatureSquareList &rhs) const {
    return !(*this == rhs);
  }

  const std::string pretty() {
    std::string s = "\n    +---+---+---+---+---+---+---+---+\n";

    for (Rank r = RANK_8; r >= RANK_1; --r) {
      s += "  " + std::to_string(r + 1) + " ";
      for (File f = FILE_A; f <= FILE_H; ++f) {
        int square_count = this->square_count[make_square(f, r)];
        s += "| " + (square_count == 0 ? " " : std::to_string(square_count)) +
             " ";
      }
      s += "|\n    +---+---+---+---+---+---+---+---+\n";
    }

    s += "\n      a   b   c   d   e   f   g   h\n\n";

    return s;
  }
};

namespace ExtractFeature {
template <class F>
std::map<std::string, std::pair<F, F>>
merge_white_black_features(std::map<std::string, F> white_feature,
                           std::map<std::string, F> black_feature) {
  std::map<std::string, std::pair<F, F>> merged_feature;

  for (auto sub_f : white_feature) {
    merged_feature[sub_f.first] =
        std::pair<F, F>(sub_f.second, black_feature[sub_f.first]);
  }

  return merged_feature;
}

template <class F>
std::map<std::string, std::pair<double, double>>
feature_value(std::map<std::string, std::pair<F, F>> feature) {
  std::map<std::string, std::pair<double, double>> feature_value;

  for (auto sub_f : feature) {
    std::string key = sub_f.first;
    feature_value[key] = std::pair<double, double>(feature[key].first.value(),
                                                   feature[key].second.value());
  }

  return feature_value;
}

std::map<std::string, std::map<std::string, std::pair<double, double>>>
extract_features(const Position &pos);

std::map<std::string, std::pair<FeatureSquareList, FeatureSquareList>>
space(const Position &pos);
std::map<std::string, std::pair<FeatureSquareList, FeatureSquareList>>
mobility(const Position &pos);
std::map<std::string, std::pair<FeatureSquareList, FeatureSquareList>>
material(const Position &pos);
std::map<std::string, std::pair<FeatureSquareList, FeatureSquareList>>
queen(const Position &pos);
std::map<std::string, std::pair<FeatureSquareList, FeatureSquareList>>
bishop(const Position &pos);
std::map<std::string, std::pair<FeatureSquareList, FeatureSquareList>>
knight(const Position &pos);
std::map<std::string, std::pair<FeatureSquareList, FeatureSquareList>>
rook(const Position &pos);
std::map<std::string, std::pair<FeatureSquareList, FeatureSquareList>>
threats(const Position &pos);
std::map<std::string, std::pair<FeatureSquareList, FeatureSquareList>>
king(const Position &pos);
std::map<std::string, std::pair<FeatureSquareList, FeatureSquareList>>
passed_pawns(const Position &pos);
} // namespace ExtractFeature

#endif // #ifndef FEAT_EXTRACT_INCLUDED
