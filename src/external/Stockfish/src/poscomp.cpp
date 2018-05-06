#include "poscomp.h"
#include <fstream>

static bool MODEL_LOADED = false;
static net_type NET;
static std::string MODEL_FILE =
    "/Users/gauravpc/Desktop/Github/CE/chess-engine/data/trained_model.dat";

bool greater(const ValueFeat &left, const ValueFeat &right) {
  if (!MODEL_LOADED) {
    std::ifstream in(MODEL_FILE);
    dlib::deserialize(NET, in);
    MODEL_LOADED = true;
  }

  ValueFeat diff = left - right;

  sample_type test;

  for (unsigned i = 0; i < FEATURE_COUNT; ++i) {
    test(i) = float(diff.features[i]);
  }

  return NET(test) == +1;
}

bool ValueFeat::operator>(const ValueFeat &x) const {
  return greater(*this, x);
}

bool ValueFeat::operator<(const ValueFeat &x) const {
  return greater(x, *this);
}
