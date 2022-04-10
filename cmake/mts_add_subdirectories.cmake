macro(mts_add_subdirectories directory)
    file(GLOB FOLDERS RELATIVE ${directory} ${directory}/*)

    foreach(folder ${FOLDERS})
        set(DIR_PATH ${directory}/${folder})

        if (IS_DIRECTORY ${DIR_PATH} AND EXISTS ${DIR_PATH}/CMakeLists.txt)
            add_subdirectory(${DIR_PATH})
        endif()
    endforeach()
endmacro()
