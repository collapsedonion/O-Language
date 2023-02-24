cmake_minimum_required(VERSION 3.25)
project(OCompilerMACOS)

set(CMAKE_CXX_STANDARD 17)

include_directories(../OLanguageSematicAnalyser)
include_directories(../OCompilerBase)
include_directories(../OtoPythonTranslator)
include_directories(../OLexicalAnalyser)

link_directories(../OLexicalAnalyser/cmake-build-debug)
link_directories(../OLanguageSematicAnalyser/cmake-build-debug)
link_directories(../OtoPythonTranslator/cmake-build-debug)

add_executable(OCompilerMAC main.cpp)

target_link_libraries(OCompilerMAC LINK_PUBLIC OLexicalAnalyser)

target_link_libraries(OCompilerMAC LINK_PUBLIC OLanguageSematicAnalyser)

target_link_libraries(OCompilerMAC LINK_PUBLIC OtoPythonTranslator)
