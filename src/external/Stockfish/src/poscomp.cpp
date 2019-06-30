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

  test(0) = left.features[0] - right.features[0];
  test(1) = left.features[1] - right.features[1];
  test(2) = left.features[2] - right.features[2];
  test(3) = left.features[3] - right.features[3];
  test(4) = left.features[4] - right.features[4];
  test(5) = left.features[5] - right.features[5];
  test(6) = left.features[6] - right.features[6];
  test(7) = left.features[7] - right.features[7];
  test(8) = left.features[8] - right.features[8];
  test(9) = left.features[9] - right.features[9];
  test(10) = left.features[10] - right.features[10];
  test(11) = left.features[11] - right.features[11];
  test(12) = left.features[12] - right.features[12];
  test(13) = left.features[13] - right.features[13];
  test(14) = left.features[14] - right.features[14];
  test(15) = left.features[15] - right.features[15];
  test(16) = left.features[16] - right.features[16];
  test(17) = left.features[17] - right.features[17];
  test(18) = left.features[18] - right.features[18];
  test(19) = left.features[19] - right.features[19];
  test(20) = left.features[20] - right.features[20];
  test(21) = left.features[21] - right.features[21];
  test(22) = left.features[22] - right.features[22];
  test(23) = left.features[23] - right.features[23];
  test(24) = left.features[24] - right.features[24];
  test(25) = left.features[25] - right.features[25];
  test(26) = left.features[26] - right.features[26];
  test(27) = left.features[27] - right.features[27];
  test(28) = left.features[28] - right.features[28];
  test(29) = left.features[29] - right.features[29];
  test(30) = left.features[30] - right.features[30];
  test(31) = left.features[31] - right.features[31];
  test(32) = left.features[32] - right.features[32];
  test(33) = left.features[33] - right.features[33];
  test(34) = left.features[34] - right.features[34];
  test(35) = left.features[35] - right.features[35];
  test(36) = left.features[36] - right.features[36];
  test(37) = left.features[37] - right.features[37];
  test(38) = left.features[38] - right.features[38];
  test(39) = left.features[39] - right.features[39];
  test(40) = left.features[40] - right.features[40];
  test(41) = left.features[41] - right.features[41];
  test(42) = left.features[42] - right.features[42];
  test(43) = left.features[43] - right.features[43];
  test(44) = left.features[44] - right.features[44];
  test(45) = left.features[45] - right.features[45];
  test(46) = left.features[46] - right.features[46];
  test(47) = left.features[47] - right.features[47];
  test(48) = left.features[48] - right.features[48];
  test(49) = left.features[49] - right.features[49];
  test(50) = left.features[50] - right.features[50];
  test(51) = left.features[51] - right.features[51];
  test(52) = left.features[52] - right.features[52];
  test(53) = left.features[53] - right.features[53];
  test(54) = left.features[54] - right.features[54];
  test(55) = left.features[55] - right.features[55];
  test(56) = left.features[56] - right.features[56];
  test(57) = left.features[57] - right.features[57];
  test(58) = left.features[58] - right.features[58];
  test(59) = left.features[59] - right.features[59];
  test(60) = left.features[60] - right.features[60];
  test(61) = left.features[61] - right.features[61];
  test(62) = left.features[62] - right.features[62];
  test(63) = left.features[63] - right.features[63];
  test(64) = left.features[64] - right.features[64];
  test(65) = left.features[65] - right.features[65];
  test(66) = left.features[66] - right.features[66];
  test(67) = left.features[67] - right.features[67];
  test(68) = left.features[68] - right.features[68];
  test(69) = left.features[69] - right.features[69];
  test(70) = left.features[70] - right.features[70];
  test(71) = left.features[71] - right.features[71];
  test(72) = left.features[72] - right.features[72];
  test(73) = left.features[73] - right.features[73];
  test(74) = left.features[74] - right.features[74];
  test(75) = left.features[75] - right.features[75];
  test(76) = left.features[76] - right.features[76];
  test(77) = left.features[77] - right.features[77];
  test(78) = left.features[78] - right.features[78];
  test(79) = left.features[79] - right.features[79];
  test(80) = left.features[80] - right.features[80];

  return NET(test);
}

bool CompFeat::operator>(const CompFeat &x) const {
  if (pos_inf || x.neg_inf)
    return true;

  if (neg_inf || x.pos_inf)
    return false;

  return comp_value(*this, x) > 0.0;
}

bool CompFeat::operator<(const CompFeat &x) const {
  return comp_value(x, *this) > 0.0;
}
