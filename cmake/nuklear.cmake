
set(NUKLEAR_PACKAGE_NAME nuklear.zip)
set(NUKLEAR_HOME "${LIBS_DIRECTORY}/nuklear")
set(NUKLEAR_PACKAGE "${NUKLEAR_HOME}/${NUKLEAR_PACKAGE_NAME}")
set(NUKLEAR_UNPACK_DIR ${CMAKE_BINARY_DIR})

if (NOT NUKLEAR_EXTRACTED)
    message(STATUS "Extracting ${NUKLEAR_PACKAGE_NAME} into ${NUKLEAR_UNPACK_DIR}")
    execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xzf ${NUKLEAR_PACKAGE}
            WORKING_DIRECTORY ${NUKLEAR_UNPACK_DIR}
            RESULT_VARIABLE retcode
    )

    if (NOT "${retcode}" STREQUAL "0")
        message(FATAL_ERROR "Fatal error when extracting ${NUKLEAR_PACKAGE}")
    endif ()

    set(NUKLEAR_EXTRACTED ON CACHE BOOL "")
endif ()

set(NUKLEAR_INCLUDE_DIR ${NUKLEAR_UNPACK_DIR}/nuklear)
