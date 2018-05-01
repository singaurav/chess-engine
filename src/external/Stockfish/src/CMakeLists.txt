cmake_minimum_required(VERSION 2.8.8)
add_subdirectory(syzygy)

set(STOCKFISH_SRCS
    adapter.cpp
    benchmark.cpp
    bitbase.cpp
    bitboard.cpp
    endgame.cpp
    evaluate.cpp
    featextract.cpp
    init.cpp
    material.cpp
    misc.cpp
    movegen.cpp
    movepick.cpp
    pawns.cpp
    position.cpp
    psqt.cpp
    search.cpp
    thread.cpp
    timeman.cpp
    tt.cpp
    uci.cpp
    ucioption.cpp
)

add_library(stockfish-static STATIC ${STOCKFISH_SRCS})

add_executable(stockfish ${STOCKFISH_SRCS} main.cpp)

target_link_libraries(stockfish syzygy-static)
