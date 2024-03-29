# Add non-main source files here
set(CORE_SOURCES
        ast/ast.cpp
        ast/expression.cpp
        ast/definition.cpp
        ast/value_definition.cpp
        ast/type_definition.cpp
        ast/simple_expression.cpp
        ast/literal.cpp
        ast/u64.cpp
        ast/variable.cpp
        ast/type.cpp
        ast/object_type.cpp
        ast/error.cpp
        type/type_checker.cpp
        type/type.cpp
        type/error.cpp
        type/class.cpp
        type/object.cpp
        driver.cpp
        json.cpp
        line_stream.cpp
        printer.cpp)

set(CMAKE_FIND_USE_PACKAGE_ROOT_PATH ON)
find_package(FLEX 2.6.4
        REQUIRED)
find_package(BISON 3.7.4
        REQUIRED)

flex_target(SCANNER
        ${CMAKE_CURRENT_SOURCE_DIR}/scanner.l
        ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp
        )
bison_target(PARSER
        ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
        ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
        COMPILE_FLAGS "-Wcounterexamples"
        VERBOSE
        )
add_flex_bison_dependency(SCANNER PARSER)

add_library(core
        ${BISON_PARSER_OUTPUTS}
        ${FLEX_SCANNER_OUTPUTS}
        ${CORE_SOURCES})

target_include_directories(core
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
        PUBLIC include
        PUBLIC ${FLEXLEXER_INCLUDE_DIR})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    # g++8 doesn't automatically link the std::filesystem library, so -lstdc++fs must be explicitly linked
    target_link_libraries(core
            PUBLIC stdc++fs)
endif ()
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10)
    # clang++-9 warns by default when using designated initializers, which are standard in C++20
    target_compile_options(core
            PUBLIC -Wno-c99-extensions)
endif ()

if (MSVC)
    target_compile_options(core
            PUBLIC /Zc:__cplusplus) # Bison checks the __cplusplus macro to enable std::move
    target_compile_definitions(core
            PRIVATE _CRT_SECURE_NO_WARNINGS)
endif ()
