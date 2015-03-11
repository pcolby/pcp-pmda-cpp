cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Enable ExternalProject CMake module
include(ExternalProject)

# Set default ExternalProject root directory
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/extern)

# Add gtest
ExternalProject_Add(
    googletest
    SVN_REPOSITORY https://googletest.googlecode.com/svn/trunk/
    UPDATE_COMMAND "" # Don't update the svn checkout every time we build.
    TIMEOUT 10
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -Dgtest_force_shared_crt=ON
    INSTALL_COMMAND "" # Disable install step
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)

# Add googletest headers to the include path.
ExternalProject_Get_Property(googletest source_dir)
include_directories(${source_dir}/include)

# Add compiler flag for MSVC 2012
if(MSVC_VERSION EQUAL 1700)
    add_definitions(-D_VARIADIC_MAX=10)
endif()
