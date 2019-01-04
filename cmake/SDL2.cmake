include(unpack)

set(SDL2_HOME "${LIBS_DIRECTORY}/SDL2")
set(SDL2_UNPACK_DIR ${CMAKE_BINARY_DIR})
set(SDL2_BINARY_HOME ${SDL2_UNPACK_DIR}/SDL2-2.0.8)

if (MINGW)
    set(SDL2_PACKAGE_NAME SDL2-devel-2.0.8-mingw.tar.gz)
    set(SDL2_PACKAGE "${SDL2_HOME}/${SDL2_PACKAGE_NAME}")

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(postfix x86_64-w64-mingw32)
    elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(postfix i686-w64-mingw32)
    endif ()

    set(SDL2_INCLUDE_DIR ${SDL2_BINARY_HOME}/${postfix}/include/SDL2 ${SDL2_BINARY_HOME}/${postfix}/include)
    set(SDL2_LIBRARY ${SDL2_BINARY_HOME}/${postfix}/lib/libSDL2.dll.a opengl32 glu32)
    set(SDL2_RUNTIME_LIBRARY ${SDL2_BINARY_HOME}/${postfix}/bin/SDL2.dll)

    unpack(SDL2 ${SDL2_PACKAGE} ${SDL2_UNPACK_DIR})

else ()
    message(FATAL_ERROR "Unable to configure SDL2: Unknown platform")
endif ()

macro(sdl2_install_runtimes target)
    if (SDL2_RUNTIME_LIBRARY)
        add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL2_RUNTIME_LIBRARY}
                $<TARGET_FILE_DIR:${target}>)
        install(FILES ${SDL2_RUNTIME_LIBRARY} DESTINATION ${OUTPUT_DIRECTORY})
    endif ()
endmacro()
