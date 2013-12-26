cmake_minimum_required (VERSION 2.6)

project(simple)

include_directories ("${PROJECT_SOURCE_DIR}/../../include")

add_executable("pmda${PROJECT_NAME}" "${PROJECT_NAME}.cpp")
add_executable("pmda${PROJECT_NAME}-noboost" "${PROJECT_NAME}.cpp")

target_link_libraries("pmda${PROJECT_NAME}" pcp pcp_pmda boost_program_options)
target_link_libraries("pmda${PROJECT_NAME}-noboost" pcp pcp_pmda)

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
    DESTINATION share/doc/pcp-cpp
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
