macro(CONFIGURE_DOXYGEN_FILE PROJECT_DIR)

    find_package(Doxygen)
    option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

    if(BUILD_DOCUMENTATION)
        if(NOT DOXYGEN_FOUND)
            message(FATAL_ERROR "Doxygen is needed to generate documentation")
        endif()

        set(DOXY_INPUT_FILE ${PROJECT_DIR}/Doxyfile.in)
        set(DOXY_FILE ${PROJECT_DIR}/Doxyfile)
        
        message(${PROJECT_DIR})
        configure_file(${DOXY_INPUT_FILE} ${DOXY_FILE} @ONLY)

        add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXY_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    endif()

endmacro(CONFIGURE_DOXYGEN_FILE)