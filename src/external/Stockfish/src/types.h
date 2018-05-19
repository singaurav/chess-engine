/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
  Copyright (C) 2015-2017 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

/// When compiling with provided Makefile (e.g. for Linux and OSX), configuration
/// is done automatically. To get started type 'make help'.
///
/// When Makefile is not used (e.g. with Microsoft Visual Studio) some switches
/// need to be set manually:
///
/// -DNDEBUG      | Disable debugging mode. Always use this for release.
///
/// -DNO_PREFETCH | Disable use of prefetch asm-instruction. You may need this to
///               | run on some very old machines.
///
/// -DUSE_POPCNT  | Add runtime support for use of popcnt asm-instruction. Works
///               | only in 64-bit mode and requires hardware with popcnt support.
///
/// -DUSE_PEXT    | Add runtime support for use of pext asm-instruction. Works
///               | only in 64-bit mode and requires hardware with pext support.

#include <cassert>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cstdlib>

#include <string>
#include <map>

#if defined(_MSC_VER)
// Disable some silly and noisy warning from MSVC compiler
#pragma warning(disable: 4127) // Conditional expression is constant
#pragma warning(disable: 4146) // Unary minus operator applied to unsigned type
#pragma warning(disable: 4800) // Forcing value to bool 'true' or 'false'
#endif

/// Predefined macros hell:
///
/// __GNUC__           Compiler is gcc, Clang or Intel on Linux
/// __INTEL_COMPILER   Compiler is Intel
/// _MSC_VER           Compiler is MSVC or Intel on Windows
/// _WIN32             Building on Windows (any)
/// _WIN64             Building on Windows 64 bit

#if defined(_WIN64) && defined(_MSC_VER) // No Makefile used
#  include <intrin.h> // Microsoft header for _BitScanForward64()
#  define IS_64BIT
#endif

#if defined(USE_POPCNT) && (defined(__INTEL_COMPILER) || defined(_MSC_VER))
#  include <nmmintrin.h> // Intel and Microsoft header for _mm_popcnt_u64()
#endif

#if !defined(NO_PREFETCH) && (defined(__INTEL_COMPILER) || defined(_MSC_VER))
#  include <xmmintrin.h> // Intel and Microsoft header for _mm_prefetch()
#endif

#if defined(USE_PEXT)
#  include <immintrin.h> // Header for _pext_u64() intrinsic
#  define pext(b, m) _pext_u64(b, m)
#else
#  define pext(b, m) 0
#endif

#ifdef USE_POPCNT
const bool HasPopCnt = true;
#else
const bool HasPopCnt = false;
#endif

#ifdef USE_PEXT
const bool HasPext = true;
#else
const bool HasPext = false;
#endif

#ifdef IS_64BIT
const bool Is64Bit = true;
#else
const bool Is64Bit = false;
#endif

typedef uint64_t Key;
typedef uint64_t Bitboard;

const int MAX_MOVES = 256;
const int MAX_PLY   = 128;

/// A move needs 16 bits to be stored
///
/// bit  0- 5: destination square (from 0 to 63)
/// bit  6-11: origin square (from 0 to 63)
/// bit 12-13: promotion piece type - 2 (from KNIGHT-2 to QUEEN-2)
/// bit 14-15: special move flag: promotion (1), en passant (2), castling (3)
/// NOTE: EN-PASSANT bit is set only when a pawn can be captured
///
/// Special cases are MOVE_NONE and MOVE_NULL. We can sneak these in because in
/// any normal move destination square is always different from origin square
/// while MOVE_NONE and MOVE_NULL have the same origin and destination square.

enum Move : int {
  MOVE_NONE,
  MOVE_NULL = 65
};

enum MoveType {
  NORMAL,
  PROMOTION = 1 << 14,
  ENPASSANT = 2 << 14,
  CASTLING  = 3 << 14
};

enum Color {
  WHITE, BLACK, NO_COLOR, COLOR_NB = 2
};

enum CastlingSide {
  KING_SIDE, QUEEN_SIDE, CASTLING_SIDE_NB = 2
};

enum CastlingRight {
  NO_CASTLING,
  WHITE_OO,
  WHITE_OOO = WHITE_OO << 1,
  BLACK_OO  = WHITE_OO << 2,
  BLACK_OOO = WHITE_OO << 3,
  ANY_CASTLING = WHITE_OO | WHITE_OOO | BLACK_OO | BLACK_OOO,
  CASTLING_RIGHT_NB = 16
};

template<Color C, CastlingSide S> struct MakeCastling {
  static const CastlingRight
  right = C == WHITE ? S == QUEEN_SIDE ? WHITE_OOO : WHITE_OO
                     : S == QUEEN_SIDE ? BLACK_OOO : BLACK_OO;
};

enum Phase {
  PHASE_ENDGAME,
  PHASE_MIDGAME = 128,
  MG = 0, EG = 1, PHASE_NB = 2
};

enum ScaleFactor {
  SCALE_FACTOR_DRAW    = 0,
  SCALE_FACTOR_ONEPAWN = 48,
  SCALE_FACTOR_NORMAL  = 64,
  SCALE_FACTOR_MAX     = 128,
  SCALE_FACTOR_NONE    = 255
};

enum Bound {
  BOUND_NONE,
  BOUND_UPPER,
  BOUND_LOWER,
  BOUND_EXACT = BOUND_UPPER | BOUND_LOWER
};

enum Value : int {
  VALUE_ZERO      = 0,
  VALUE_DRAW      = 0,
  VALUE_KNOWN_WIN = 10000,
  VALUE_MATE      = 32000,
  VALUE_INFINITE  = 32001,
  VALUE_NONE      = 32002,

  VALUE_MATE_IN_MAX_PLY  =  VALUE_MATE - 2 * MAX_PLY,
  VALUE_MATED_IN_MAX_PLY = -VALUE_MATE + 2 * MAX_PLY,

  PawnValueMg   = 188,   PawnValueEg   = 248,
  KnightValueMg = 764,   KnightValueEg = 848,
  BishopValueMg = 826,   BishopValueEg = 891,
  RookValueMg   = 1282,  RookValueEg   = 1373,
  QueenValueMg  = 2526,  QueenValueEg  = 2646,

  MidgameLimit  = 15258, EndgameLimit  = 3915
};


enum FeatureName : int {
  BISHOP__MINOR_BEHIND_PAWN = 0,
  BISHOP__PAWN_SUPPORTED_OCCUPIED_OUTPOST,
  BISHOP__PAWN_SUPPORTED_REACHABLE_OUTPOST,
  BISHOP__PAWN_UNSUPPORTED_OCCUPIED_OUTPOST,
  BISHOP__PAWN_UNSUPPORTED_REACHABLE_OUTPOST,
  BISHOP__PAWNS_ON_SAME_COLOR_SQUARES,
  KING__CASTLE_KING_SIDE,
  KING__CASTLE_QUEEN_SIDE,
  KING__CLOSE_ENEMIES_ONE,
  KING__CLOSE_ENEMIES_TWO,
  KING__ENEMY_OTHER_BISHOP_CHECK,
  KING__ENEMY_OTHER_KNIGHT_CHECK,
  KING__ENEMY_OTHER_ROOK_CHECK,
  KING__ENEMY_SAFE_BISHOP_CHECK,
  KING__ENEMY_SAFE_KNIGHT_CHECK,
  KING__ENEMY_SAFE_QUEEN_CHECK,
  KING__ENEMY_SAFE_ROOK_CHECK,
  KING__KING_ADJ_ZONE_ATTACKS_COUNT,
  KING__KING_ATTACKERS_COUNT,
  KING__KING_ONLY_DEFENDED,
  KING__MIN_KING_PAWN_DISTANCE,
  KING__NOT_DEFENDED_LARGER_KING_RING,
  KING__PAWNLESS_FLANK,
  KING__SHELTER_RANK_US,
  KING__SHELTER_STORM_EDGE_DISTANCE,
  KING__STORM_RANK_THEM,
  KING__STORM_TYPE_BLOCKED_BY_KING,
  KING__STORM_TYPE_BLOCKED_BY_PAWN,
  KING__STORM_TYPE_UNBLOCKED,
  KING__STORM_TYPE_UNOPPOSED,
  KNIGHT__MINOR_BEHIND_PAWN,
  KNIGHT__PAWN_SUPPORTED_OCCUPIED_OUTPOST,
  KNIGHT__PAWN_SUPPORTED_REACHABLE_OUTPOST,
  KNIGHT__PAWN_UNSUPPORTED_OCCUPIED_OUTPOST,
  KNIGHT__PAWN_UNSUPPORTED_REACHABLE_OUTPOST,
  MATERIAL__BISHOP,
  MATERIAL__KNIGHT,
  MATERIAL__PAWN,
  MATERIAL__QUEEN,
  MATERIAL__ROOK,
  MOBILITY__ALL,
  MOBILITY__BISHOP,
  MOBILITY__KNIGHT,
  MOBILITY__QUEEN,
  MOBILITY__ROOK,
  PASSED_PAWNS__AVERAGE_CANDIDATE_PASSERS,
  PASSED_PAWNS__BLOCKSQ_OUR_KING_DISTANCE,
  PASSED_PAWNS__BLOCKSQ_THEIR_KING_DISTANCE,
  PASSED_PAWNS__DEFENDED_BLOCK_SQUARE,
  PASSED_PAWNS__EMPTY_BLOCKSQ,
  PASSED_PAWNS__FRIENDLY_OCCUPIED_BLOCKSQ,
  PASSED_PAWNS__FULLY_DEFENDED_PATH,
  PASSED_PAWNS__HINDERED_PASSED_PAWN,
  PASSED_PAWNS__NO_UNSAFE_BLOCKSQ,
  PASSED_PAWNS__NO_UNSAFE_SQUARES,
  PASSED_PAWNS__TWO_BLOCKSQ_OUR_KING_DISTANCE,
  QUEEN__WEAK,
  ROOK__CASTLE,
  ROOK__ROOK_ON_OPEN_FILE,
  ROOK__ROOK_ON_PAWN,
  ROOK__ROOK_ON_SEMI_OPEN_FILE,
  ROOK__TRAPPED,
  SPACE__EXTRA_SAFE_SQUARES,
  SPACE__SAFE_SQUARES,
  THREATS__HANGING,
  THREATS__HANGING_PAWN,
  THREATS__KING_THREAT_BY_MINOR,
  THREATS__KING_THREAT_BY_ROOK,
  THREATS__MINOR_THREAT_BY_MINOR,
  THREATS__MINOR_THREAT_BY_ROOK,
  THREATS__PAWN_PUSH,
  THREATS__PAWN_THREAT_BY_MINOR,
  THREATS__PAWN_THREAT_BY_ROOK,
  THREATS__QUEEN_THREAT_BY_MINOR,
  THREATS__QUEEN_THREAT_BY_ROOK,
  THREATS__ROOK_THREAT_BY_MINOR,
  THREATS__ROOK_THREAT_BY_ROOK,
  THREATS__SAFE_PAWN,
  THREATS__THREAT_BY_KING,
  THREATS__THREAT_BY_MINOR_RANK,
  THREATS__THREAT_BY_ROOK_RANK,

  FEATURE_COUNT
};

struct ValueFeat {
  int total_counts[FEATURE_COUNT];
  int square_counts[FEATURE_COUNT][64];

  void reset() {
    for (unsigned f = 0; f < FEATURE_COUNT; ++f) {
      total_counts[f] = 0;

      for (unsigned s = 0; s < 64; ++s) {
        square_counts[f][s] = 0;
      }
    }
  }

  ValueFeat() {
    reset();
  }

  void add_square(FeatureName f, int square) {
    square_counts[f][square] += 1;
    total_counts[f] += 1;
  }

  void add_count(FeatureName f, int count) {
    square_counts[f][0] += count;
    total_counts[f] += count;
  }

  void add_bitboard(FeatureName f, Bitboard b);
  const std::string pretty(FeatureName f);

  bool compare(FeatureName name, const ValueFeat &x);
};

struct CompFeat {
  int features[FEATURE_COUNT];

  bool pos_inf = false;
  bool neg_inf = false;
  int mate_depth = 1000000;

  void reset() {
    std::memset(features, 0, sizeof features);

    pos_inf = neg_inf = false;
    mate_depth = 1000000;
  }

  CompFeat() {
    reset();
  }

  CompFeat(ValueFeat &white_features, ValueFeat &black_features) {
    features[0] =
        white_features.total_counts[0] - black_features.total_counts[0];
    features[1] =
        white_features.total_counts[1] - black_features.total_counts[1];
    features[2] =
        white_features.total_counts[2] - black_features.total_counts[2];
    features[3] =
        white_features.total_counts[3] - black_features.total_counts[3];
    features[4] =
        white_features.total_counts[4] - black_features.total_counts[4];
    features[5] =
        white_features.total_counts[5] - black_features.total_counts[5];
    features[6] =
        white_features.total_counts[6] - black_features.total_counts[6];
    features[7] =
        white_features.total_counts[7] - black_features.total_counts[7];
    features[8] =
        white_features.total_counts[8] - black_features.total_counts[8];
    features[9] =
        white_features.total_counts[9] - black_features.total_counts[9];
    features[10] =
        white_features.total_counts[10] - black_features.total_counts[10];
    features[11] =
        white_features.total_counts[11] - black_features.total_counts[11];
    features[12] =
        white_features.total_counts[12] - black_features.total_counts[12];
    features[13] =
        white_features.total_counts[13] - black_features.total_counts[13];
    features[14] =
        white_features.total_counts[14] - black_features.total_counts[14];
    features[15] =
        white_features.total_counts[15] - black_features.total_counts[15];
    features[16] =
        white_features.total_counts[16] - black_features.total_counts[16];
    features[17] =
        white_features.total_counts[17] - black_features.total_counts[17];
    features[18] =
        white_features.total_counts[18] - black_features.total_counts[18];
    features[19] =
        white_features.total_counts[19] - black_features.total_counts[19];
    features[20] =
        white_features.total_counts[20] - black_features.total_counts[20];
    features[21] =
        white_features.total_counts[21] - black_features.total_counts[21];
    features[22] =
        white_features.total_counts[22] - black_features.total_counts[22];
    features[23] =
        white_features.total_counts[23] - black_features.total_counts[23];
    features[24] =
        white_features.total_counts[24] - black_features.total_counts[24];
    features[25] =
        white_features.total_counts[25] - black_features.total_counts[25];
    features[26] =
        white_features.total_counts[26] - black_features.total_counts[26];
    features[27] =
        white_features.total_counts[27] - black_features.total_counts[27];
    features[28] =
        white_features.total_counts[28] - black_features.total_counts[28];
    features[29] =
        white_features.total_counts[29] - black_features.total_counts[29];
    features[30] =
        white_features.total_counts[30] - black_features.total_counts[30];
    features[31] =
        white_features.total_counts[31] - black_features.total_counts[31];
    features[32] =
        white_features.total_counts[32] - black_features.total_counts[32];
    features[33] =
        white_features.total_counts[33] - black_features.total_counts[33];
    features[34] =
        white_features.total_counts[34] - black_features.total_counts[34];
    features[35] =
        white_features.total_counts[35] - black_features.total_counts[35];
    features[36] =
        white_features.total_counts[36] - black_features.total_counts[36];
    features[37] =
        white_features.total_counts[37] - black_features.total_counts[37];
    features[38] =
        white_features.total_counts[38] - black_features.total_counts[38];
    features[39] =
        white_features.total_counts[39] - black_features.total_counts[39];
    features[40] =
        white_features.total_counts[40] - black_features.total_counts[40];
    features[41] =
        white_features.total_counts[41] - black_features.total_counts[41];
    features[42] =
        white_features.total_counts[42] - black_features.total_counts[42];
    features[43] =
        white_features.total_counts[43] - black_features.total_counts[43];
    features[44] =
        white_features.total_counts[44] - black_features.total_counts[44];
    features[45] =
        white_features.total_counts[45] - black_features.total_counts[45];
    features[46] =
        white_features.total_counts[46] - black_features.total_counts[46];
    features[47] =
        white_features.total_counts[47] - black_features.total_counts[47];
    features[48] =
        white_features.total_counts[48] - black_features.total_counts[48];
    features[49] =
        white_features.total_counts[49] - black_features.total_counts[49];
    features[50] =
        white_features.total_counts[50] - black_features.total_counts[50];
    features[51] =
        white_features.total_counts[51] - black_features.total_counts[51];
    features[52] =
        white_features.total_counts[52] - black_features.total_counts[52];
    features[53] =
        white_features.total_counts[53] - black_features.total_counts[53];
    features[54] =
        white_features.total_counts[54] - black_features.total_counts[54];
    features[55] =
        white_features.total_counts[55] - black_features.total_counts[55];
    features[56] =
        white_features.total_counts[56] - black_features.total_counts[56];
    features[57] =
        white_features.total_counts[57] - black_features.total_counts[57];
    features[58] =
        white_features.total_counts[58] - black_features.total_counts[58];
    features[59] =
        white_features.total_counts[59] - black_features.total_counts[59];
    features[60] =
        white_features.total_counts[60] - black_features.total_counts[60];
    features[61] =
        white_features.total_counts[61] - black_features.total_counts[61];
    features[62] =
        white_features.total_counts[62] - black_features.total_counts[62];
    features[63] =
        white_features.total_counts[63] - black_features.total_counts[63];
    features[64] =
        white_features.total_counts[64] - black_features.total_counts[64];
    features[65] =
        white_features.total_counts[65] - black_features.total_counts[65];
    features[66] =
        white_features.total_counts[66] - black_features.total_counts[66];
    features[67] =
        white_features.total_counts[67] - black_features.total_counts[67];
    features[68] =
        white_features.total_counts[68] - black_features.total_counts[68];
    features[69] =
        white_features.total_counts[69] - black_features.total_counts[69];
    features[70] =
        white_features.total_counts[70] - black_features.total_counts[70];
    features[71] =
        white_features.total_counts[71] - black_features.total_counts[71];
    features[72] =
        white_features.total_counts[72] - black_features.total_counts[72];
    features[73] =
        white_features.total_counts[73] - black_features.total_counts[73];
    features[74] =
        white_features.total_counts[74] - black_features.total_counts[74];
    features[75] =
        white_features.total_counts[75] - black_features.total_counts[75];
    features[76] =
        white_features.total_counts[76] - black_features.total_counts[76];
    features[77] =
        white_features.total_counts[77] - black_features.total_counts[77];
    features[78] =
        white_features.total_counts[78] - black_features.total_counts[78];
    features[79] =
        white_features.total_counts[79] - black_features.total_counts[79];
    features[80] =
        white_features.total_counts[80] - black_features.total_counts[80];
  }

  bool found_mate(int depth) {
    if (mate_depth <= depth)
      return true;

    return false;
  }

  CompFeat(std::string s) {
    if (s == "+ve infinity") {
      pos_inf = true;
      neg_inf = false;
    } else if (s == "-ve infinity") {
      pos_inf = false;
      neg_inf = true;
    } else {
      assert(false);
    }
  }

  CompFeat operator-() const {
    CompFeat minus_x;

    if (pos_inf || neg_inf) {
      minus_x.pos_inf = neg_inf;
      minus_x.neg_inf = pos_inf;

      return minus_x;
    }

    minus_x.features[0] = -features[0];
    minus_x.features[1] = -features[1];
    minus_x.features[2] = -features[2];
    minus_x.features[3] = -features[3];
    minus_x.features[4] = -features[4];
    minus_x.features[5] = -features[5];
    minus_x.features[6] = -features[6];
    minus_x.features[7] = -features[7];
    minus_x.features[8] = -features[8];
    minus_x.features[9] = -features[9];
    minus_x.features[10] = -features[10];
    minus_x.features[11] = -features[11];
    minus_x.features[12] = -features[12];
    minus_x.features[13] = -features[13];
    minus_x.features[14] = -features[14];
    minus_x.features[15] = -features[15];
    minus_x.features[16] = -features[16];
    minus_x.features[17] = -features[17];
    minus_x.features[18] = -features[18];
    minus_x.features[19] = -features[19];
    minus_x.features[20] = -features[20];
    minus_x.features[21] = -features[21];
    minus_x.features[22] = -features[22];
    minus_x.features[23] = -features[23];
    minus_x.features[24] = -features[24];
    minus_x.features[25] = -features[25];
    minus_x.features[26] = -features[26];
    minus_x.features[27] = -features[27];
    minus_x.features[28] = -features[28];
    minus_x.features[29] = -features[29];
    minus_x.features[30] = -features[30];
    minus_x.features[31] = -features[31];
    minus_x.features[32] = -features[32];
    minus_x.features[33] = -features[33];
    minus_x.features[34] = -features[34];
    minus_x.features[35] = -features[35];
    minus_x.features[36] = -features[36];
    minus_x.features[37] = -features[37];
    minus_x.features[38] = -features[38];
    minus_x.features[39] = -features[39];
    minus_x.features[40] = -features[40];
    minus_x.features[41] = -features[41];
    minus_x.features[42] = -features[42];
    minus_x.features[43] = -features[43];
    minus_x.features[44] = -features[44];
    minus_x.features[45] = -features[45];
    minus_x.features[46] = -features[46];
    minus_x.features[47] = -features[47];
    minus_x.features[48] = -features[48];
    minus_x.features[49] = -features[49];
    minus_x.features[50] = -features[50];
    minus_x.features[51] = -features[51];
    minus_x.features[52] = -features[52];
    minus_x.features[53] = -features[53];
    minus_x.features[54] = -features[54];
    minus_x.features[55] = -features[55];
    minus_x.features[56] = -features[56];
    minus_x.features[57] = -features[57];
    minus_x.features[58] = -features[58];
    minus_x.features[59] = -features[59];
    minus_x.features[60] = -features[60];
    minus_x.features[61] = -features[61];
    minus_x.features[62] = -features[62];
    minus_x.features[63] = -features[63];
    minus_x.features[64] = -features[64];
    minus_x.features[65] = -features[65];
    minus_x.features[66] = -features[66];
    minus_x.features[67] = -features[67];
    minus_x.features[68] = -features[68];
    minus_x.features[69] = -features[69];
    minus_x.features[70] = -features[70];
    minus_x.features[71] = -features[71];
    minus_x.features[72] = -features[72];
    minus_x.features[73] = -features[73];
    minus_x.features[74] = -features[74];
    minus_x.features[75] = -features[75];
    minus_x.features[76] = -features[76];
    minus_x.features[77] = -features[77];
    minus_x.features[78] = -features[78];
    minus_x.features[79] = -features[79];
    minus_x.features[80] = -features[80];

    return minus_x;
  }

  bool operator>(const CompFeat& x) const;
  bool operator<(const CompFeat& x) const;
};

const CompFeat COMP_FEAT_POS_INF("+ve infinity");
const CompFeat COMP_FEAT_NEG_INF("-ve infinity");

enum PieceType {
  NO_PIECE_TYPE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
  ALL_PIECES = 0,
  PIECE_TYPE_NB = 8
};

enum Piece {
  NO_PIECE,
  W_PAWN = 1, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
  B_PAWN = 9, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
  PIECE_NB = 16
};

extern Value PieceValue[PHASE_NB][PIECE_NB];

enum Depth : int {

  ONE_PLY = 1,

  DEPTH_ZERO          =  0 * ONE_PLY,
  DEPTH_QS_CHECKS     =  0 * ONE_PLY,
  DEPTH_QS_NO_CHECKS  = -1 * ONE_PLY,
  DEPTH_QS_RECAPTURES = -5 * ONE_PLY,

  DEPTH_NONE = -6 * ONE_PLY,
  DEPTH_MAX  = MAX_PLY * ONE_PLY
};

static_assert(!(ONE_PLY & (ONE_PLY - 1)), "ONE_PLY is not a power of 2");

enum Square {
  SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
  SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
  SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
  SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
  SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
  SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
  SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
  SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
  SQ_NONE,

  SQUARE_NB = 64,

  NORTH =  8,
  EAST  =  1,
  SOUTH = -NORTH,
  WEST  = -EAST,

  NORTH_EAST = NORTH + EAST,
  SOUTH_EAST = SOUTH + EAST,
  SOUTH_WEST = SOUTH + WEST,
  NORTH_WEST = NORTH + WEST
};

enum File : int {
  FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NB
};

enum Rank : int {
  RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NB
};


/// Score enum stores a middlegame and an endgame value in a single integer
/// (enum). The least significant 16 bits are used to store the endgame value
/// and the upper 16 bits are used to store the middlegame value. Take some
/// care to avoid left-shifting a signed int to avoid undefined behavior.
enum Score : int { SCORE_ZERO };

inline Score make_score(int mg, int eg) {
  return Score((int)((unsigned int)eg << 16) + mg);
}

/// Extracting the signed lower and upper 16 bits is not so trivial because
/// according to the standard a simple cast to short is implementation defined
/// and so is a right shift of a signed integer.
inline Value eg_value(Score s) {

  union { uint16_t u; int16_t s; } eg = { uint16_t(unsigned(s + 0x8000) >> 16) };
  return Value(eg.s);
}

inline Value mg_value(Score s) {

  union { uint16_t u; int16_t s; } mg = { uint16_t(unsigned(s)) };
  return Value(mg.s);
}

#define ENABLE_BASE_OPERATORS_ON(T)                             \
inline T operator+(T d1, T d2) { return T(int(d1) + int(d2)); } \
inline T operator-(T d1, T d2) { return T(int(d1) - int(d2)); } \
inline T operator-(T d) { return T(-int(d)); }                  \
inline T& operator+=(T& d1, T d2) { return d1 = d1 + d2; }      \
inline T& operator-=(T& d1, T d2) { return d1 = d1 - d2; }      \

#define ENABLE_FULL_OPERATORS_ON(T)                             \
ENABLE_BASE_OPERATORS_ON(T)                                     \
inline T operator*(int i, T d) { return T(i * int(d)); }        \
inline T operator*(T d, int i) { return T(int(d) * i); }        \
inline T& operator++(T& d) { return d = T(int(d) + 1); }        \
inline T& operator--(T& d) { return d = T(int(d) - 1); }        \
inline T operator/(T d, int i) { return T(int(d) / i); }        \
inline int operator/(T d1, T d2) { return int(d1) / int(d2); }  \
inline T& operator*=(T& d, int i) { return d = T(int(d) * i); } \
inline T& operator/=(T& d, int i) { return d = T(int(d) / i); }

ENABLE_FULL_OPERATORS_ON(Value)
ENABLE_FULL_OPERATORS_ON(PieceType)
ENABLE_FULL_OPERATORS_ON(Piece)
ENABLE_FULL_OPERATORS_ON(Color)
ENABLE_FULL_OPERATORS_ON(Depth)
ENABLE_FULL_OPERATORS_ON(Square)
ENABLE_FULL_OPERATORS_ON(File)
ENABLE_FULL_OPERATORS_ON(Rank)

ENABLE_BASE_OPERATORS_ON(Score)

#undef ENABLE_FULL_OPERATORS_ON
#undef ENABLE_BASE_OPERATORS_ON

/// Additional operators to add integers to a Value
inline Value operator+(Value v, int i) { return Value(int(v) + i); }
inline Value operator-(Value v, int i) { return Value(int(v) - i); }
inline Value& operator+=(Value& v, int i) { return v = v + i; }
inline Value& operator-=(Value& v, int i) { return v = v - i; }

/// Only declared but not defined. We don't want to multiply two scores due to
/// a very high risk of overflow. So user should explicitly convert to integer.
inline Score operator*(Score s1, Score s2);

/// Division of a Score must be handled separately for each term
inline Score operator/(Score s, int i) {
  return make_score(mg_value(s) / i, eg_value(s) / i);
}

/// Multiplication of a Score by an integer. We check for overflow in debug mode.
inline Score operator*(Score s, int i) {

  Score result = Score(int(s) * i);

  assert(eg_value(result) == (i * eg_value(s)));
  assert(mg_value(result) == (i * mg_value(s)));
  assert((i == 0) || (result / i) == s );

  return result;
}

inline Color operator~(Color c) {
  return Color(c ^ BLACK); // Toggle color
}

inline Square operator~(Square s) {
  return Square(s ^ SQ_A8); // Vertical flip SQ_A1 -> SQ_A8
}

inline Piece operator~(Piece pc) {
  return Piece(pc ^ 8); // Swap color of piece B_KNIGHT -> W_KNIGHT
}

inline CastlingRight operator|(Color c, CastlingSide s) {
  return CastlingRight(WHITE_OO << ((s == QUEEN_SIDE) + 2 * c));
}

inline Value mate_in(int ply) {
  return VALUE_MATE - ply;
}

inline Value mated_in(int ply) {
  return -VALUE_MATE + ply;
}

inline Square make_square(File f, Rank r) {
  return Square((r << 3) + f);
}

inline Piece make_piece(Color c, PieceType pt) {
  return Piece((c << 3) + pt);
}

inline PieceType type_of(Piece pc) {
  return PieceType(pc & 7);
}

inline Color color_of(Piece pc) {
  assert(pc != NO_PIECE);
  return Color(pc >> 3);
}

inline bool is_ok(Square s) {
  return s >= SQ_A1 && s <= SQ_H8;
}

inline File file_of(Square s) {
  return File(s & 7);
}

inline Rank rank_of(Square s) {
  return Rank(s >> 3);
}

inline Square relative_square(Color c, Square s) {
  return Square(s ^ (c * 56));
}

inline Rank relative_rank(Color c, Rank r) {
  return Rank(r ^ (c * 7));
}

inline Rank relative_rank(Color c, Square s) {
  return relative_rank(c, rank_of(s));
}

inline bool opposite_colors(Square s1, Square s2) {
  int s = int(s1) ^ int(s2);
  return ((s >> 3) ^ s) & 1;
}

inline Square pawn_push(Color c) {
  return c == WHITE ? NORTH : SOUTH;
}

inline Square from_sq(Move m) {
  return Square((m >> 6) & 0x3F);
}

inline Square to_sq(Move m) {
  return Square(m & 0x3F);
}

inline int from_to(Move m) {
 return m & 0xFFF;
}

inline MoveType type_of(Move m) {
  return MoveType(m & (3 << 14));
}

inline PieceType promotion_type(Move m) {
  return PieceType(((m >> 12) & 3) + KNIGHT);
}

inline Move make_move(Square from, Square to) {
  return Move((from << 6) + to);
}

template<MoveType T>
inline Move make(Square from, Square to, PieceType pt = KNIGHT) {
  return Move(T + ((pt - KNIGHT) << 12) + (from << 6) + to);
}

inline bool is_ok(Move m) {
  return from_sq(m) != to_sq(m); // Catch MOVE_NULL and MOVE_NONE
}

#endif // #ifndef TYPES_H_INCLUDED
