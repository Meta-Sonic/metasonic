cmake_minimum_required(VERSION 3.21)

project(mts-examples-discoverable)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE mts::core mts::graphics)

set_mts_compile_options(${PROJECT_NAME} PRIVATE)

target_include_directories(${PROJECT_NAME} PUBLIC
"${CMAKE_CURRENT_SOURCE_DIR}"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER ${MTS_EXAMPLES_FOLDER_NAME}
    RUNTIME_OUTPUT_DIRECTORY ${MTS_EXAMPLES_OUTPUT_DIRECTORY}
)