
if (MINGW)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(GLEW_PACKAGE_NAME glew-2.0.0-mingw-w64.zip)
    elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(GLEW_PACKAGE_NAME glew-2.0.0-mingw.zip)
    endif ()

    set(GLEW_HOME "${LIBS_DIRECTORY}/glew")
    set(GLEW_PACKAGE "${GLEW_HOME}/${GLEW_PACKAGE_NAME}")
    set(GLEW_DOWNLOAD_DIR ${CMAKE_BINARY_DIR})
    set(GLEW_BINARY_HOME ${GLEW_DOWNLOAD_DIR}/glew-2.0.0)

    set(GLEW_INCLUDE_DIR ${GLEW_BINARY_HOME}/include)
    set(GLEW_LIBRARY ${GLEW_BINARY_HOME}/lib/libglew32.dll.a)
    set(GLEW_RUNTIME_LIBRARY ${GLEW_BINARY_HOME}/lib/glew32.dll)

    if (NOT GLEW_EXTRACTED)
        message(STATUS "Extracting ${GLEW_PACKAGE_NAME} into ${GLEW_DOWNLOAD_DIR}")
        execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xzf ${GLEW_PACKAGE}
                WORKING_DIRECTORY ${GLEW_DOWNLOAD_DIR}
                RESULT_VARIABLE retcode
        )
        if (NOT "${retcode}" STREQUAL "0")
            message(FATAL_ERROR "Fatal error when extracting ${GLEW_PACKAGE}")
        endif ()

        set(GLEW_EXTRACTED ON CACHE BOOL "")
    endif ()

else ()
    message(FATAL_ERROR "Unable to configure glew: Unknown platform")
endif ()

macro(glew_install_runtimes target)
    if (GLEW_RUNTIME_LIBRARY)
        add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GLEW_RUNTIME_LIBRARY}
                $<TARGET_FILE_DIR:${target}>)
        install(FILES ${GLEW_RUNTIME_LIBRARY} DESTINATION ${OUTPUT_DIRECTORY})
    endif ()
endmacro()
