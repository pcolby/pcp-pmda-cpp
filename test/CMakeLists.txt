cmake_minimum_required(VERSION 2.6)
add_subdirectory(functional)
add_subdirectory(unit)

# Add a custom "check" target a la GNU's Standard Targets for Users.
add_custom_target(check COMMAND test DEPENDS check-functional check-unit)

# Add a custom "coverage" target, if supported.
find_program(LCOV lcov)
if (LCOV)
    add_custom_command(
        OUTPUT coverage.info
        COMMAND ${LCOV} --capture --directory ${CMAKE_BINARY_DIR} --output-file coverage.info --quiet
        COMMAND ${LCOV} --remove coverage.info '/usr/**' 'example/*' 'test/*' --output-file coverage.info --quiet
    )
    find_program(GENHTML genhtml)
    if (GENHTML)
        add_custom_target(
            coverage
            COMMAND ${GENHTML} --output-directory coverage_html --quiet coverage.info --title 'PMDA++ Unit Tests'
            DEPENDS coverage.info
        )
    endif()
endif()
