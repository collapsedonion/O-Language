cmake_minimum_required(VERSION 3.25)
project(OLanguageSematicAnalyser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)

add_subdirectory(${PROJECT_SOURCE_DIR}/../OLexicalAnalyser/ ${PROJECT_SOURCE_DIR}/buid/)
add_subdirectory(${PROJECT_SOURCE_DIR}/../OCompilerBase/ ${PROJECT_SOURCE_DIR}/build/)

add_library(OLanguageSematicAnalyser STATIC ${PROJECT_SOURCE_DIR}/SematicAnalyser.cpp)

target_include_directories(OLanguageSematicAnalyser PRIVATE ${PROJECT_SOURCE_DIR}/../OLexicalAnalyser)
target_include_directories(OLanguageSematicAnalyser PRIVATE ${PROJECT_SOURCE_DIR}/../OCompilerBase)
target_link_libraries(OLanguageSematicAnalyser LINK_PUBLIC OLexicalAnalyser)
