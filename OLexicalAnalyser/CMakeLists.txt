cmake_minimum_required(VERSION 3.25)
include(../OCompilerBase/CMakeLists.txt)
project(OLexicalAnalyser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(OLexicalAnalyser STATIC ../OLexicalAnalyser/pch.cpp ../OLexicalAnalyser/Analyser.cpp)
