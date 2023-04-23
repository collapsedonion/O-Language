cmake_minimum_required(VERSION 3.25)
project(OPreProccessor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)

add_library(OPreProccessor ${PROJECT_SOURCE_DIR}/Preproccesor.cpp)
