#ifndef FEAT_EXTRACT_INCLUDED
#define FEAT_EXTRACT_INCLUDED

#include "position.h"
#include <map>
#include <sstream>
#include <vector>

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
