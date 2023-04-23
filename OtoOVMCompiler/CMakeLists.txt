cmake_minimum_required(VERSION 3.25)
project(OtoOVMCompiler)
set(CMAKE_EXPORT_COMPILE_COMMAND ON)

set(CMAKE_CXX_STANDARD 17)
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)

add_subdirectory(${PROJECT_SOURCE_DIR}/../O-ByteCode-generator/ ${PROJECT_SOURCE_DIR}/../O-ByteCode-generator/build/)
add_subdirectory(${PROJECT_SOURCE_DIR}/../OCompilerBase/ ${PROJECT_SOURCE_DIR}/../OCompilerBase/build/)

add_library(OtoOVMCompiler ${PROJECT_SOURCE_DIR}/OtoOTranslator.cpp ${PROJECT_SOURCE_DIR}/OtoOTranslator.h)

target_include_directories(OtoOVMCompiler PUBLIC ${PROJECT_SOURCE_DIR}/../O-ByteCode-generator)
target_include_directories(OtoOVMCompiler PRIVATE ${PROJECT_SOURCE_DIR}/../OCompilerBase)
target_link_libraries(OtoOVMCompiler PUBLIC O_ByteCode_generator)
