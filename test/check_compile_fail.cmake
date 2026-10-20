if(NOT DEFINED BUILD_DIR OR NOT DEFINED TARGET_NAME OR NOT DEFINED EXPECTED_TEXT)
    message(FATAL_ERROR "BUILD_DIR, TARGET_NAME, and EXPECTED_TEXT are required.")
endif()

set(build_command "${CMAKE_COMMAND}" --build "${BUILD_DIR}" --target "${TARGET_NAME}")
if(DEFINED CONFIG AND NOT CONFIG STREQUAL "")
    list(APPEND build_command --config "${CONFIG}")
endif()

execute_process(
    COMMAND ${build_command}
    RESULT_VARIABLE build_result
    OUTPUT_VARIABLE build_stdout
    ERROR_VARIABLE build_stderr
)

set(build_output "${build_stdout}\n${build_stderr}")

if(build_result EQUAL 0)
    message(FATAL_ERROR
        "Compile-fail target '${TARGET_NAME}' unexpectedly compiled successfully.\n"
        "${build_output}"
    )
endif()

# GCC and Clang echo the failing source line, which may contain the static_assert message itself.
# Match the text only on a static_assert error line (GCC, Clang, MSVC C2338).
string(REGEX REPLACE "([][+.*?()^$|\\{}])" "\\\\\\1" expected_regex "${EXPECTED_TEXT}")
string(REGEX MATCH "error( ?: static.assert| C2338:)[^\n]*${expected_regex}" diagnostic_match "${build_output}")
if(diagnostic_match STREQUAL "")
    message(FATAL_ERROR
        "Compile-fail target '${TARGET_NAME}' failed without the expected diagnostic:\n"
        "  ${EXPECTED_TEXT}\n"
        "Compiler output:\n${build_output}"
    )
endif()

if(DEFINED UNEXPECTED_TEXT AND NOT UNEXPECTED_TEXT STREQUAL "")
    string(FIND "${build_output}" "${UNEXPECTED_TEXT}" unexpected_diagnostic_position)
    if(NOT unexpected_diagnostic_position EQUAL -1)
        message(FATAL_ERROR
            "Compile-fail target '${TARGET_NAME}' produced an unexpected secondary diagnostic:\n"
            "  ${UNEXPECTED_TEXT}\n"
            "Compiler output:\n${build_output}"
        )
    endif()
endif()

message(STATUS
    "Compile-fail target '${TARGET_NAME}' produced the expected diagnostic: "
    "${EXPECTED_TEXT}"
)
