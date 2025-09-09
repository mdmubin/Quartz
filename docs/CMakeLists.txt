project(QuartzDocs)

find_package(Doxygen)

if (DOXYGEN_FOUND)
    set(QZ_DOXYGEN_INPUT "${CMAKE_SOURCE_DIR}/include")
    set(QZ_DOXYGEN_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}")

    # process the configuration file.
    configure_file(${CMAKE_CURRENT_LIST_DIR}/doxyfile ${CMAKE_CURRENT_BINARY_DIR}/doxyfile @ONLY)
    set(QZ_DOXYGEN_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)

    add_custom_target(qzdocs
        COMMAND
            ${DOXYGEN_EXECUTABLE} ${QZ_DOXYGEN_CONFIG}
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        BYPRODUCTS
            ${CMAKE_CURRENT_LIST_DIR}/html/index.html
        COMMENT
            "Generating API documentations."
        VERBATIM
    )
    add_custom_command(
        TARGET qzdocs
        POST_BUILD COMMAND
            echo Documentation files successfully generated! You can view them at ${QZ_DOXYGEN_OUTPUT}/html/index.html
    )
else ()
    message("Doxygen was not found. Skipping building documentation.")
endif ()
