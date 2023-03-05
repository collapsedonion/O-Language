cmake_minimum_required(VERSION 3.25)
include(../OVM_SDK/CMakeLists.txt)
project(OVM_module_example)

set(CMAKE_CXX_STANDARD 17)

add_library(OVM_module_example SHARED main.cpp)
