cmake_minimum_required(VERSION 3.25)
project(OtoPythonTranslator)

set(CMAKE_CXX_STANDARD 17)

add_library(OtoPythonTranslator STATIC pch.cpp Call.cpp Translator.cpp)

target_include_directories(OtoPythonTranslator PUBLIC ../OCompilerBase)