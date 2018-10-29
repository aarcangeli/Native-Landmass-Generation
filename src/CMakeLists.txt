include(SDL2)
include(nuklear)
include(ResourceCompiler)
include(glew)

include_directories(
        ${SDL2_INCLUDE_DIR}
        ${NUKLEAR_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIR}
)

file(GLOB_RECURSE SOURCES *.cpp)
compile_resources(SOURCES glsl *.glsl)

add_executable(NLG ${SOURCES})

target_link_libraries(
        NLG
        ${SDL2_LIBRARY}
        ${GLEW_LIBRARY}
)

install(TARGETS NLG RUNTIME DESTINATION ${OUTPUT_DIRECTORY})

sdl_add_binaries(NLG)
glew_add_binaries(NLG)
