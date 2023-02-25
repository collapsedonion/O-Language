cmake_minimum_required(VERSION 3.25)
include(../OCompilerBase/CMakeLists.txt)
include(../OLanguageSematicAnalyser/CMakeLists.txt)
include(../OtoPythonTranslator/CMakeLists.txt)

project(OCompilerMACOS)

set(CMAKE_CXX_STANDARD 17)

add_executable(OCompilerMAC main.cpp)

target_include_directories(OCompilerMAC PRIVATE ../OLanguageSematicAnalyser)
target_include_directories(OCompilerMAC PRIVATE ../OtoPythonTranslator)
target_include_directories(OCompilerMAC PRIVATE ../OLexicalAnalyser)

target_link_libraries(OCompilerMAC LINK_PUBLIC OLexicalAnalyser)

target_link_libraries(OCompilerMAC LINK_PUBLIC OLanguageSematicAnalyser)

target_link_libraries(OCompilerMAC LINK_PUBLIC OtoPythonTranslator)
