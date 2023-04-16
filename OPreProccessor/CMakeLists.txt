cmake_minimum_required(VERSION 3.25)
project(OPreProccessor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(OPreProccessor ../OPreProccessor/Preproccesor.cpp)
