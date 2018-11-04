include(unpack)

set(SDL2_IMAGE_HOME "${LIBS_DIRECTORY}/SDL_image")
set(SDL2_IMAGE_UNPACK_DIR ${CMAKE_BINARY_DIR})
set(SDL2_IMAGE_BINARY_HOME ${SDL2_IMAGE_UNPACK_DIR}/SDL2_image-2.0.4)

if (MINGW)
    set(SDL2_IMAGE_PACKAGE_NAME SDL2_image-devel-2.0.4-mingw.tar.gz)
    set(SDL2_IMAGE_PACKAGE "${SDL2_IMAGE_HOME}/${SDL2_IMAGE_PACKAGE_NAME}")

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(postfix x86_64-w64-mingw32)
    elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(postfix i686-w64-mingw32)
    endif ()

    set(SDL2_IMAGE_INCLUDE_DIR ${SDL2_IMAGE_BINARY_HOME}/${postfix}/include/SDL2)
    set(SDL2_IMAGE_LIBRARY ${SDL2_IMAGE_BINARY_HOME}/${postfix}/lib/libSDL2_image.dll.a)
    set(SDL2_IMAGE_RUNTIME_LIBRARY ${SDL2_IMAGE_BINARY_HOME}/${postfix}/bin/SDL2_image.dll ${SDL2_IMAGE_BINARY_HOME}/${postfix}/bin/libpng16-16.dll ${SDL2_IMAGE_BINARY_HOME}/${postfix}/bin/zlib1.dll)

    unpack(SDL2_IMAGE ${SDL2_IMAGE_PACKAGE} ${SDL2_IMAGE_UNPACK_DIR})

else ()
    message(FATAL_ERROR "Unable to configure SDL2_image: Unknown platform")
endif ()

macro(sdl2_image_install_runtimes target)
    if (SDL2_IMAGE_RUNTIME_LIBRARY)
        add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL2_IMAGE_RUNTIME_LIBRARY}
                $<TARGET_FILE_DIR:${target}>)
        install(FILES ${SDL2_IMAGE_RUNTIME_LIBRARY} DESTINATION ${OUTPUT_DIRECTORY})
    endif ()
endmacro()
