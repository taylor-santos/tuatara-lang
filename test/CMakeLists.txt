include(CTest)
enable_testing()

set(TEST_NAME
        ${PROJECT_NAME}_tests)

set(TEST_SOURCES
        test_scanner.cpp
        test_parser.cpp
        test_line_stream.cpp
        test_type_checker.cpp
        test_printer.cpp
        ast/test_u64.cpp
        ast/test_error.cpp
        ast/test_object_type.cpp
        ast/test_type_definition.cpp
        ast/test_value_definition.cpp
        ast/test_variable.cpp
        type/test_class.cpp
        type/test_error.cpp
        type/test_object.cpp)

add_executable(${TEST_NAME}
        ${TEST_SOURCES}
        test_main.cpp)

target_include_directories(${TEST_NAME}
        PRIVATE .)

target_link_libraries(${TEST_NAME}
        PRIVATE core
        PRIVATE doctest)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(${TEST_NAME}
        ADD_LABELS 1) # https://github.com/onqtam/doctest/pull/490
