include_directories(inc)

set(PRE_TRAIN_SRCS gameinfo.cpp)

add_library(pretrainlib STATIC ${PRE_TRAIN_SRCS})

add_executable(pretrain ${PRE_TRAIN_SRCS} main.cpp)
