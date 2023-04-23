cmake_minimum_required(VERSION 3.25)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(OCompiler)
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)

set(CMAKE_CXX_STANDARD 17)

add_executable(OCompiler ${PROJECT_SOURCE_DIR}/main.cpp)


add_subdirectory(${PROJECT_SOURCE_DIR}/../OCompilerBase/ ${PROJECT_SOURCE_DIR}/build/ocb)
add_subdirectory(${PROJECT_SOURCE_DIR}/../OLanguageSematicAnalyser/ ${PROJECT_SOURCE_DIR}/build/olsa)
add_subdirectory(${PROJECT_SOURCE_DIR}/../OPreProccessor/ ${PROJECT_SOURCE_DIR}/build/opp)
add_subdirectory(${PROJECT_SOURCE_DIR}/../OtoOVMCompiler/ ${PROJECT_SOURCE_DIR}/build/otoc)

add_subdirectory(${PROJECT_SOURCE_DIR}/../CDC/ ${CMAKE_SOURCE_DIR}/build/cdc)



target_include_directories(OCompiler PRIVATE ${PROJECT_SOURCE_DIR}/../OLanguageSematicAnalyser)
target_include_directories(OCompiler PRIVATE ${PROJECT_SOURCE_DIR}/../OLexicalAnalyser)
target_include_directories(OCompiler PRIVATE ${PROJECT_SOURCE_DIR}/../OPreProccessor)
target_include_directories(OCompiler PRIVATE ${PROJECT_SOURCE_DIR}/../OtoOVMCompiler)
target_include_directories(OCompiler PRIVATE ${PROJECT_SOURCE_DIR}/../O-ByteCode-generator)
target_include_directories(OCompiler PRIVATE ${PROJECT_SOURCE_DIR}/../OCompilerBase)
target_include_directories(OCompiler PRIVATE ${PROJECT_SOURCE_DIR}/../CDC)

target_link_libraries(OCompiler LINK_PUBLIC OLexicalAnalyser)

target_link_libraries(OCompiler LINK_PUBLIC OLanguageSematicAnalyser)

target_link_libraries(OCompiler LINK_PUBLIC OPreProccessor)

target_link_libraries(OCompiler LINK_PUBLIC OtoOVMCompiler)

target_link_libraries(OCompiler LINK_PRIVATE CDC)

add_custom_command(TARGET OCompiler  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
	${PROJECT_SOURCE_DIR}/../OStdLib/STD_HEADERS
	${PROJECT_BINARY_DIR}/stdlib)
