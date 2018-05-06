#ifndef POS_COMP_INCLUDED
#define POS_COMP_INCLUDED

#include "types.h"
#include <dlib/dnn.h>

typedef dlib::matrix<float, FEATURE_COUNT, 1> sample_type;

// clang-format off
using net_type = dlib::loss_binary_log<dlib::fc<1,
                 dlib::htan<dlib::fc<230,
                 dlib::input<sample_type>>>>>;
// clang-format on

bool compare(const ValueFeat &left, const ValueFeat &right);

#endif // #ifndef POS_COMP_INCLUDED
