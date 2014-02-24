cmake_minimum_required(VERSION 2.6)
install(DIRECTORY pcp-cpp DESTINATION include)

# Add a doc target for generating doxygen documentation, if available.
find_package(Doxygen)
if (DOXYGEN_FOUND)
    configure_file(Doxyfile.in Doxyfile)
    add_custom_target(
        doc
        ${DOXYGEN_EXECUTABLE} Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
        SOURCES Doxyfile.in
    )
    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES api)
endif ()
