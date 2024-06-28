cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 17)
project(DFA)

aux_source_directory(./src_DFA SRC_LIST)
include_directories(./include)
add_executable(DFA ${SRC_LIST})
