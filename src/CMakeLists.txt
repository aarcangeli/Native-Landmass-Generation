include(SDL2)

include_directories(
        ${SDL2_INCLUDE_DIR}
)

file(GLOB_RECURSE SOURCES *.cpp)
add_executable(NPCG ${SOURCES})

target_link_libraries(
        NPCG
        ${SDL2_LIBRARY}
)

install(TARGETS NPCG RUNTIME DESTINATION ${OUTPUT_DIRECTORY})

sdl_add_binaries(NPCG)
