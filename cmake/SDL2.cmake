
if (MINGW)
    set(SDL2_PACKAGE_NAME SDL2-devel-2.0.8-mingw.tar.gz)
    set(SDL2_HOME "${LIBS_DIRECTORY}/SDL2")
    set(SDL2_PACKAGE "${SDL2_HOME}/${SDL2_PACKAGE_NAME}")
    set(SDL2_DOWNLOAD_DIR ${CMAKE_BINARY_DIR})
    set(SDL2_BINARY_HOME ${SDL2_DOWNLOAD_DIR}/SDL2-2.0.8)

    message(STATUS "Extracting ${SDL2_PACKAGE_NAME} into ${SDL2_DOWNLOAD_DIR}")
    execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xzf ${SDL2_PACKAGE}
            WORKING_DIRECTORY ${SDL2_DOWNLOAD_DIR}
            RESULT_VARIABLE retcode
    )
    if (NOT "${retcode}" STREQUAL "0")
        message(FATAL_ERROR "Fatal error when extracting ${SDL2_PACKAGE}")
    endif ()

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(postfix x86_64-w64-mingw32)
    elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(postfix i686-w64-mingw32)
    endif ()

    set(SDL2_INCLUDE_DIR ${SDL2_BINARY_HOME}/${postfix}/include)
    set(SDL2_LIBRARY ${SDL2_BINARY_HOME}/${postfix}/lib/libSDL2.dll.a opengl32 glu32)
    set(SDL2_RUNTIME_LIBRARY ${SDL2_BINARY_HOME}/${postfix}/bin/SDL2.dll)

else ()
    message(FATAL_ERROR "Unable to configure SDL2: Unknown platform")
endif ()

macro(sdl_add_binaries target)
    if (NOT ${SDL2_RUNTIME_LIBRARY})
        add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL2_RUNTIME_LIBRARY}
                $<TARGET_FILE_DIR:${target}>)
        install(FILES ${SDL2_RUNTIME_LIBRARY} DESTINATION ${OUTPUT_DIRECTORY})
    endif ()
endmacro()
