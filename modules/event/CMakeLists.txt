project(mts-event VERSION 1.0.0 LANGUAGES CXX)

set(MTS_EVENT_LIBRARY_NAME mts_event)

# Sources.
set(MTS_EVENT_INCLUDE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(MTS_EVENT_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE MTS_EVENT_HEADER_FILES "${MTS_EVENT_INCLUDE_DIRECTORY}/*.h")
file(GLOB_RECURSE MTS_EVENT_SOURCE_FILES "${MTS_EVENT_SOURCE_DIRECTORY}/*.cpp")

source_group(TREE "${MTS_EVENT_INCLUDE_DIRECTORY}/mts/event" PREFIX include FILES ${MTS_EVENT_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MTS_EVENT_SOURCE_FILES})

# Library.
add_library(${PROJECT_NAME} STATIC ${MTS_EVENT_HEADER_FILES} ${MTS_EVENT_SOURCE_FILES})

set_mts_compile_options(${PROJECT_NAME} PRIVATE)

target_include_directories(${PROJECT_NAME} PUBLIC ${MTS_EVENT_INCLUDE_DIRECTORY})

mts_fetch_libuv(${MTS_EXTERN_REPO_DIR} ${MTS_EXTERN_PREFIX})

target_link_libraries(${PROJECT_NAME} PUBLIC mts::core uv_a)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework CoreFoundation"
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER ${MTS_MODULES_FOLDER_NAME}
    XCODE_GENERATE_SCHEME NO
    ARCHIVE_OUTPUT_NAME ${MTS_EVENT_LIBRARY_NAME}
    ARCHIVE_OUTPUT_DIRECTORY ${MTS_MODULES_OUTPUT_DIRECTORY}
)

add_library(mts::event ALIAS ${PROJECT_NAME})

# Add tests target.
if (MTS_BUILD_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()
