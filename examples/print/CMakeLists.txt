cmake_minimum_required(VERSION 3.21)

project(print)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE mts::core)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "examples")
