cmake_minimum_required (VERSION 2.6)

project(trivial)

include_directories ("${PROJECT_SOURCE_DIR}/../../include")

add_executable("pmda${PROJECT_NAME}" "${PROJECT_NAME}.cpp")
add_executable("pmda${PROJECT_NAME}-noboost" "${PROJECT_NAME}.cpp")
add_library("pmda_${PROJECT_NAME}" SHARED "${PROJECT_NAME}.cpp")

find_package(Boost COMPONENTS program_options REQUIRED)

target_link_libraries("pmda${PROJECT_NAME}" pcp pcp_pmda ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries("pmda${PROJECT_NAME}-noboost" pcp pcp_pmda)
target_link_libraries("pmda_${PROJECT_NAME}" pcp pcp_pmda ${Boost_PROGRAM_OPTIONS_LIBRARY})

set_property(
   TARGET "pmda${PROJECT_NAME}-noboost"
   PROPERTY COMPILE_DEFINITIONS PCP_CPP_NO_BOOST
)

# Detect the PCP environment.
find_program(
    PCP_PMCONFIG_EXECUTABLE
    NAMES pmconfig
    PATHS /usr/lib/pcp/bin /usr/libexec/pcp/bin
)
if (PCP_PMCONFIG_EXECUTABLE)
    message(STATUS "Found pmconfig: ${PCP_PMCONFIG_EXECUTABLE}")
    execute_process(
        COMMAND ${PCP_PMCONFIG_EXECUTABLE} PCP_BINADM_DIR PCP_PMDAS_DIR
        OUTPUT_VARIABLE PCP_PMCONFIG_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    STRING(REPLACE "\n" ";" PCP_PMCONFIG_OUTPUT ${PCP_PMCONFIG_OUTPUT})
    foreach (PCP_PMCONFIG_LINE ${PCP_PMCONFIG_OUTPUT})
        string(REGEX REPLACE "=.*$" "" PCP_PMCONFIG_NAME  ${PCP_PMCONFIG_LINE})
        string(REGEX REPLACE "^[A-Z_]+=" "" PCP_PMCONFIG_VALUE ${PCP_PMCONFIG_LINE})
        set(${PCP_PMCONFIG_NAME} ${PCP_PMCONFIG_VALUE})
        message(STATUS "  ${PCP_PMCONFIG_NAME}: ${${PCP_PMCONFIG_NAME}}")
    endforeach ()
endif ()
find_program(PCP_NEWHELP_EXECUTABLE NAMES newhelp HINTS ${PCP_BINADM_DIR})
if (PCP_NEWHELP_EXECUTABLE)
    message(STATUS "Found newhelp: ${PCP_NEWHELP_EXECUTABLE}")
endif ()

# Setup the default installation directory.
if (PCP_PMDAS_DIR)
    set(DEFAULT_PMDAS_INSTALL_DIR ${PCP_PMDAS_DIR}/pcp-pmda-cpp-examples/pmdas)
    set(
        PMDAS_INSTALL_DIR ${DEFAULT_PMDAS_INSTALL_DIR}
        CACHE PATH "directory to install PMDAs to (${DEFAULT_PMDAS_INSTALL_DIR})"
    )
endif()

# Create an install target, but only if the PMDAS_INSTALL_DIR directory was set.
if (PMDAS_INSTALL_DIR)
    SET(PMDA_INSTALL_DIR ${PMDAS_INSTALL_DIR}/${PROJECT_NAME})
    # Install the PMDA binaries.
    install(
        TARGETS "pmda${PROJECT_NAME}" "pmda${PROJECT_NAME}-noboost"
        RUNTIME DESTINATION ${PMDA_INSTALL_DIR}
    )
    # Export the PMDA's support files (domain, help, pmns, etc).
    install(
        CODE "
            message(\"-- Exporting:  \$ENV{DESTDIR}${PMDA_INSTALL_DIR}/{domain.h,help,pmns,root}\")
            execute_process(
                COMMAND \$ENV{DESTDIR}${PMDA_INSTALL_DIR}/pmda${PROJECT_NAME} --export-all
                WORKING_DIRECTORY \$ENV{DESTDIR}${PMDA_INSTALL_DIR}
                RESULT_VARIABLE export_result
            )
            if (NOT \${export_result} EQUAL 0)
                message(FATAL_ERROR \"Failed to export PMDA support files: \${export_result}\")
            endif ()
            if (EXISTS ${PCP_NEWHELP_EXECUTABLE})
                message(\"-- Generating: \$ENV{DESTDIR}${PMDA_INSTALL_DIR}/help.{dir,pag}\")
                execute_process(
                    COMMAND ${PCP_NEWHELP_EXECUTABLE} -n root help
                    WORKING_DIRECTORY \$ENV{DESTDIR}${PMDA_INSTALL_DIR}
                    RESULT_VARIABLE export_result
                )
                if (NOT \${export_result} EQUAL 0)
                    message(FATAL_ERROR \"Failed to generate help database: \${export_result}\")
                endif ()
            endif ()
        "
    )
    # Install the PMDA source and registration scripts.
    configure_file(Install.in ${CMAKE_CURRENT_SOURCE_DIR}/Install)
    configure_file(Remove.in  ${CMAKE_CURRENT_SOURCE_DIR}/Remove)
    install(PROGRAMS Install Remove DESTINATION ${PMDA_INSTALL_DIR})
    install(FILES ${PROJECT_NAME}.cpp DESTINATION ${PMDA_INSTALL_DIR})
endif()

# Enable (and stop on) compiler warnings.
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wall   HAVE_WALL)
check_cxx_compiler_flag(-Werror HAVE_WERROR)
check_cxx_compiler_flag(-Wextra HAVE_WEXTRA)
if (HAVE_WALL)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()
if (HAVE_WERROR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()
if (HAVE_WEXTRA)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
endif()

# Enable test coverage reporting.
check_cxx_compiler_flag(-ftest-coverage HAVE_TEST_COVERAGE)
if (HAVE_TEST_COVERAGE)
    # Remove optimisation flags for accurate coverage reporting, but not
    # if source fortification is enabled, since: "_FORTIFY_SOURCE requires
    # compiling with optimization (-O)" (see /usr/include/features.h).
    if (NOT ${CMAKE_CXX_FLAGS} MATCHES "-D_FORTIFY_SOURCE=[^0]")
        string(REGEX REPLACE "(^| )-O[^ ]*( *|$)" "\\1" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    endif()
    if (NOT ${CMAKE_CXX_FLAGS_DEBUG} MATCHES "-D_FORTIFY_SOURCE=[^0]")
        string(REGEX REPLACE "(^| )-O[^ ]*( *|$)" "\\1" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
    endif()
    if (NOT ${CMAKE_CXX_FLAGS_RELEASE} MATCHES "-D_FORTIFY_SOURCE=[^0]")
        string(REGEX REPLACE "(^| )-O[^ ]*( *|$)" "\\1" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O0")
    endif()
    # Enable gcov code instrumentation for code coverage reporting.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fprofile-arcs -ftest-coverage")
endif()
