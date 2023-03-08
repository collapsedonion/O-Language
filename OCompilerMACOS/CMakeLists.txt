cmake_minimum_required(VERSION 3.25)
include(../OCompilerBase/CMakeLists.txt)
include(../OLanguageSematicAnalyser/CMakeLists.txt)
include(../OPreProccessor/CMakeLists.txt)
include(../OtoOVMCompiler/CMakeLists.txt)

project(OCompilerMACOS)

set(CMAKE_CXX_STANDARD 17)

add_executable(OCompilerMAC main.cpp)

target_include_directories(OCompilerMAC PRIVATE ../OLanguageSematicAnalyser)
target_include_directories(OCompilerMAC PRIVATE ../OLexicalAnalyser)
target_include_directories(OCompilerMAC PRIVATE ../OPreProccessor)
target_include_directories(OCompilerMAC PRIVATE ../OtoOVMCompiler)
target_include_directories(OCompilerMAC PRIVATE ../O-ByteCode-generator)

target_link_libraries(OCompilerMAC LINK_PUBLIC OLexicalAnalyser)

target_link_libraries(OCompilerMAC LINK_PUBLIC OLanguageSematicAnalyser)

target_link_libraries(OCompilerMAC LINK_PUBLIC OPreProccessor)

target_link_libraries(OCompilerMAC LINK_PUBLIC OtoOVMCompiler)

add_custom_command(TARGET OCompilerMAC  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/../OStdLib/STD_HEADERS
        ${CMAKE_BINARY_DIR}/stdlib)
