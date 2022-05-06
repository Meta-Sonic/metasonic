cmake_minimum_required(VERSION 3.21)

project(mts-examples-audio-app)

set(AUDIO_APP_INCLUDE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(AUDIO_APP_SRC_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(AUDIO_APP_RESOURCES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets)

# All sources.
file(GLOB_RECURSE AUDIO_APP_INCLUDE_FILES
    "${AUDIO_APP_INCLUDE_DIRECTORY}/*.h"
)

file(GLOB_RECURSE AUDIO_APP_SOURCE_FILES
    "${AUDIO_APP_SRC_DIRECTORY}/*.cpp"
    "${AUDIO_APP_SRC_DIRECTORY}/*.c"
    "${AUDIO_APP_SRC_DIRECTORY}/*.m"
    "${AUDIO_APP_SRC_DIRECTORY}/*.mm"
    "${AUDIO_APP_SRC_DIRECTORY}/*.h"
)

# File groups.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${AUDIO_APP_INCLUDE_FILES} ${AUDIO_APP_SOURCE_FILES})

add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${AUDIO_APP_INCLUDE_FILES} ${AUDIO_APP_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${AUDIO_APP_INCLUDE_DIRECTORY}
    ${AUDIO_APP_SRC_DIRECTORY}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    mts::core
    mts::graphics
    mts::ui
    mts::audio
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
    RESOURCES_DIRECTORY="${AUDIO_APP_RESOURCES_DIRECTORY}"
)