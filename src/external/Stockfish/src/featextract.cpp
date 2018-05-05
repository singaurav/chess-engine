#include "featextract.h"
#include "position.h"
#include "types.h"
#include <iostream>

std::map<std::string, std::map<std::string, std::pair<int16_t, int16_t>>>
ExtractFeature::extract_features(const Position &pos) {
  std::map<std::string, std::map<std::string, std::pair<int16_t, int16_t>>>
      features;

  features["material"] = ExtractFeature::feature_value<FeatureSquareList>(
      ExtractFeature::material(pos));
  features["space"] = ExtractFeature::feature_value<FeatureSquareList>(
      ExtractFeature::space(pos));
  features["mobility"] = ExtractFeature::feature_value<FeatureSquareList>(
      ExtractFeature::mobility(pos));
  features["queen"] = ExtractFeature::feature_value<FeatureSquareList>(
      ExtractFeature::queen(pos));
  features["bishop"] = ExtractFeature::feature_value<FeatureSquareList>(
      ExtractFeature::bishop(pos));
  features["knight"] = ExtractFeature::feature_value<FeatureSquareList>(
      ExtractFeature::knight(pos));
  features["rook"] = ExtractFeature::feature_value<FeatureSquareList>(
      ExtractFeature::rook(pos));
  features["threats"] = ExtractFeature::feature_value<FeatureSquareList>(
      ExtractFeature::threats(pos));
  features["king"] = ExtractFeature::feature_value<FeatureSquareList>(
      ExtractFeature::king(pos));
  features["passed-pawns"] = ExtractFeature::feature_value<FeatureSquareList>(
      ExtractFeature::passed_pawns(pos));

  return features;
}
