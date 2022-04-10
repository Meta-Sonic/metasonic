function(mts_fetch_library)
    include(FetchContent)
    set(singleValues TARGET_NAME REPO_PREFIX FOLDER_PREFIX GIT_REPOSITORY GIT_TAG OPTIONS_MACRO)
    set(multiValues LIBS)

    include(CMakeParseArguments)
    cmake_parse_arguments(
        "OPT"
        ""
        "${singleValues}"
        "${multiValues}"
        ${ARGN}
    )

    set(targetName ${OPT_TARGET_NAME})
    string(TOUPPER ${targetName} targetNameUpper)

    get_filename_component(repoName  ${OPT_GIT_REPOSITORY} NAME_WE)

    if (IS_DIRECTORY "${repoPrefix}/${repoName}")
        set("MTS_${targetNameUpper}_SOURCE_DIR" "${OPT_REPO_PREFIX}/${repoName}")

        # If this is set, no download or update steps are performed for the specified
        # content and the <lcName>_SOURCE_DIR variable returned to the caller is
        # pointed at this location. This gives developers a way to have a separate
        # checkout of the content that they can modify freely without interference
        # from the build. The build simply uses that existing source, but it still
        # defines <lcName>_BINARY_DIR to point inside its own build area.
        set("FETCHCONTENT_SOURCE_DIR_${targetNameUpper}" "${MTS_${targetNameUpper}_SOURCE_DIR}")
    endif()

    FetchContent_Declare(${targetName}
        GIT_REPOSITORY ${OPT_GIT_REPOSITORY}
        GIT_TAG ${OPT_GIT_TAG}
        GIT_PROGRESS ON
        GIT_SHALLOW ON
    )

    FetchContent_GetProperties(${targetName})
    if (NOT ${targetName}_POPULATED)

        if (OPT_OPTIONS_MACRO)
            cmake_language(CALL ${OPT_OPTIONS_MACRO})
        endif()

        # Fetch the content using previously declared details.
        FetchContent_Populate(${targetName})

        # Bring the populated content into the build.
        add_subdirectory(${${targetName}_SOURCE_DIR} ${${targetName}_BINARY_DIR})

        if (OPT_LIBS)
            set_target_properties(${OPT_LIBS} PROPERTIES FOLDER ${OPT_FOLDER_PREFIX})
        endif()
    endif()
endfunction()
