set(EIGEN_HOME "${LIBS_DIRECTORY}/eigen")
set(EIGEN_PACKAGE_NAME eigen-3.2.10.tar.bz2)
set(EIGEN_PACKAGE "${EIGEN_HOME}/${EIGEN_PACKAGE_NAME}")
set(EIGEN_DOWNLOAD_DIR ${CMAKE_BINARY_DIR})
set(EIGEN_SOURCE_HOME ${EIGEN_DOWNLOAD_DIR}/eigen-eigen-b9cd8366d4e8)

set(EIGEN_INCLUDE_DIR ${EIGEN_SOURCE_HOME})

if (NOT EIGEN_EXTRACTED)
    message(STATUS "Extracting ${EIGEN_PACKAGE_NAME} into ${EIGEN_DOWNLOAD_DIR}")
    execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xzf ${EIGEN_PACKAGE}
            WORKING_DIRECTORY ${EIGEN_DOWNLOAD_DIR}
            RESULT_VARIABLE retcode
    )

    if (NOT "${retcode}" STREQUAL "0")
        message(FATAL_ERROR "Fatal error when extracting ${EIGEN_PACKAGE}")
    endif ()

    set(EIGEN_EXTRACTED ON CACHE BOOL "")
endif ()
