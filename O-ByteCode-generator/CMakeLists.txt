cmake_minimum_required(VERSION 3.25)
project(O_ByteCode_generator)

set(CMAKE_CXX_STANDARD 17)
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)

add_library(O_ByteCode_generator ${PROJECT_SOURCE_DIR}/Geneerator.cpp ${PROJECT_SOURCE_DIR}/Geneerator.hpp)
