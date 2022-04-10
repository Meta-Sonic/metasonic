function(set_mts_compile_options PROJECT_NAME EXPOSURE)

    #-------------------------------------------------------------
    # Clang.
    #-------------------------------------------------------------
	if (("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "AppleClang"))
        set(MTS_CLANG_COMMON_FLAGS
            -stdlib=libc++

            # Warnings.
            -Wall
            -Wpedantic
            -Woverloaded-virtual
            -Wreorder
            -Wuninitialized
            -Wunused-parameter
            -Wshift-sign-overflow
            -Wno-switch
            -Wswitch-enum
            -Wno-ignored-qualifiers
            -Wunused-private-field
            -Wunreachable-code
            -Wcast-align
            -Winconsistent-missing-destructor-override
            -Wnullable-to-nonnull-conversion
            -Wno-missing-field-initializers
            -Wsuggest-override
        )

        if ((CMAKE_BUILD_TYPE EQUAL "Release") OR (CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo"))
            # Release.
            target_compile_definitions(${PROJECT_NAME} ${EXPOSURE} "NDEBUG=1")

            target_compile_options(${PROJECT_NAME} ${EXPOSURE}
                -Ofast
                -ffast-math
                ${MTS_CLANG_COMMON_FLAGS}
            )

            if (CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo")
                target_compile_options(${PROJECT_NAME} ${EXPOSURE} -g)
            endif()

        else()
            # Debug.
            target_compile_definitions(${PROJECT_NAME} ${EXPOSURE}
                "_DEBUG=1"
                "DEBUG=1"
            )

            target_compile_options(${PROJECT_NAME} ${EXPOSURE}
                -O0
                -g
                ${MTS_CLANG_COMMON_FLAGS}
            )
        endif()

    #-------------------------------------------------------------
    # g++.
    #-------------------------------------------------------------
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

        set(MTS_GCC_COMMON_FLAGS
            -Wall
            -Wshadow-all
            -Wshorten-64-to-32
            -Wstrict-aliasing
            -Wuninitialized
            -Wunused-parameter
            -Wconversion
            -Wsign-compare
            -Wint-conversion
            -Wconditional-uninitialized
            -Woverloaded-virtual
            -Wreorder
            -Wconstant-conversion
            -Wsign-conversion
            -Wunused-private-field
            -Wbool-conversion
            -Wextra-semi
            -Wunreachable-code
            -Wzero-as-null-pointer-constant
            -Wcast-align
            -Winconsistent-missing-destructor-override
            -Wshift-sign-overflow
            -Wnullable-to-nonnull-conversion
            -Wno-missing-field-initializers
            -Wno-ignored-qualifiers
            -Wswitch-enum
            -Wpedantic
        )

        if ((CMAKE_BUILD_TYPE EQUAL "Release") OR (CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo"))
            # Release.
            target_compile_definitions(${PROJECT_NAME} ${EXPOSURE} "NDEBUG=1")

            target_compile_options(${PROJECT_NAME} ${EXPOSURE}
                -Ofast
                -ffast-math
                -stdlib=libc++
                ${MTS_GCC_COMMON_FLAGS}
            )

            if (CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo")
                target_compile_options(${PROJECT_NAME} ${EXPOSURE} -g)
            endif()
        else()
            # Debug.
            target_compile_definitions(${PROJECT_NAME} ${EXPOSURE}
                "_DEBUG=1"
                "DEBUG=1"
            )

            target_compile_options(${PROJECT_NAME} ${EXPOSURE}
                -stdlib=libc++
                -O0
                -g
                ${MTS_GCC_COMMON_FLAGS}
            )
        endif()

    #-------------------------------------------------------------
    # Intel.
    #-------------------------------------------------------------
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
        if ((CMAKE_BUILD_TYPE EQUAL "Release") OR (CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo"))
            # Release.
            target_compile_definitions(${PROJECT_NAME} ${EXPOSURE} "NDEBUG=1" )

            target_compile_options(${PROJECT_NAME} ${EXPOSURE}
                # Flags
            )

            if (CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo")
                target_compile_options(${PROJECT_NAME} ${EXPOSURE} -g)
            endif()
        else()
            # Debug.
            target_compile_definitions(${PROJECT_NAME} ${EXPOSURE}
                "_DEBUG=1"
                "DEBUG=1"
            )

            target_compile_options(${PROJECT_NAME} ${EXPOSURE}
            # Flags
            )
        endif()

    #-------------------------------------------------------------
    # MSVC.
    #-------------------------------------------------------------
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(MTS_MSVC_COMMON_FLAGS
                /Zc:alignedNew
                /W4
            )

        set(MTS_MSVC_COMMON_DEFS
            NOMINMAX
            UNICODE
            _UNICODE
            _SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING
        )

        if ((CMAKE_BUILD_TYPE EQUAL "Release") OR (CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo"))
            # Release.
            target_compile_definitions(${PROJECT_NAME} ${EXPOSURE}
                ${MTS_MSVC_COMMON_DEFS}
            )

            target_compile_options(${PROJECT_NAME} ${EXPOSURE}
                ${MTS_MSVC_COMMON_FLAGS}
            )

            if (CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo")
                target_compile_options(${PROJECT_NAME} ${EXPOSURE} /Zi)
            endif()

        else()
            # Debug.
            target_compile_definitions(${PROJECT_NAME} ${EXPOSURE}
                ${MTS_MSVC_COMMON_DEFS}
                "_DEBUG=1"
                "DEBUG=1"
            )

            target_compile_options(${PROJECT_NAME} ${EXPOSURE}
                ${MTS_MSVC_COMMON_FLAGS}
            )
        endif()
	endif()
endfunction()