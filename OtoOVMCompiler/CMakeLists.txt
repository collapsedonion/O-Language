cmake_minimum_required(VERSION 3.25)
include(../O-ByteCode-generator/CMakeLists.txt)
include(../OCompilerBase/CMakeLists.txt)
set(CMAKE_EXPORT_COMPILE_COMMAND ON)
project(OtoOVMCompiler)

set(CMAKE_CXX_STANDARD 17)

add_library(OtoOVMCompiler ../OtoOVMCompiler/OtoOTranslator.cpp ../OtoOVMCompiler/OtoOTranslator.h)

target_include_directories(OtoOVMCompiler PUBLIC ../O-ByteCode-generator)
target_link_libraries(OtoOVMCompiler PUBLIC O_ByteCode_generator)
