cmake_minimum_required(VERSION 3.25)
include(../OVM_SDK/CMakeLists.txt)
project(OVirtualMachine)

set(CMAKE_CXX_STANDARD 17)

add_executable(OVirtualMachine main.cpp Memory.cpp Memory.h LogicUnit.cpp LogicUnit.h Scenary.cpp Scenary.h)

add_custom_command(TARGET OVirtualMachine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/../OStdLib/STD_MAC_BIN
        ${CMAKE_BINARY_DIR}/stdbin)

add_custom_command(TARGET OVirtualMachine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/../OStdLib/libs.conf
        ${CMAKE_BINARY_DIR}/stdbin/libs.conf)