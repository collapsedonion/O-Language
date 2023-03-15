cmake_minimum_required(VERSION 3.25)
include(../OVM_SDK/CMakeLists.txt)
project(OAppKit)

set(CMAKE_CXX_STANDARD 17)

add_library(OAppKit SHARED library.cpp InterruptDefinitions.h InterruptImplemantation.mm AppDelegate.h AppDelegate.mm)

target_link_libraries(OAppKit PRIVATE
        "-framework AppKit"
        )
