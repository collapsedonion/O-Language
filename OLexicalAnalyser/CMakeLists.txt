cmake_minimum_required(VERSION 3.25)
include(../OCompilerBase/CMakeLists.txt)
project(OLexicalAnalyser)

set(CMAKE_CXX_STANDARD 17)

add_library(OLexicalAnalyser STATIC ../OLexicalAnalyser/pch.cpp ../OLexicalAnalyser/Analyser.cpp)
