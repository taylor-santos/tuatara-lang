include(CTest)
enable_testing()

set(TEST_NAME
        ${PROJECT_NAME}_tests)

set(TEST_SOURCES
        test_scanner.cpp test_parser.cpp)

add_executable(${TEST_NAME}
        ${TEST_SOURCES}
        test_main.cpp)

target_link_libraries(${TEST_NAME}
        PRIVATE core
        PRIVATE doctest)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(${TEST_NAME}
        ADD_LABELS 1) # https://github.com/onqtam/doctest/pull/490
