project(mts-ui VERSION 1.0.0 LANGUAGES CXX)

set(MTS_UI_LIBRARY_NAME mts_ui)

# Sources.
set(MTS_UI_INCLUDE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(MTS_UI_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE MTS_UI_HEADER_FILES "${MTS_UI_INCLUDE_DIRECTORY}/*.h")
file(GLOB_RECURSE MTS_UI_SOURCE_FILES "${MTS_UI_SOURCE_DIRECTORY}/*.cpp" "${MTS_UI_SOURCE_DIRECTORY}/*.mm" "${MTS_UI_SOURCE_DIRECTORY}/*.h")

source_group(TREE "${MTS_UI_INCLUDE_DIRECTORY}/mts/ui" PREFIX include FILES ${MTS_UI_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MTS_UI_SOURCE_FILES})

# Library.
add_library(${PROJECT_NAME} STATIC ${MTS_UI_HEADER_FILES} ${MTS_UI_SOURCE_FILES})

set_mts_compile_options(${PROJECT_NAME} PRIVATE)

target_include_directories(${PROJECT_NAME} PUBLIC ${MTS_UI_INCLUDE_DIRECTORY})

target_link_libraries(${PROJECT_NAME} PUBLIC mts::core mts::graphics)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework CoreText"
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER ${MTS_MODULES_FOLDER_NAME}
    XCODE_GENERATE_SCHEME NO
    ARCHIVE_OUTPUT_NAME ${MTS_UI_LIBRARY_NAME}
    ARCHIVE_OUTPUT_DIRECTORY ${MTS_MODULES_OUTPUT_DIRECTORY}
)

add_library(mts::ui ALIAS ${PROJECT_NAME})

# Add tests target.
if (MTS_BUILD_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()
