cmake_minimum_required(VERSION 3.25)
include(../OLexicalAnalyser/CMakeLists.txt)
project(OLanguageSematicAnalyser)

set(CMAKE_CXX_STANDARD 17)

add_library(OLanguageSematicAnalyser STATIC ../OLanguageSematicAnalyser/SematicAnalyser.cpp)

target_include_directories(OLanguageSematicAnalyser PRIVATE ../OCompilerBase)
target_include_directories(OLanguageSematicAnalyser PRIVATE ../OLexicalAnalyser)

target_link_libraries(OLanguageSematicAnalyser LINK_PUBLIC OLexicalAnalyser)