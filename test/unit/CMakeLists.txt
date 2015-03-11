cmake_minimum_required(VERSION 2.6)
project(unit-tests CXX)

# Set the build type if it isn't already
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(GoogleTestExternal.cmake)

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

# Enabled test coverage reporting.
check_cxx_compiler_flag(-ftest-coverage HAVE_TEST_COVERAGE)
if (HAVE_TEST_COVERAGE)
    # Remove any optimisation flags for most accurate coverage reporting.
    string(REGEX REPLACE "(^| )-O[^ ]*( *|$)" "\\1" CMAKE_CXX_FLAGS         ${CMAKE_CXX_FLAGS})
    string(REGEX REPLACE "(^| )-O[^ ]*( *|$)" "\\1" CMAKE_CXX_FLAGS_DEBUG   ${CMAKE_CXX_FLAGS_DEBUG})
    string(REGEX REPLACE "(^| )-O[^ ]*( *|$)" "\\1" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
    # Enable gcov code instrumentation for code coverage reporting.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -O0")
endif()

# Add test executable target
aux_source_directory(${PROJECT_SOURCE_DIR}/../../include pcp_pmda_cpp_sources)
add_executable(
    ${PROJECT_NAME}
    ${pcp_pmda_cpp_sources}
    ${PROJECT_SOURCE_DIR}/src/fake_libpcp.cpp
    ${PROJECT_SOURCE_DIR}/src/fake_libpcp-pmda.cpp
    ${PROJECT_SOURCE_DIR}/src/test_atom.cpp
    ${PROJECT_SOURCE_DIR}/src/test_cache.cpp
    ${PROJECT_SOURCE_DIR}/src/test_config.cpp
    ${PROJECT_SOURCE_DIR}/src/test_exception.cpp
    ${PROJECT_SOURCE_DIR}/src/test_instance_domain.cpp
    ${PROJECT_SOURCE_DIR}/src/test_metric_cluster.cpp
    ${PROJECT_SOURCE_DIR}/src/test_metric_description.cpp
    ${PROJECT_SOURCE_DIR}/src/test_metrics_description.cpp
    ${PROJECT_SOURCE_DIR}/src/test_pmda.cpp
    ${PROJECT_SOURCE_DIR}/src/test_types.cpp
    ${PROJECT_SOURCE_DIR}/src/test_units.cpp
)

# Create dependency of test on googletest
add_dependencies(${PROJECT_NAME} googletest)

# Specify test's link libraries
ExternalProject_Get_Property(googletest binary_dir)
if(NOT MSVC)
    set(pthread "-pthread")
endif()
find_package(Boost COMPONENTS program_options REQUIRED)
target_link_libraries(
    ${PROJECT_NAME}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    debug ${binary_dir}/debug/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}
    debug ${binary_dir}/debug/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}
    optimized ${binary_dir}/release/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}
    optimized ${binary_dir}/release/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${pthread}
)

# Add a custom "check-unit" target.
add_custom_target(check-unit COMMAND ${PROJECT_NAME} DEPENDS ${PROECT_NAME})

install(
    TARGETS ${PROECT_NAME}
    DESTINATION bin/pcp-pmda-cpp
)
