project(mts-audio VERSION 1.0.0 LANGUAGES CXX)

set(MTS_AUDIO_LIBRARY_NAME mts_audio)

# Sources.
set(MTS_AUDIO_INCLUDE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(MTS_AUDIO_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE MTS_AUDIO_HEADER_FILES "${MTS_AUDIO_INCLUDE_DIRECTORY}/*.h")
file(GLOB_RECURSE MTS_AUDIO_SOURCE_FILES "${MTS_AUDIO_SOURCE_DIRECTORY}/*.cpp" "${MTS_AUDIO_SOURCE_DIRECTORY}/*.h")

source_group(TREE "${MTS_AUDIO_INCLUDE_DIRECTORY}/mts/audio" PREFIX include FILES ${MTS_AUDIO_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MTS_AUDIO_SOURCE_FILES})

# Library.
add_library(${PROJECT_NAME} STATIC ${MTS_AUDIO_HEADER_FILES} ${MTS_AUDIO_SOURCE_FILES})

set_mts_compile_options(${PROJECT_NAME} PRIVATE)

target_include_directories(${PROJECT_NAME} PUBLIC ${MTS_AUDIO_INCLUDE_DIRECTORY})

target_link_libraries(${PROJECT_NAME} PUBLIC mts::core)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework CoreFoundation"
        "-framework CoreAudio"
        "-framework Accelerate")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER ${MTS_MODULES_FOLDER_NAME}
    XCODE_GENERATE_SCHEME NO
    ARCHIVE_OUTPUT_NAME ${MTS_AUDIO_LIBRARY_NAME}
    ARCHIVE_OUTPUT_DIRECTORY ${MTS_MODULES_OUTPUT_DIRECTORY}
)

add_library(mts::audio ALIAS ${PROJECT_NAME})

# Add tests target.
if (MTS_BUILD_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()

# Add benchmarks target.
if (MTS_BUILD_BENCHS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/benchmarks)
endif()
