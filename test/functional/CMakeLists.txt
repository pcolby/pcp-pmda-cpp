cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Functional)

# Add a custom "check-unit" target.
add_custom_target(
    check-functional
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test.sh
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Functional tests require a whole lot of other targets.
add_dependencies(
    check-functional
    pmdasimple    pmdasimple-boost    libpmda_simple
    pmdasimplecpu pmdasimplecpu-boost libpmda_simplecpu
    pmdatrivial   pmdatrivial-boost   libpmda_trivialx
)
