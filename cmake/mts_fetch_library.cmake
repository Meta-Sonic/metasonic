
include(mts_fetch_util)

#
# googletest.
#
function(mts_fetch_googletest repoPrefix prefix)

    macro(Options)
        if (APPLE)
            set(CMAKE_MACOSX_RPATH ON)
        endif (APPLE)

        # For Windows: Prevent overriding the parent project's compiler/linker settings.
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endmacro()

    mts_fetch_library(
        TARGET_NAME googletest
        REPO_PREFIX ${repoPrefix}
        FOLDER_PREFIX ${prefix}
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG e2239ee6043f73722e7aa812a459f54a28552929
        OPTIONS_MACRO Options
        LIBS gtest gtest_main gmock gmock_main
    )

    set_target_properties(gtest PROPERTIES XCODE_GENERATE_SCHEME NO)
    set_target_properties(gtest_main PROPERTIES XCODE_GENERATE_SCHEME NO)
    set_target_properties(gmock PROPERTIES XCODE_GENERATE_SCHEME NO)
    set_target_properties(gmock_main PROPERTIES XCODE_GENERATE_SCHEME NO)
endfunction()

#
# benchmark.
#
function(mts_fetch_benchmark repoPrefix prefix)
    macro(Options)
        set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
        set(BENCHMARK_INSTALL_DOCS OFF CACHE BOOL "" FORCE)
        set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    endmacro()

    mts_fetch_library(
        TARGET_NAME benchmark
        REPO_PREFIX ${repoPrefix}
        FOLDER_PREFIX ${prefix}
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG 0d98dba29d66e93259db7daa53a9327df767a415
        OPTIONS_MACRO Options
        LIBS benchmark benchmark_main
    )

    set_target_properties(benchmark PROPERTIES XCODE_GENERATE_SCHEME NO)
    set_target_properties(benchmark_main PROPERTIES XCODE_GENERATE_SCHEME NO)
endfunction()

#
# miniz.
#
function(mts_fetch_miniz repoPrefix prefix)
    macro(Options)
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(INSTALL_PROJECT OFF CACHE BOOL "" FORCE)
    endmacro()

    mts_fetch_library(
        TARGET_NAME miniz
        REPO_PREFIX ${repoPrefix}
        FOLDER_PREFIX ${prefix}
        GIT_REPOSITORY https://github.com/richgel999/miniz.git
        GIT_TAG cb97387d20909c0a4728972fd0f19bfacd083c42
        OPTIONS_MACRO Options
        LIBS miniz
    )
endfunction()
