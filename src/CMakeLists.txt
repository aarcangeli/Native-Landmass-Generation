include(SDL2)

include_directories(
        ${SDL2_INCLUDE_DIR}
)

file(GLOB_RECURSE SOURCES *.cpp)
add_executable(NLG ${SOURCES})

target_link_libraries(
        NLG
        ${SDL2_LIBRARY}
)

install(TARGETS NLG RUNTIME DESTINATION ${OUTPUT_DIRECTORY})

sdl_add_binaries(NLG)
