include(unpack)

set(GLEW_HOME "${LIBS_DIRECTORY}/glew")
set(GLEW_UNPACK_DIR ${CMAKE_BINARY_DIR})
set(GLEW_BINARY_HOME ${GLEW_UNPACK_DIR}/glew-2.0.0)

if (MINGW)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(GLEW_PACKAGE_NAME glew-2.0.0-mingw-w64.zip)
    elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(GLEW_PACKAGE_NAME glew-2.0.0-mingw.zip)
    endif ()

    set(GLEW_PACKAGE "${GLEW_HOME}/${GLEW_PACKAGE_NAME}")

    set(GLEW_INCLUDE_DIR ${GLEW_BINARY_HOME}/include)
    set(GLEW_LIBRARY ${GLEW_BINARY_HOME}/lib/libglew32.dll.a)
    set(GLEW_RUNTIME_LIBRARY ${GLEW_BINARY_HOME}/lib/glew32.dll)

    unpack(GLEW ${GLEW_PACKAGE} ${GLEW_UNPACK_DIR})

endif ()

macro(glew_install_runtimes target)
    if (GLEW_RUNTIME_LIBRARY)
        add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GLEW_RUNTIME_LIBRARY}
                $<TARGET_FILE_DIR:${target}>)
        install(FILES ${GLEW_RUNTIME_LIBRARY} DESTINATION ${OUTPUT_DIRECTORY})
    endif ()
endmacro()
