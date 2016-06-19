# Both FindGTest and ExternalProject were added in CMake 2.8.0.
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(unit-tests CXX)

# Add PMDA++ headers to the include path.
include_directories(${PROJECT_SOURCE_DIR}/../../include)

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

# Try the FindGTest module first.
find_package(GTest)
if (NOT GTEST_FOUND)
    message(STATUS "Resorting to external GTest project")
    # Include the external Google Test project, and mark as a dependency.
    include(GoogleTestExternal.cmake)
    add_dependencies(${PROJECT_NAME} googletest)
    # Mimic the FindGTest module's library variables.
    ExternalProject_Get_Property(googletest binary_dir)
    set(GTEST_LIBRARY        ${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(GTEST_MAIN_LIBRARY   ${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(GTEST_BOTH_LIBRARIES ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY})
endif()

# Specify the project's link libraries
if(NOT MSVC)
    set(pthread "-pthread")
endif()
find_package(Boost COMPONENTS program_options REQUIRED)
target_link_libraries(
    ${PROJECT_NAME}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${GTEST_BOTH_LIBRARIES}
    ${pthread}
)

# Add a custom "check-unit" target.
add_custom_target(check-unit COMMAND ${PROJECT_NAME} DEPENDS ${PROECT_NAME})

install(
    TARGETS ${PROECT_NAME}
    DESTINATION bin/pcp-pmda-cpp
)
