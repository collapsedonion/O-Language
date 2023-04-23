cmake_minimum_required(VERSION 3.25)
include(../OVM_SDK/CMakeLists.txt)
project(OVirtualMachine)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
add_subdirectory(${PROJECT_SOURCE_DIR}/../CDC ${PROJECT_BINARY_DIR}/cdc)

add_executable(OVirtualMachine ${PROJECT_SOURCE_DIR}/main.cpp ${PROJECT_SOURCE_DIR}/Memory.cpp ${PROJECT_SOURCE_DIR}/Memory.h ${PROJECT_SOURCE_DIR}/LogicUnit.cpp ${PROJECT_SOURCE_DIR}/LogicUnit.h ${PROJECT_SOURCE_DIR}/Scenary.cpp ${PROJECT_SOURCE_DIR}/Scenary.h)

target_link_libraries(OVirtualMachine LINK_PRIVATE CDC)
target_include_directories(OVirtualMachine PRIVATE ${PROJECT_SOURCE_DIR}/../CDC/)

add_custom_command(TARGET OVirtualMachine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/../OStdLib/STD_MAC_BIN
        ${CMAKE_BINARY_DIR}/stdbin)

add_custom_command(TARGET OVirtualMachine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/../OStdLib/libs.conf
        ${CMAKE_BINARY_DIR}/stdbin/libs.conf)
