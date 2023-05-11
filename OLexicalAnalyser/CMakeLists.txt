cmake_minimum_required(VERSION 3.25)
project(OLexicalAnalyser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)

add_subdirectory(${PROJECT_SOURCE_DIR}/../OCompilerBase/ ${PROJECT_SOURCE_DIR}/build/ocb)

add_library(OLexicalAnalyser STATIC ${PROJECT_SOURCE_DIR}/Analyser.cpp ${PROJECT_SOURCE_DIR}/Analyser.h)

target_include_directories(OLexicalAnalyser PRIVATE ${PROJECT_SOURCE_DIR}/../OCompilerBase/)
