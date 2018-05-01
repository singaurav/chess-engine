#include "adapter.h"
#include "featextract.h"
#include "init.h"
#include "position.h"
#include "thread.h"
#include "uci.h"
#include <assert.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

namespace {
StateListPtr States(new std::deque<StateInfo>(1));
}

struct TestCase {
  std::string feat_key;
  std::string sub_feat_key;
  std::string fen;
  Feature *white_sub_feature;
  Feature *black_sub_feature;
  double white_val;
  double black_val;
};

static bool INIT_RUN = false;

template <class F>
void run_test_case(TestCase t) {
  Position pos;

  if (!INIT_RUN) {
    Init::init();
    INIT_RUN = true;
  }

  UCI::newgame();

  States = StateListPtr(new std::deque<StateInfo>(1));
  pos.set(t.fen, false, &States->back(), Threads.main());

  F actual[2], expected[2];
  std::map<std::string, std::pair<FeatureSquareList, FeatureSquareList>>
      feature;

  if (t.feat_key == "space") {
    feature = ExtractFeature::space(pos);
  } else if (t.feat_key == "mobility") {
    feature = ExtractFeature::mobility(pos);
  } else if (t.feat_key == "material") {
    feature = ExtractFeature::material(pos);
  } else if (t.feat_key == "queen") {
    feature = ExtractFeature::queen(pos);
  } else if (t.feat_key == "bishop") {
    feature = ExtractFeature::bishop(pos);
  } else if (t.feat_key == "knight") {
    feature = ExtractFeature::knight(pos);
  } else if (t.feat_key == "rook") {
    feature = ExtractFeature::rook(pos);
  } else if (t.feat_key == "threats") {
    feature = ExtractFeature::threats(pos);
  } else if (t.feat_key == "king") {
    feature = ExtractFeature::king(pos);
  } else if (t.feat_key == "passed-pawns") {
    feature = ExtractFeature::passed_pawns(pos);
  }

  actual[0] = feature[t.sub_feat_key].first;
  actual[1] = feature[t.sub_feat_key].second;

  expected[0] = static_cast<F &>(*t.white_sub_feature);
  expected[1] = static_cast<F &>(*t.black_sub_feature);

  for (int i = 0; i < 2; ++i) {
    std::string side_str = i == 0 ? "WHITE" : "BLACK";
    double expected_value = i == 0 ? t.white_val : t.black_val;
    if (actual[i] != expected[i] || actual[i].value() != expected_value) {
      std::cout << std::endl
                << std::endl
                << " feature: " << t.feat_key << " " << t.sub_feat_key
                << std::endl
                << "     fen: " << t.fen << std::endl
                << "    side: " << side_str << std::endl
                << "  actual: " << actual[i].value() << std::endl
                << actual[i].pretty() << "expected: " << expected_value
                << std::endl
                << expected[i].pretty();
      assert(false);
    }
  }
}

TestCase TEST_CASES[] = {
    // bishop

    {"bishop", "pawns-on-same-color-squares",
     "8/ppb3pp/3p1kp1/8/2nP4/3K3P/PP3PP1/2B1N3 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"b2", "f2", "d4"}),
     new FeatureSquareList(std::vector<std::string>{"a7", "d6", "g7"}), 3, 3},

    {"bishop", "pawns-on-same-color-squares",
     "8/ppb3pp/1n1p1kp1/8/4P3/2BK1B1P/PP3PP1/4N3 w - - 0 1",
     new FeatureSquareList(
         std::vector<std::string>{"a2", "b2", "e4", "f2", "g2", "h3"}),
     new FeatureSquareList(std::vector<std::string>{"a7", "d6", "g7"}), 6, 3},

    {"bishop", "pawns-on-same-color-squares",
     "8/pp4pp/1n1p1kp1/8/1B6/1P1KP1P1/P4P2/4N3 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"e3", "f2", "g3"}),
     new FeatureSquareList(std::vector<std::string>{}), 3, 0},

    {"bishop", "pawns-on-same-color-squares",
     "3q3r/p3k2p/1p1n2p1/8/3R4/P2KP1P1/1P3P2/2Q5 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{}), 0, 0},

    {"bishop", "minor-behind-pawn",
     "rn2kb1r/pp3ppb/1qp1pn1p/3p4/2PN4/1Q1P2P1/PP2PPBP/RNB2RK1 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"g2"}),
     new FeatureSquareList(std::vector<std::string>{"h7", "f8"}), 1, 2},

    {"bishop", "minor-behind-pawn",
     "r2r2k1/1bpq1ppp/1p2p3/p7/3P4/P1Q1P3/1P2BPPP/R2R2K1 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"e2"}),
     new FeatureSquareList(std::vector<std::string>{"b7"}), 1, 1},

    {"bishop", "minor-behind-pawn",
     "2rr2k1/pp2bppp/2b1pn2/2P5/3B4/1N4P1/P3PPBP/R2R2K1 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"g2"}),
     new FeatureSquareList(std::vector<std::string>{"c6", "e7"}), 1, 2},

    {"bishop", "minor-behind-pawn",
     "r2qrnk1/ppp3pp/2p1bp2/P3p3/2NbP2N/3PB2P/1PP2PP1/R2Q1RK1 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"e3"}),
     new FeatureSquareList(std::vector<std::string>{"e6"}), 1, 1},

    {"bishop", "minor-behind-pawn",
     "r3r1k1/p1p2p1p/np3b2/3q2p1/3P4/2P1B1P1/P2N3P/2RQ1RK1 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{}), 0, 0},

    {"bishop", "pawn-unsupported-occupied-outpost",
     "5rk1/pp2p3/3p2pb/2pP4/2q5/3b1B1P/PPn2Q2/R1NK2R1 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"d3"}), 0, 1},

    {"bishop", "pawn-unsupported-occupied-outpost",
     "rq4k1/1b1nbrpp/p2p4/3P1BP1/1p1B1P1P/5Q2/PPP5/2K1R2R w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"d4"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},

    {"bishop", "pawn-unsupported-occupied-outpost",
     "rq4k1/3n1p1p/1p1p4/3b4/1p3B1b/3P3n/PP3Q1P/2K1R3 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"f4"}),
     new FeatureSquareList(std::vector<std::string>{"d5", "h4"}), 1, 2},

    {"bishop", "pawn-supported-occupied-outpost",
     "r5k1/1b2q1pp/p2p4/3PP1P1/1p1B3P/2P2r2/PP2Q3/2K4R w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"d4"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},

    {"bishop", "pawn-supported-occupied-outpost",
     "r2qnrk1/3nbp1p/3p4/5B2/p5P1/1b2P2P/1PNQ4/1K1R1B1R w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"f5"}),
     new FeatureSquareList(std::vector<std::string>{"b3"}), 1, 1},

    {"bishop", "pawn-supported-occupied-outpost",
     "8/5pk1/7p/8/1p4P1/1P1R2P1/3NrqBP/3N3K w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{}), 0, 0},

    {"bishop", "pawn-supported-occupied-outpost",
     "rn4k1/1p2p3/3b1np1/5b1p/1B1pB2N/P2P1P2/2N4P/R3K3 w Q - 0 1",
     new FeatureSquareList(std::vector<std::string>{"b4", "e4"}),
     new FeatureSquareList(std::vector<std::string>{"f5"}), 2, 1},

    {"bishop", "pawn-supported-reachable-outpost",
     "rn4k1/1p2p3/5np1/3B3p/3p3N/P2P1P2/2N4P/R3K3 w Q - 0 1",
     new FeatureSquareList(std::vector<std::string>{"d5"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},

    {"bishop", "pawn-supported-reachable-outpost",
     "r4k2/1p1n1p1b/3pp3/7p/8/P2P4/2N2P1P/R3KB2 w Q - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"h7"}), 0, 1},

    {"bishop", "pawn-supported-reachable-outpost",
     "5rk1/1ppp1p1p/1n4p1/4b3/b7/6P1/PPP4P/1KR1NBB1 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{}), 0, 0},

    {"bishop", "pawn-unsupported-reachable-outpost",
     "5rk1/1p1p1p1p/1n4p1/4b3/b7/8/PPPP3P/1KR1NBB1 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"f1", "g1"}),
     new FeatureSquareList(std::vector<std::string>{"e5"}), 2, 1},

    {"bishop", "pawn-unsupported-reachable-outpost",
     "6k1/3ppppb/7b/8/8/2K2BB1/4PPPP/8 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{}), 0, 0},

    {"bishop", "pawn-unsupported-reachable-outpost",
     "BB4k1/3pppp1/7b/8/8/5P2/5PPP/1b5K w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"b1"}), 0, 1},

    // knight

    {"knight", "minor-behind-pawn",
     "r1bqkbnr/pp3ppp/3n1P2/3p1N2/3P4/3N4/PPP1B1PP/R2Q1RK1 w kq - 0 1",
     new FeatureSquareList(std::vector<std::string>{"d3"}),
     new FeatureSquareList(std::vector<std::string>{"d6", "g8"}), 1, 2},

    {"knight", "minor-behind-pawn",
     "r2rb1k1/1p2qn1p/p1p1pnp1/3pNp2/1PPP1P2/3BPN2/P4QPP/1RR3K1 b - - 1 17",
     new FeatureSquareList(std::vector<std::string>{"f3"}),
     new FeatureSquareList(std::vector<std::string>{"f6"}), 1, 1},

    {"knight", "minor-behind-pawn",
     "r2q1rk1/ppp1ppbp/2np1np1/8/3PP1b1/2N1BN2/PPP1BPPP/R2Q1RK1 w - - 10 8",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{}), 0, 0},

    {"knight", "minor-behind-pawn",
     "8/2p3kp/2Bp2p1/3P1n2/1P3p2/r1P5/2R3PP/6K1 w - - 2 29",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"f5"}), 0, 1},

    {"knight", "pawn-supported-occupied-outpost",
     "8/8/8/1pN5/1PkP4/P3K3/8/7b b - - 8 62",
     new FeatureSquareList(std::vector<std::string>{"c5"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},

    {"knight", "pawn-supported-occupied-outpost",
     "r4rk1/2R1pp1p/2p3p1/2PnB3/N3R3/pP5P/Pb3PP1/6K1 b - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"a4"}),
     new FeatureSquareList(std::vector<std::string>{"d5"}), 1, 1},

    {"knight", "pawn-unsupported-occupied-outpost",
     "r4rk1/pp3pNp/3R2p1/1B2n3/8/1q2P1QP/1P4P1/5RK1 b - - 0 25",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"e5"}), 0, 1},

    {"knight", "pawn-unsupported-occupied-outpost",
     "6k1/pr4pp/2N5/4p3/5n1P/5K2/P4R2/8 b - - 1 32",
     new FeatureSquareList(std::vector<std::string>{"c6"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},

    {"knight", "pawn-supported-reachable-outpost",
     "6k1/3pbp2/4n1p1/1PNrp3/2R5/6PP/2B2P1K/2B5 b - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"c5"}),
     new FeatureSquareList(std::vector<std::string>{"e6"}), 1, 1},

    {"knight", "pawn-supported-reachable-outpost",
     "k7/1nn5/1pp5/8/8/6PP/5NNK/8 b - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"f2", "g2"}),
     new FeatureSquareList(std::vector<std::string>{"b7", "c7"}), 2, 2},

    {"knight", "pawn-unsupported-reachable-outpost",
     "k7/1nn5/8/8/8/8/5NNK/8 b - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"f2", "g2"}),
     new FeatureSquareList(std::vector<std::string>{"b7", "c7"}), 2, 2},

    {"knight", "pawn-unsupported-reachable-outpost",
     "k7/6N1/8/8/8/8/1n6/7K b - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"g7"}),
     new FeatureSquareList(std::vector<std::string>{"b2"}), 1, 1},

    // material

    {"material", "pawn",
     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
     new FeatureSquareList(std::vector<std::string>{"a2", "b2", "c2", "d2",
                                                    "e2", "f2", "g2", "h2"}),
     new FeatureSquareList(std::vector<std::string>{"a7", "b7", "c7", "d7",
                                                    "e7", "f7", "g7", "h7"}),
     8, 8},

    {"material", "pawn", "7K/5P2/7k/8/5r2/8/2R5/8 b - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"f7"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},

    {"material", "pawn", "7K/4k3/8/8/8/8/8/8 w - - 0 7",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{}), 0, 0},

    {"material", "pawn", "4k3/p4ppp/3p4/1K6/8/4b2P/PP2N1P1/8 w - - 0 2",
     new FeatureSquareList(std::vector<std::string>{"a2", "b2", "g2", "h3"}),
     new FeatureSquareList(
         std::vector<std::string>{"a7", "d6", "f7", "g7", "h7"}),
     4, 5},

    {"material", "knight", "6k1/8/5P1b/r3p3/5N2/p7/1P2P3/4K2R w K - 0 1",
     new FeatureSquareList(std::vector<std::string>{"f4"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},

    {"material", "knight",
     "rnbqk2r/ppp1bppp/8/3pp3/3PB3/5N2/PPP2PPP/RNBQK2R w KQkq d6 0 6",
     new FeatureSquareList(std::vector<std::string>{"b1", "f3"}),
     new FeatureSquareList(std::vector<std::string>{"b8"}), 2, 1},

    {"material", "knight", "6k1/8/5P1R/r7/5p2/p7/1P2P3/4K3 w - - 0 2",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{}), 0, 0},

    {"material", "bishop", "6k1/8/5P1b/r3p3/5N2/p7/1P2P3/4K2R w K - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"h6"}), 0, 1},

    {"material", "bishop",
     "rnbqk1nr/pppp1ppp/4p3/8/1P2P3/8/PP1P1PPP/RNBQKBNR b KQkq - 0 3",
     new FeatureSquareList(std::vector<std::string>{"c1", "f1"}),
     new FeatureSquareList(std::vector<std::string>{"c8"}), 2, 1},

    {"material", "rook", "8/8/8/3N4/2p2Kp1/3b2P1/8/4k3 b - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{}), 0, 0},

    {"material", "rook",
     "rn2kbnr/ppp1pppp/3q4/5b2/8/2N2N2/PPPP1PPP/R1BQKB1R w KQkq - 4 5",
     new FeatureSquareList(std::vector<std::string>{"a1", "h1"}),
     new FeatureSquareList(std::vector<std::string>{"a8", "h8"}), 2, 2},

    {"material", "rook",
     "r2qkb1r/ppp2ppp/2n2n2/3pp2P/6P1/3P4/PP1PPP2/RNBQKBN1 w Qkq e6 0 8",
     new FeatureSquareList(std::vector<std::string>{"a1"}),
     new FeatureSquareList(std::vector<std::string>{"a8", "h8"}), 1, 2},

    {"material", "queen", "1b5q/5Pp1/8/1BN4k/8/7P/6K1/8 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"h8"}), 0, 1},

    {"material", "queen",
     "r1bqkb1r/pp1ppppp/2n5/2P5/4P1n1/8/PPP2PPP/RNB1KBNR w KQkq - 0 5",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"d8"}), 0, 1},

    {"material", "queen",
     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
     new FeatureSquareList(std::vector<std::string>{"d1"}),
     new FeatureSquareList(std::vector<std::string>{"d8"}), 1, 1},

    // mobility

    {"mobility", "all",
     "r1b1k1nr/ppppqppp/2n5/b7/2B1P3/1QN2N2/P4PPP/R1B2RK1 b kq - 0 9",
     new FeatureSquareList(std::vector<std::string>{
         "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "a7", "b7", "c7",
         "d7", "e7", "f7", "g7", "h7", "a5", "b5", "c5", "d5", "e5", "f5",
         "g5", "h5", "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "a3",
         "b3", "c3", "d3", "e3", "f3", "g3", "h3", "b2", "c2", "d2", "e2",
         "a1", "b1", "c1", "d1", "e1", "f1", "h1"}),
     new FeatureSquareList(std::vector<std::string>{
         "a8", "b8", "c8", "d8", "f8", "g8", "h8", "e7", "a6", "b6",
         "c6", "d6", "e6", "f6", "g6", "h6", "a5", "b5", "c5", "e5",
         "g5", "h5", "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
         "a3", "c3", "d3", "a2", "b2", "c2", "d2", "e2", "f2", "g2",
         "h2", "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"}),
     51, 49},

    {"mobility", "all", "5rk1/1b3p1p/ppq3p1/2p5/8/1P1P1R1Q/PBP3PP/7K b - - 0 1",
     new FeatureSquareList(std::vector<std::string>{
         "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "a7", "b7",
         "c7", "d7", "e7", "f7", "g7", "h7", "a6", "b6", "c6", "d6",
         "f6", "h6", "d5", "e5", "g5", "a4", "c4", "e4", "f4", "g4",
         "h4", "a3", "c3", "e3", "f3", "g3", "h3", "b2", "d2", "e2",
         "f2", "a1", "b1", "c1", "d1", "e1", "f1", "g1"}),
     new FeatureSquareList(std::vector<std::string>{
         "a8", "b8", "c8", "d8", "e8", "f8", "h8", "a7", "b7", "c7",
         "d7", "e7", "g7", "c6", "d6", "e6", "f6", "h6", "a5", "b5",
         "c5", "d5", "e5", "f5", "g5", "h5", "b4", "d4", "f4", "g4",
         "h4", "a3", "c3", "e3", "a2", "b2", "c2", "d2", "e2", "f2",
         "g2", "h2", "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"}),
     48, 50},

    {"mobility", "bishop", "8/1p3k2/8/1B6/2K3b1/3P4/8/8 b - - 1 1",
     new FeatureSquareList(std::vector<std::string>{"e8", "d7", "a4"}),
     new FeatureSquareList(std::vector<std::string>{
         "c8", "d7", "e6", "f5", "h3", "h5", "f3", "e2", "d1"}),
     3, 9},

    {"mobility", "bishop", "8/3r1nk1/8/2PB4/3K4/4P3/8/8 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{}), 0, 0},

    {"mobility", "bishop", "8/1pp2k2/p7/1N6/B5b1/1KRP4/8/8 w - - 0 2",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{
         "d1", "e2", "f3", "h5", "c8", "d7", "e6", "f5", "h3"}),
     0, 9},

    // bishop can attack friendly knight, bishop and rook for mobility
    {"mobility", "bishop", "4r3/ppp2k2/8/1N6/B5b1/1KRP4/8/8 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"b5"}),
     new FeatureSquareList(std::vector<std::string>{
         "d1", "e2", "f3", "h5", "c8", "d7", "e6", "f5", "h3"}),
     1, 9},

    {"mobility", "bishop", "4r3/p1pn4/1p5k/8/B5b1/1KRP4/8/8 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"b5", "c6", "d7"}),
     new FeatureSquareList(std::vector<std::string>{"d7", "e6", "f5", "h3",
                                                    "h5", "f3", "e2", "d1"}),
     3, 8},

    // bishop can look through friendly queen because they form battery
    {"mobility", "bishop", "4r3/pqp5/1p5k/1Q6/B5b1/1KRP4/8/8 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"b5", "c6", "d7", "e8"}),
     new FeatureSquareList(std::vector<std::string>{
         "c8", "d7", "e6", "f5", "h3", "h5", "f3", "e2", "d1"}),
     4, 9},

    {"mobility", "knight", "4k3/p1p5/1p3n2/8/NN6/2P2P2/1K6/8 w - - 0 1",
     new FeatureSquareList(
         std::vector<std::string>{"a6", "c6", "d5", "d3", "c2", "a2"}),
     new FeatureSquareList(
         std::vector<std::string>{"h5", "d5", "d7", "g8", "h7"}),
     6, 5},

    {"mobility", "knight", "8/pp3k1n/2p5/6n1/B7/1KNP4/8/8 w - - 0 1",
     new FeatureSquareList(
         std::vector<std::string>{"a4", "e4", "e2", "a2", "b1", "d1"}),
     new FeatureSquareList(
         std::vector<std::string>{"f8", "f6", "g5", "h7", "e6", "f3", "h3"}),
     6, 7},

    {"mobility", "queen", "8/1R1q2k1/1P6/QK6/1P4b1/8/7p/8 b - - 0 1",
     new FeatureSquareList(
         std::vector<std::string>{"a1", "a2", "a3", "a4", "a6", "a7", "a8"}),
     new FeatureSquareList(std::vector<std::string>{"b7", "e7", "f7"}), 7, 3},

    // queen cannot look through friendly bishop or rook
    {"mobility", "queen", "2n5/pp2qk2/2p5/4P3/4N3/1K1B4/4Q3/8 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{
         "e3", "d3", "f3", "e4", "g4", "h5", "a2", "b2", "c2", "d2", "f2", "g2",
         "h2", "d1", "e1", "f1"}),
     new FeatureSquareList(std::vector<std::string>{"d8", "e8", "f8", "g5",
                                                    "c7", "d7", "e6", "e5",
                                                    "b4", "c5", "a3", "h4"}),
     16, 12},

    // h5 pawn is blocked
    {"mobility", "rook", "2k2n2/8/3B2pK/p4r1p/1p5P/5PP1/PR6/8 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"b3", "c2", "d2", "e2",
                                                    "f2", "g2", "h2", "b1"}),
     new FeatureSquareList(std::vector<std::string>{
         "f6", "f7", "f8", "a5", "b5", "c5", "d5", "e5", "f3"}),
     8, 9},

    // rook can look through friendly rook and queen because they form battery
    {"mobility", "rook", "8/pp3k2/2prn3/3r4/5PP1/1K4P1/4Q3/4R3 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"e2", "e3", "e4", "e5",
                                                    "e6", "a1", "b1", "c1",
                                                    "d1", "f1", "g1", "h1"}),
     new FeatureSquareList(std::vector<std::string>{
         "d7", "d8", "d5", "d4", "d3", "d2", "d1", "d1", "d2", "d3", "d4", "d6",
         "d7", "d8", "a5", "b5", "c5", "e6"}),
     12, 18},

    // queen

    {"queen", "weak", "8/pp3k2/2prn3/1b1r4/6P1/3R1PP1/2K1Q3/8 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"e2"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},

    {"queen", "weak", "8/pp1r1k2/2p1q3/2bn1r2/8/6PQ/R1K2P1P/8 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{}), 0, 0},

    {"queen", "weak", "8/pp1r1k2/2p5/1qb2r2/2n5/6P1/R1K2P1P/3Q1B2 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"b5"}), 0, 1},

    {"queen", "weak", "8/pp1r1k2/2p5/1qb2r2/8/3N2P1/R1K2P1P/3Q1B2 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"d1"}),
     new FeatureSquareList(std::vector<std::string>{"b5"}), 1, 1},

    {"queen", "weak", "8/pp1r1k2/2p5/1qb3r1/3P4/4Q1P1/R1KB3P/8 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"e3"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},

    {"queen", "weak", "8/pp3k2/3r4/1q3r2/5p1b/3n4/R1KB1Q1P/4P1P1 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"f2"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},

    // rook

    {"rook", "rook-on-pawn",
     "6k1/pp6/2p2R2/6p1/6b1/1P1P2P1/P3pN1P/6K1 b - - 1 35",
     new FeatureSquareList(std::vector<std::string>{"c6"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},

    {"rook", "rook-on-pawn", "8/7k/7p/3p2p1/3N4/2p4P/Pr4RK/8 w - g6 0 47",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"a2"}), 0, 1},

    {"rook", "rook-on-pawn",
     "1k5r/p1p1p2p/1b1nR1p1/8/1rPB4/1P3BP1/5N1P/1R4K1 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"e7", "g6"}),
     new FeatureSquareList(std::vector<std::string>{"b3", "c4"}), 2, 2},

    {"rook", "rook-on-pawn",
     "6k1/pp6/5q1p/4p1pR/6Q1/rP1PpbP1/P6P/3N2K1 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"e5", "g5", "h6"}),
     new FeatureSquareList(std::vector<std::string>{"a2", "b3", "d3", "g3"}), 3,
     4},

    {"rook", "rook-on-pawn", "6k1/RR1ppp2/8/1p6/p6r/7r/7P/6KB w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"a4", "b5", "d7", "e7",
                                                    "f7", "d7", "e7", "f7"}),
     new FeatureSquareList(std::vector<std::string>{"h2", "h2"}), 8, 2},

    {"rook", "rook-on-open-file",
     "2rr2k1/pb4pp/1p2pp2/3n4/8/P3P1B1/1PR1BPPP/5RK1 w - - 4 20",
     new FeatureSquareList(std::vector<std::string>{"c2"}),
     new FeatureSquareList(std::vector<std::string>{"c8", "d8"}), 1, 2},

    {"rook", "rook-on-open-file",
     "1k4r1/2p5/1n1r4/6N1/3p4/1P2P1R1/P1B5/1K6 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"g3"}),
     new FeatureSquareList(std::vector<std::string>{"g8"}), 1, 1},

    {"rook", "rook-on-open-file",
     "1k6/8/2rb1r2/np4p1/3R1n2/1PN1P3/2B3P1/1K4R1 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"d4"}),
     new FeatureSquareList(std::vector<std::string>{"c6", "f6"}), 1, 2},

    {"rook", "rook-on-open-file",
     "1kr5/1p2pqr1/4b3/6n1/3N2p1/PR2B3/2P1Q3/1K6 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{}), 0, 0},

    {"rook", "rook-on-open-file",
     "1k6/p1p2n2/1b1r1p2/7n/3R4/3RPBP1/1PN5/1K6 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"d3", "d4"}),
     new FeatureSquareList(std::vector<std::string>{"d6"}), 2, 1},

    {"rook", "rook-on-semi-open-file",
     "1kr5/pp6/1n3R2/7r/2Nb1p2/1P2nP1B/P1P3P1/1K2R3 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"c8"}), 0, 1},

    {"rook", "rook-on-semi-open-file",
     "1k6/1p2r3/3b3p/2p2n2/8/1N2P1B1/B1R2P2/2R3K1 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"c1", "c2"}),
     new FeatureSquareList(std::vector<std::string>{"e7"}), 2, 1},

    {"rook", "rook-on-semi-open-file",
     "1k6/p1p5/1b3pr1/3p4/2Pn4/R2P2N1/5B2/3R2K1 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"a3"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},

    // TODO Logic for trapped rook + castle not clear

    // space

    {"space", "safe-squares",
     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
     new FeatureSquareList(std::vector<std::string>{"c3", "d3", "e3", "f3",
                                                    "c4", "d4", "e4", "f4"}),
     new FeatureSquareList(std::vector<std::string>{"c5", "d5", "e5", "f5",
                                                    "c6", "d6", "e6", "f6"}),
     8, 8},
    {"space", "extra-safe-squares",
     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{}), 0, 0},
    {"space", "safe-squares",
     "r1bqkbnr/pppp2pp/2n5/1B2pp2/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq f6 0 4",
     new FeatureSquareList(
         std::vector<std::string>{"c4", "c3", "d3", "e3", "f3", "e2"}),
     new FeatureSquareList(
         std::vector<std::string>{"c5", "c6", "d6", "e6", "f6", "e7", "f7"}),
     6, 7},
    {"space", "extra-safe-squares",
     "r1bqkbnr/pppp2pp/2n5/1B2pp2/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq f6 0 4",
     new FeatureSquareList(std::vector<std::string>{"e3", "e2"}),
     new FeatureSquareList(std::vector<std::string>{"e6", "f6", "e7", "f7"}), 2,
     4},
    {"space", "safe-squares",
     "rnbqkb1r/pp3ppp/2p1pn2/8/4P3/5NP1/PPPN1P1P/R1BQKB1R b KQkq - 0 6",
     new FeatureSquareList(std::vector<std::string>{
         "c4", "d4", "f4", "c3", "d3", "e3", "f3", "d2", "e2"}),
     new FeatureSquareList(
         std::vector<std::string>{"c5", "d6", "f6", "c7", "d7", "e7"}),
     9, 6},
    {"space", "extra-safe-squares",
     "rnbqkb1r/pp3ppp/2p1pn2/8/4P3/5NP1/PPPN1P1P/R1BQKB1R b KQkq - 0 6",
     new FeatureSquareList(std::vector<std::string>{"e3", "e2"}),
     new FeatureSquareList(std::vector<std::string>{"c7", "e7"}), 2, 2},
    {"space", "safe-squares",
     "rnbq1rk1/ppp1b1pp/8/3n1p2/8/3P1NN1/PPP2PPP/R1BQKB1R b KQ - 0 9",
     new FeatureSquareList(std::vector<std::string>{"c4", "d4", "f4", "c3",
                                                    "e3", "f3", "d2", "e2"}),
     new FeatureSquareList(std::vector<std::string>{
         "c5", "d5", "c6", "d6", "e6", "f6", "d7", "e7", "f7"}),
     8, 9},
    {"space", "extra-safe-squares",
     "rnbq1rk1/ppp1b1pp/8/3n1p2/8/3P1NN1/PPP2PPP/R1BQKB1R b KQ - 0 9",
     new FeatureSquareList(std::vector<std::string>{"d2"}),
     new FeatureSquareList(std::vector<std::string>{"f6", "f7"}), 1, 2},

    // threats

    {"threats", "safe-pawn",
     "r2q1rk1/pb1pbppp/1pn1pn2/2pP2B1/2P5/2NBPN2/PP3PPP/R2Q1RK1 b - - 0 9",
     new FeatureSquareList(std::vector<std::string>{"c6"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},
    {"threats", "safe-pawn",
     "4r3/8/5p2/p1p2kp1/2n2N2/PR2P3/1P2KP1r/6R1 w - g6 0 35",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"f4"}), 0, 1},
    {"threats", "safe-pawn",
     "r3kbnr/pppp1p1p/8/1bn1p1q1/P2P2p1/5B1P/1PPNPPP1/R1BQK1NR b KQkq - 1 1",
     new FeatureSquareList(std::vector<std::string>{"b5"}),
     new FeatureSquareList(std::vector<std::string>{"f3"}), 1, 1},
    {"threats", "safe-pawn",
     "r1r3k1/3b1ppp/p2ppn2/1p6/1q1pPNb1/1nP1N2P/PPB2PP1/R1BQR1K1 w - - 0 15",
     new FeatureSquareList(std::vector<std::string>{"b3", "b4", "g4"}),
     new FeatureSquareList(std::vector<std::string>{"e3"}), 3, 1},
    {"threats", "hanging-pawn",
     "r1bqkb1r/pppppp1p/2n2np1/3P4/8/8/PPPBPPPP/RN1QKBNR b KQkq - 1 1",
     new FeatureSquareList(std::vector<std::string>{"c6"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},
    {"threats", "hanging-pawn",
     "4k3/pppp2pp/5pr1/5P2/nb1n4/1PP1p1P1/P4N1P/1K2R3 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"b4", "d4", "g6"}),
     new FeatureSquareList(std::vector<std::string>{"f2"}), 3, 1},
    {"threats", "hanging-pawn",
     "r1b2nn1/1pp1qk1p/6p1/p2Ppp2/2PQ4/PP1N4/2B2PPP/2R2RK1 w - - 0 25",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{}), 0, 0},
    {"threats", "pawn-push",
     "r4rk1/pp2ppbp/2n1bnp1/q1B5/2P5/2N2NP1/PP2PPBP/R2Q1RK1 w - - 1 11",
     new FeatureSquareList(std::vector<std::string>{"a5"}),
     new FeatureSquareList(std::vector<std::string>{"c5"}), 1, 1},
    {"threats", "pawn-push",
     "4r1k1/p1r1ppb1/7p/q2n4/1pbR4/B5P1/PP1QPPBP/1N3RK1 w - - 2 21",
     new FeatureSquareList(std::vector<std::string>{"c4", "d5"}),
     new FeatureSquareList(std::vector<std::string>{"d4"}), 2, 1},
    {"threats", "pawn-push",
     "r1bq1rk1/pppn1pp1/3b3p/2P5/3PN3/3BpN2/PP4PP/R2Q1RK1 b - - 0 12",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"e4"}), 0, 1},
    {"threats", "pawn-threat-by-minor",
     "r1bqkb1r/pp3ppp/2nppn2/6B1/3NP3/2N5/PPP2PPP/R2QKB1R w KQkq - 0 7",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"e4"}), 0, 1},
    {"threats", "pawn-threat-by-minor",
     "r1bqkb1r/pp1n2pp/2n1pp2/3pP3/3P1N2/3B4/PP1N1PPP/R1BQK2R b KQkq - 1 9",
     new FeatureSquareList(std::vector<std::string>{"e6", "h7"}),
     new FeatureSquareList(std::vector<std::string>{"d4"}), 2, 1},
    {"threats", "minor-threat-by-minor",
     "r1bqkb1r/pp3ppp/2nppn2/6B1/3NP3/2N5/PPP2PPP/R2QKB1R w KQkq - 0 7",
     new FeatureSquareList(std::vector<std::string>{"c6", "f6"}),
     new FeatureSquareList(std::vector<std::string>{"d4"}), 2, 1},
    {"threats", "rook-threat-by-minor",
     "1b2r1k1/1b1q1ppp/1pr5/3pN3/1B1Pn3/4P3/1Q2BPPP/1RR3K1 b - - 3 24",
     new FeatureSquareList(std::vector<std::string>{"c6"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},
    {"threats", "queen-threat-by-minor",
     "1b2r1k1/1b1q1ppp/1pr5/3pN3/1B1Pn3/4P3/1Q2BPPP/1RR3K1 b - - 3 24",
     new FeatureSquareList(std::vector<std::string>{"d7"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},
    {"threats", "king-threat-by-minor",
     "1b2r1k1/1b1q1ppp/1pr5/3pN3/1B1Pn3/4P3/1Q1KBPPP/1RR5 w - - 4 25",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"d2"}), 0, 1},
    {"threats", "pawn-threat-by-rook",
     "1bbr2k1/3q1ppp/pPr2n2/3p4/8/2P1PNP1/4BP1B/R4KQR b - - 2 27",
     new FeatureSquareList(std::vector<std::string>{"a6"}),
     new FeatureSquareList(std::vector<std::string>{"b6", "c3"}), 1, 2},
    {"threats", "queen-threat-by-rook",
     "r2q1rk1/1bp2ppp/1p1b1n2/1P1p4/3P1N2/2B1P3/Q3BPPP/1R3RK1 b - - 0 17",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"a2"}), 0, 1},
    {"threats", "king-threat-by-rook",
     "r2q1rk1/1bp2ppp/1p1b1n2/1P1p4/3P1N2/2B1P3/K2QBPPP/1R3R2 b - - 0 17",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"a2"}), 0, 1},
    {"threats", "threat-by-minor-rank",
     "1r1q1rk1/1b3ppp/1ppb1n2/1P1p4/3P1N2/2B1P3/4BPPP/2RQ1RK1 w - - 3 19",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"a1", "a1", "a1"}), 0, 3},
    {"threats", "threat-by-minor-rank",
     "1r1q1rk1/1b3ppp/1ppb1n2/1P1p4/3P4/2B1P1N1/4BPPP/2RQ1RK1 w - - 3 19",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"a1", "a1"}), 0, 2},
    {"threats", "threat-by-rook-rank",
     "1r1q1rk1/1b1b1p1p/1pp2np1/1P1p4/3P1R2/2B1PP1P/4B1PN/2RQ2K1 w - - 0 20",
     new FeatureSquareList(std::vector<std::string>{"a1", "a1"}),
     new FeatureSquareList(std::vector<std::string>{}), 2, 0},
    {"threats", "hanging",
     "rnbqkb1r/pppp1ppp/4p3/2R5/3n4/4P3/PPPP1PPP/1NBQKBNR w KQkq - 0 2",
     new FeatureSquareList(std::vector<std::string>{"d4"}),
     new FeatureSquareList(std::vector<std::string>{"c5"}), 1, 1},
    {"threats", "threat-by-king", "8/4P3/8/6k1/6B1/Bp1n4/3K4/3b4 b - - 2 52",
     new FeatureSquareList(std::vector<std::string>{"a1", "a1"}),
     new FeatureSquareList(std::vector<std::string>{"a1"}), 2, 1},

    // king

    {"king", "castle-king-side",
     "rbq1rk1/p2nbppp/1pQ2n2/2pp2B1/3P4/2N1PN2/PP3PPP/2R1KB1R w K - 2 11",
     new FeatureSquareList(std::vector<std::string>{"a1"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},
    {"king", "castle-queen-side",
     "r2qkb1r/pppb1ppp/2np1n2/1B2p3/3PP3/2N2N2/PPP2PPP/R1BQ1RK1 b kq - 5 6",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"a1"}), 0, 1},
    {"king", "min-king-pawn-distance",
     "K6k/8/4pp2/3p2p1/3Pn1P1/4R2P/r7/6N1 b - - 1 58",
     new FeatureSquareList(std::vector<std::string>{"a1", "a1", "a1", "a1"}),
     new FeatureSquareList(std::vector<std::string>{"a1", "a1"}), 4, 2},
    // attackers on white king: rook on a2 and knight on e4
    // attackers on black king: pawn on g4 but counted twice because for pawns
    // attack squares are counted instead of the pawns themselves
    {"king", "king-attackers-count",
     "8/8/4ppk1/3p2p1/3Pn1P1/4R2P/r7/5KN1 b - - 1 58",
     new FeatureSquareList(std::vector<std::string>{"a1", "a1"}),
     new FeatureSquareList(std::vector<std::string>{"a1", "a1"}), 2, 2},
    {"king", "king-attackers-count", "8/p7/8/2pk4/7B/PPK3p1/8/8 b - - 1 50",
     new FeatureSquareList(std::vector<std::string>{"a1", "a1"}),
     new FeatureSquareList(std::vector<std::string>{"a1"}), 2, 1},
    {"king", "king-adj-zone-attacks-count",
     "r6k/pp3Pr1/7p/3P1pp1/3N4/PP5P/1B5K/4Rq2 w - - 0 33",
     new FeatureSquareList(std::vector<std::string>{"a1", "a1", "a1", "a1"}),
     new FeatureSquareList(std::vector<std::string>{}), 4, 0},
    {"king", "king-only-defended", "3bkr2/8/8/8/1B6/8/4KN2/8 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"f2", "f3"}),
     new FeatureSquareList(std::vector<std::string>{"f8"}), 2, 1},
    {"king", "not-defended-larger-king-ring",
     "4k3/8/8/8/1B5b/7r/5N2/4K3 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"d3", "e3", "f3"}),
     new FeatureSquareList(std::vector<std::string>{"d6"}), 3, 1},
    {"king", "enemy-safe-queen-check", "7q/8/6rk/8/1B5b/8/5N2/4KQ2 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"e8", "e5", "a1"}),
     new FeatureSquareList(std::vector<std::string>{}), 3, 0},
    {"king", "enemy-safe-rook-check", "7q/8/6rk/8/1B5b/8/5N2/4KQ2 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"e6"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},
    {"king", "enemy-other-rook-check",
     "4b2q/8/4p1r1/kB4p1/6P1/P5R1/5N2/4KQ2 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"e6"}),
     new FeatureSquareList(std::vector<std::string>{"a3"}), 1, 1},
    {"king", "enemy-safe-bishop-check", "4k3/8/1b6/p7/7b/1B6/3K4/8 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"g5"}),
     new FeatureSquareList(std::vector<std::string>{"a4"}), 1, 1},
    {"king", "enemy-other-bishop-check", "4k3/8/1b6/p7/7b/1B6/3K4/8 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"a5", "e3", "g5", "e1"}),
     new FeatureSquareList(std::vector<std::string>{"a4", "f7"}), 4, 2},
    {"king", "enemy-safe-knight-check",
     "4kb2/8/8/1N2n3/8/3p1p2/5P2/4K3 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"c7"}), 0, 1},
    {"king", "enemy-other-knight-check",
     "4kb2/8/8/1N2n3/8/3p1p2/5P2/4K3 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"d3"}),
     new FeatureSquareList(std::vector<std::string>{"c7", "d6"}), 1, 2},
    {"king", "close-enemies-one",
     "8/1R3p1p/4pk2/8/PP3pn1/8/5PPP/2r2BK1 b - - 2 34",
     new FeatureSquareList(std::vector<std::string>{
         "e5", "f5", "g5", "e3", "g3", "f2", "h2", "e1", "f1"}),
     new FeatureSquareList(std::vector<std::string>{"e7", "f7"}), 9, 2},
    {"king", "close-enemies-two",
     "8/1R3p1p/4pk2/8/PP3pn1/8/5PPP/2r2BK1 b - - 2 34",
     new FeatureSquareList(std::vector<std::string>{"e5", "f5"}),
     new FeatureSquareList(std::vector<std::string>{}), 2, 0},
    {"king", "pawnless-flank", "8/1R6/5k2/8/PP4n1/8/8/K1r2B2 b - - 2 34",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"a1"}), 0, 1},

    // passed pawns

    {"passed-pawns", "hindered-passed-pawn",
     "3k4/1p4R1/1Pb5/3pP3/1N1P2K1/p2r4/5P2/8 b - - 0 39",
     new FeatureSquareList(std::vector<std::string>{"f3", "e7", "e8"}),
     new FeatureSquareList(std::vector<std::string>{"a2"}), 3, 1},
    {"passed-pawns", "blocksq-their-king-distance",
     "4k3/8/8/8/1P6/4p3/8/4K3 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"a1", "a1", "a1"}),
     new FeatureSquareList(std::vector<std::string>{"a1"}), 3, 1},
    {"passed-pawns", "blocksq-our-king-distance",
     "4k3/8/8/8/1P6/4p3/8/4K3 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"a1", "a1", "a1", "a1"}),
     new FeatureSquareList(
         std::vector<std::string>{"a1", "a1", "a1", "a1", "a1", "a1"}),
     4, 6},
    {"passed-pawns", "two-blocksq-our-king-distance",
     "4k3/8/8/8/1P6/4p3/8/4K3 w - - 0 1",
     new FeatureSquareList(
         std::vector<std::string>{"a1", "a1", "a1", "a1", "a1"}),
     new FeatureSquareList(
         std::vector<std::string>{"a1", "a1", "a1", "a1", "a1", "a1", "a1"}),
     5, 7},
    {"passed-pawns", "empty-blocksq",
     "8/q5pk/8/2P5/4pp2/1R6/P6p/1KR4N b - - 0 52",
     new FeatureSquareList(std::vector<std::string>{"c6"}),
     new FeatureSquareList(std::vector<std::string>{"e3", "f3"}), 1, 2},
    {"passed-pawns", "no-unsafe-squares",
     "5k2/4n3/8/1PP5/8/5p2/5P2/1K6 b - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"b5"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},
    {"passed-pawns", "no-unsafe-blocksq",
     "5k2/8/8/1PPn4/8/5p2/5P2/1K6 b - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"c5"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},
    {"passed-pawns", "fully-defended-path",
     "5k2/8/1PP5/8/8/5p2/6q1/1K6 b - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"f3"}), 0, 1},
    {"passed-pawns", "defended-block-square",
     "5k2/8/8/1P6/8/5pq1/8/1K6 b - - 0 1",
     new FeatureSquareList(std::vector<std::string>{}),
     new FeatureSquareList(std::vector<std::string>{"f3"}), 0, 1},
    {"passed-pawns", "friendly-occupied-blocksq",
     "5k2/8/1R6/1P6/8/5pq1/8/1K6 b - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"b5"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},
    {"passed-pawns", "average-candidate-passers",
     "4k3/8/2p5/p7/PP6/2P5/8/4K3 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"b4"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0},
    {"passed-pawns", "average-candidate-passers",
     "4k3/8/1p6/1P6/P7/8/8/4K3 w - - 0 1",
     new FeatureSquareList(std::vector<std::string>{"b5"}),
     new FeatureSquareList(std::vector<std::string>{}), 1, 0}};

int main() {
  std::cout << std::endl
            << "////////////////////////" << std::endl
            << "// Testing features ..." << std::endl
            << "//////////////////////" << std::endl
            << std::endl;

  int count = 1;
  for (TestCase t : TEST_CASES) {
    std::cout << std::setw(5) << count << "." << std::flush;

    run_test_case<FeatureSquareList>(t);

    std::cout << " "
              << "OK |";

    if (count++ % 5 == 0) {
      std::cout << std::endl;
    }
  }
  std::cout << std::endl << std::endl;
  return 0;
}
