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

install(
    TARGETS "pmda${PROJECT_NAME}" "pmda${PROJECT_NAME}-noboost"
    RUNTIME DESTINATION bin
)

install(
    FILES "${PROJECT_NAME}.cpp"
    DESTINATION share/doc/pcp-cpp/examples
)

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

# Enabled code coverage instrumentation, if supported by the compiler.
check_cxx_compiler_flag(-ftest-coverage HAVE_TEST_COVERAGE)
if (HAVE_TEST_COVERAGE)
    # Remove any optimisation flags for most accurate coverage reporting.
    string(REGEX REPLACE "(^| )-O[^ ]*( *|$)" "\\1" CMAKE_CXX_FLAGS         ${CMAKE_CXX_FLAGS})
    string(REGEX REPLACE "(^| )-O[^ ]*( *|$)" "\\1" CMAKE_CXX_FLAGS_DEBUG   ${CMAKE_CXX_FLAGS_DEBUG})
    string(REGEX REPLACE "(^| )-O[^ ]*( *|$)" "\\1" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
    # Enable gcov code instrumentation for code coverage reporting.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -O0")
endif()
