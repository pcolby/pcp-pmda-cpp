cmake_minimum_required (VERSION 2.6)

project(simple)

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

# GNUInstallDirs was added in CMake 2.8.5.
include(GNUInstallDirs OPTIONAL RESULT_VARIABLE HAVE_GNUINSTALLDIRS)
if (NOT HAVE_GNUINSTALLDIRS)
    # Set CMAKE_INSTALL_LOCALSTATEDIR the way GNUInstallDirs would have.
    set(CMAKE_INSTALL_LOCALSTATEDIR var CACHE STRING "modifiable single-machine data (var)")
endif()

install(
    TARGETS "pmda${PROJECT_NAME}" "pmda${PROJECT_NAME}-noboost"
    RUNTIME DESTINATION ${CMAKE_INSTALL_LOCALSTATEDIR}/lib/pcp/pmdas/pcp-pmda-cpp-examples
)

install(
    FILES "${PROJECT_NAME}.cpp"
    DESTINATION ${CMAKE_INSTALL_LOCALSTATEDIR}/lib/pcp/pmdas/pcp-pmda-cpp-examples
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
