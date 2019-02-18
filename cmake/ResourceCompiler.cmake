if (_CMRC_GENERATE_MODE)
    # Read in the digits
    file(READ "${INPUT_FILE}" bytes HEX)
    string(LENGTH "${bytes}" size)
    math(EXPR size "${size}/2")
    string(REGEX REPLACE "(..)" "'\\\\x\\1', " chars "${bytes}")
    string(REGEX REPLACE "(................................................................................)" "\\1\n        " chars "${chars}")
    set(SOURCE "")
    string(APPEND SOURCE "namespace ${namespace} {\n")
    string(APPEND SOURCE "    const char ${VAR_NAME}_array[] = {\n        ${chars}};\n")
    string(APPEND SOURCE "    const char* ${VAR_NAME} = ${VAR_NAME}_array;\n")
    string(APPEND SOURCE "    const char* ${VAR_NAME}_end = ${VAR_NAME}_array + sizeof(${VAR_NAME}_array);\n")
    string(APPEND SOURCE "}\n")
    file(WRITE "${OUTPUT_FILE}" "${SOURCE}")
    return()
endif ()

macro(compile_resources output_list namespace infile)
    file(GLOB_RECURSE RES_SOURCES ${infile})
    set(CR_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/resources/include)
    set(CR_HEADER_FILE "")
    foreach (it ${RES_SOURCES})
        get_filename_component(it_name ${it} NAME)
        get_filename_component(it_name_we ${it} NAME_WE)
        string(REGEX REPLACE [^0-9a-zA-Z] _ it_name_we ${it_name_we})
        string(REGEX REPLACE ^[^a-zA-Z] _ it_name_we ${it_name_we})
        set(OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/resources/${it_name}.cpp)
        add_custom_command(
                # This is the file we will generate
                OUTPUT "${OUTPUT_FILE}"
                # These are the primary files that affect the output
                DEPENDS ${CMAKE_MODULE_PATH}/ResourceCompiler.cmake "${it}"
                COMMAND "${CMAKE_COMMAND}" -D_CMRC_GENERATE_MODE=TRUE "-DINPUT_FILE=${it}" "-DOUTPUT_FILE=${OUTPUT_FILE}" "-Dnamespace=${namespace}" "-DVAR_NAME=${it_name_we}" -P ${CMAKE_MODULE_PATH}/ResourceCompiler.cmake
                COMMENT "Generating resource file ${it_name} into ${OUTPUT_FILE}"
        )
        list(APPEND ${output_list} ${OUTPUT_FILE})
        set(CR_HEADER_FILE "${CR_HEADER_FILE}    // ${it}\n")
        set(CR_HEADER_FILE "${CR_HEADER_FILE}    extern const char* ${it_name_we};\n")
        set(CR_HEADER_FILE "${CR_HEADER_FILE}    extern const char* ${it_name_we}_end;\n\n")
    endforeach ()
    file(WRITE "${CR_INCLUDE_DIR}/res_${namespace}.h" "namespace ${namespace} {\n\n${CR_HEADER_FILE}}\n")
    include_directories(${CR_INCLUDE_DIR})
endmacro()
