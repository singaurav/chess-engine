include_directories(inc)
include_directories(../src/inc)

set(TEST_SRCS utils.test.cpp)

add_executable(runtests ${TEST_SRCS} main.cpp)

target_link_libraries(runtests pretrainlib)
