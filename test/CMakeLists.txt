include_directories(inc)
include_directories(../src/inc)
include_directories(../src/external/Stockfish/src)

set(TEST_SRCS dummy.test.cpp utils.test.cpp)

add_executable(runtests ${TEST_SRCS} main.cpp)

target_link_libraries(runtests pretrainlib stockfish-static syzygy-static)
