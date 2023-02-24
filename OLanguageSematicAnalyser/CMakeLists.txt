cmake_minimum_required(VERSION 3.25)
project(OLanguageSematicAnalyser)

set(CMAKE_CXX_STANDARD 17)

include(../OLexicalAnalyser/CMakeLists.txt)

include_directories(../OCompilerBase)
include_directories(../OLexicalAnalyser)

add_library(OLanguageSematicAnalyser STATIC SematicAnalyser.cpp)

find_library(SyntaxTreeBuilder ../OLexicalAnalyser/cmake-build-debug/libOLexicalAnalyser.a lib)
target_link_libraries(OLanguageSematicAnalyser LINK_PUBLIC SyntaxTreeBuilder)