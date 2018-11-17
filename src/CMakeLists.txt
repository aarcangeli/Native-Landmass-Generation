include(SDL2)
include(SDL2_image)
include(nuklear)
include(ResourceCompiler)
include(glew)
include(eigen)
include(glm)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${SDL2_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIR}
        ${NUKLEAR_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIR}
        ${EIGEN_INCLUDE_DIR}
        ${GLM_INCLUDE_DIR}
)

file(GLOB_RECURSE SOURCES *.cpp)
compile_resources(SOURCES glsl *.glsl)
compile_resources(SOURCES img *.png)

add_executable(NLG ${SOURCES})

target_link_libraries(
        NLG
        ${SDL2_LIBRARY}
        ${GLEW_LIBRARY}
        ${SDL2_IMAGE_LIBRARY}
)

sdl2_install_runtimes(NLG)
glew_install_runtimes(NLG)
sdl2_image_install_runtimes(NLG)

install(TARGETS NLG RUNTIME DESTINATION ${OUTPUT_DIRECTORY})

string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE)
if ("${BUILD_TYPE}" STREQUAL "RELEASE" OR "${BUILD_TYPE}" STREQUAL "RELWITHDEBINFO" OR "${BUILD_TYPE}" STREQUAL "MINSIZEREL")
    message(STATUS "Release build")
    add_definitions(-DNLG_RELEASE)
endif ()
