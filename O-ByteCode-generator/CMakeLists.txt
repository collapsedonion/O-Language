cmake_minimum_required(VERSION 3.25)
project(O_ByteCode_generator)

set(CMAKE_CXX_STANDARD 17)

add_library(O_ByteCode_generator ../O-ByteCode-generator/Geneerator.cpp ../O-ByteCode-generator/Geneerator.h)
