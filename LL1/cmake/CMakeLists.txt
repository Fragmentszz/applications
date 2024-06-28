cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)
project(LR)

aux_source_directory(./LR SRC_LIST)
include_directories(./include)
add_executable(LR ${SRC_LIST})
