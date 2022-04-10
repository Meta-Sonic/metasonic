set(MTS_CORE_TEST_NAME mts-core-tests)

# Sources.
set(TESTS_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB_RECURSE SOURCES "${TESTS_SOURCE_DIRECTORY}/*.h" "${TESTS_SOURCE_DIRECTORY}/*.cpp")
source_group(TREE ${TESTS_SOURCE_DIRECTORY} FILES ${SOURCES})

# Target.
add_executable(${MTS_CORE_TEST_NAME})

target_sources(${MTS_CORE_TEST_NAME} PRIVATE ${SOURCES})

target_include_directories(${MTS_CORE_TEST_NAME} PRIVATE ${TESTS_SOURCE_DIRECTORY})

target_link_libraries(${MTS_CORE_TEST_NAME} PRIVATE mts::core gtest)

set_target_properties(${MTS_CORE_TEST_NAME} PROPERTIES FOLDER "tests")

target_compile_definitions(${MTS_CORE_TEST_NAME} PUBLIC
    MTS_TEST_RESOURCES_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/resources"
)
