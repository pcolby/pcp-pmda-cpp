cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(functional-tests)

# Add a custom "check-functional" target.
add_custom_target(
    check-functional
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test.sh
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Functional tests require a whole lot of other targets.
add_dependencies(
    check-functional
    pmdasimple    pmdasimple-noboost    pmda_simple
    pmdasimplecpu pmdasimplecpu-noboost pmda_simplecpu
    pmdatrivial   pmdatrivial-noboost   pmda_trivial
)
