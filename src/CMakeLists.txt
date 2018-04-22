include_directories(external/Stockfish/src)
include_directories(external/Stockfish/src/syzygy)
include_directories(inc)

add_subdirectory(external/Stockfish)
add_subdirectory(external/dlib)

set(PRE_TRAIN_SRCS gameinfo.cpp utils.cpp)

add_library(pretrainlib STATIC ${PRE_TRAIN_SRCS})

add_executable(pretrain ${PRE_TRAIN_SRCS} main.cpp)

target_link_libraries(pretrain stockfish-static syzygy-static dlib::dlib)
