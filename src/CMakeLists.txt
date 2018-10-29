include(SDL2)
include(nuklear)
include(ResourceCompiler)
include(glew)
include(eigen)

include_directories(
        ${SDL2_INCLUDE_DIR}
        ${NUKLEAR_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIR}
        ${EIGEN_INCLUDE_DIR}
)

file(GLOB_RECURSE SOURCES *.cpp)
compile_resources(SOURCES glsl *.glsl)

add_executable(NLG ${SOURCES})

target_link_libraries(
        NLG
        ${SDL2_LIBRARY}
        ${GLEW_LIBRARY}
)

sdl2_install_runtimes(NLG)
glew_install_runtimes(NLG)

install(TARGETS NLG RUNTIME DESTINATION ${OUTPUT_DIRECTORY})
