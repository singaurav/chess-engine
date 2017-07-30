include_directories(inc)
include_directories(external/Stockfish/src)
include_directories(external/Stockfish/src/syzygy)

add_subdirectory(external/Stockfish)

set(PRE_TRAIN_SRCS gameinfo.cpp)

add_executable(pretrain ${PRE_TRAIN_SRCS} main.cpp)

target_link_libraries(pretrain stockfish-static syzygy-static)
