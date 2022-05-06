cmake_minimum_required(VERSION 3.21)

project(demo)

set(DEMO_INCLUDE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DEMO_SRC_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(DEMO_RESOURCES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets)

# All sources.
file(GLOB_RECURSE DEMO_INCLUDE_FILES "${DEMO_INCLUDE_DIRECTORY}/*.h")
file(GLOB_RECURSE DEMO_SOURCE_FILES "${DEMO_SRC_DIRECTORY}/*.cpp" "${DEMO_SRC_DIRECTORY}/*.c" "${DEMO_SRC_DIRECTORY}/*.m" "${DEMO_SRC_DIRECTORY}/*.mm" "${DEMO_SRC_DIRECTORY}/*.h")

# File groups.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${DEMO_INCLUDE_FILES} ${DEMO_SOURCE_FILES})
# make_pretty_source_folders("include" "include" "${DEMO_INCLUDE_FILES}")
# make_pretty_source_folders("src" "src" "${DEMO_SOURCE_FILES}")


add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${DEMO_INCLUDE_FILES} ${DEMO_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${DEMO_INCLUDE_DIRECTORY}
    ${DEMO_SRC_DIRECTORY}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    mts::core
    mts::graphics
    mts::ui
    "-framework CoreGraphics"
    "-framework CoreText"
    "-framework AppKit"
    "-framework Security"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER ${MTS_EXAMPLES_FOLDER_NAME}
    RUNTIME_OUTPUT_DIRECTORY ${MTS_EXAMPLES_OUTPUT_DIRECTORY}
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    RESOURCES_DIRECTORY="${DEMO_RESOURCES_DIRECTORY}"
)