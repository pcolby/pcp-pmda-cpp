cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Functional)

# Add a custom "check-unit" target.
add_custom_target(
    check-functional
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test.sh
)

if (TARGET pmdasimple)
    add_dependencies(check-functional pmdasimple)
endif()

if (TARGET pmdasimplecpu)
    add_dependencies(check-functional pmdasimplecpu)
endif()

if (TARGET pmdatrivial)
    add_dependencies(check-functional pmdatrivial)
endif()
