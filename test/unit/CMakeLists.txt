cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Test CXX)

# Enable ExternalProject CMake module
include(ExternalProject)

# Set the build type if it isn't already
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set default ExternalProject root directory
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/extern)

# Add gtest
ExternalProject_Add(
    googletest
    SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk/
    UPDATE_COMMAND "" # Don't update the svn checkout every time we build.
    TIMEOUT 10
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=debug
               -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=release
               -Dgtest_force_shared_crt=ON
    INSTALL_COMMAND "" # Disable install step
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)

# Specify include dir
ExternalProject_Get_Property(googletest source_dir)
include_directories(
    ${source_dir}/include
    ${PROJECT_SOURCE_DIR}/../../include
)

# Add compiler flag for MSVC 2012
if(MSVC_VERSION EQUAL 1700)
    add_definitions(-D_VARIADIC_MAX=10)
endif()

# Add test executable target
aux_source_directory(${PROJECT_SOURCE_DIR}/../../include pcp_pmda_cpp_sources)
add_executable(test
    ${pcp_pmda_cpp_sources}
    ${PROJECT_SOURCE_DIR}/src/fake_libpcp.cpp
    ${PROJECT_SOURCE_DIR}/src/fake_libpcp-pmda.cpp
    ${PROJECT_SOURCE_DIR}/src/test_atom.cpp
    ${PROJECT_SOURCE_DIR}/src/test_exception.cpp
    ${PROJECT_SOURCE_DIR}/src/test_instance_domain.cpp
    ${PROJECT_SOURCE_DIR}/src/test_types.cpp
    ${PROJECT_SOURCE_DIR}/src/test_units.cpp
)

# Create dependency of test on googletest
add_dependencies(test googletest)

# Specify test's link libraries
ExternalProject_Get_Property(googletest binary_dir)
if(NOT MSVC)
    set(pthread "-pthread")
endif()
target_link_libraries(
    test
    debug ${binary_dir}/debug/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}
    debug ${binary_dir}/debug/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}
    optimized ${binary_dir}/release/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}
    optimized ${binary_dir}/release/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${pthread}
)

# Add a custom "check" target a la GNU's Standard Targets for Users.
add_custom_target(check COMMAND test DEPENDS test)
