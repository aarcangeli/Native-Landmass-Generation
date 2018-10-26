# Postfix
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(postfix x64)
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(postfix x86)
endif ()

set(OUTPUT_DIRECTORY ${ROOT_DIRECTORY}/bin/${postfix})
set(LIBS_DIRECTORY ${ROOT_DIRECTORY}/libs)

if (MINGW)
    # remove dependencies to libstdc++-6.dll, libgcc_s_dw2-1.dll, libwinpthread-1.dll
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
endif ()
