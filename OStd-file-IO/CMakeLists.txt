cmake_minimum_required(VERSION 3.25)
include(../OVM_SDK/CMakeLists.txt)
project(OStd_file_IO)

set(CMAKE_CXX_STANDARD 17)

add_library(OStd_file_IO SHARED library.cpp)
