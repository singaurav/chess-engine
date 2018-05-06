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

const unsigned FEATURE_COUNT = 81;

const std::pair<std::string, std::string> FEATURE_NAMES[FEATURE_COUNT] = {
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

struct ValueFeat {
  int16_t features[FEATURE_COUNT];
  bool pos_inf = false;
  bool neg_inf = false;
  int mate_depth = 1000000;

  bool found_mate(int depth) {
    if (mate_depth <= depth)
      return true;

    return false;
  }

  ValueFeat(
      std::map<std::string, std::map<std::string, std::pair<int16_t, int16_t>>>
          raw_features) {
    for (unsigned i = 0; i < FEATURE_COUNT; ++i) {
      features[i] =
          raw_features[FEATURE_NAMES[i].first][FEATURE_NAMES[i].second].first -
          raw_features[FEATURE_NAMES[i].first][FEATURE_NAMES[i].second].second;
    }
  }

  ValueFeat() {

  }

  ValueFeat(std::string s) {
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

  ValueFeat operator-(const ValueFeat& x) const {
    ValueFeat minus_x;

    if (x.pos_inf || x.neg_inf) {
      minus_x.pos_inf = x.neg_inf;
      minus_x.neg_inf = x.pos_inf;

      return minus_x;
    }

    for (unsigned i = 0; i < FEATURE_COUNT; ++i) {
      minus_x.features[i] = -x.features[i];
    }

    return minus_x;
  }

  bool operator>(const ValueFeat& x) const;
  bool operator<(const ValueFeat& x) const;

  bool operator==(const ValueFeat& x) const {
    for (unsigned i = 0; i < FEATURE_COUNT; ++i)
      if (features[i] != x.features[i])
        return false;

    return true;
  }

  bool operator!=(const ValueFeat& x) const {
    for (unsigned i = 0; i < FEATURE_COUNT; ++i)
      if (features[i] != x.features[i])
        return true;

    return false;
  }
};

const ValueFeat VALUE_FEAT_POS_INF("+ve infinity");
const ValueFeat VALUE_FEAT_NEG_INF("-ve infinity");

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