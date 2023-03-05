cmake_minimum_required(VERSION 3.25)
include(../OVM_SDK/CMakeLists.txt)
project(OVirtualMachine)

set(CMAKE_CXX_STANDARD 17)

add_executable(OVirtualMachine main.cpp Memory.cpp Memory.h LogicUnit.cpp LogicUnit.h Scenary.cpp Scenary.h)
