#include "poscomp.h"
#include <fstream>

static bool MODEL_LOADED = false;
static net_type NET;
static std::string MODEL_FILE =
    "/Users/gauravpc/Desktop/Github/CE/chess-engine/data/trained_model.dat";

float comp_value(const CompFeat &left, const CompFeat &right) {
  if (!MODEL_LOADED) {
    std::ifstream in(MODEL_FILE);
    dlib::deserialize(NET, in);
    MODEL_LOADED = true;
  }

  sample_type test;

  for (unsigned i = 0; i < FEATURE_COUNT; ++i) {
    test(i) = float(left.features[i] - right.features[i]);
  }

  return NET(test);
}

bool CompFeat::operator>(const CompFeat &x) const {
  return comp_value(*this, x) > 0.0;
}

bool CompFeat::operator<(const CompFeat &x) const {
  return comp_value(x, *this) > 0.0;
}
