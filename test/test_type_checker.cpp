//
// Created by taylor-santos on 2/12/2022 at 15:43.
//

#include "doctest/doctest.h"

#include <sstream>
#include <algorithm>

#include "type/type_checker.hpp"
#include "driver.hpp"

TEST_SUITE_BEGIN("TypeChecker");

TEST_CASE("uninitialized variable") {
    auto input    = std::stringstream();
    auto output   = std::stringstream();
    auto driver   = Driver(input, output);
    auto filename = std::string("test");

    input << R"(
    foo :: U64;
    bar := foo;
    )";

    driver.parse(&filename);
    auto ctx = driver.type_check();
    CHECK(ctx.did_fail());
    auto &errors = driver.errors();
    REQUIRE(errors.size() == 1);
    auto &msg = errors[0].message_;
    REQUIRE(msg.size() == 2);
    CHECK(msg[1].message_ == "`foo` used before initialization");
    auto &details = errors[0].details_;
    REQUIRE(details.size() == 2);
    CHECK(details[0].message_ == "`foo` declared without being initialized here");
    CHECK(details[1].message_ == "`foo` used here");
}

TEST_CASE("moved variable") {
    auto input    = std::stringstream();
    auto output   = std::stringstream();
    auto driver   = Driver(input, output);
    auto filename = std::string("test");

    input << R"(
    foo := 123;
    bar := foo;
    baz := foo;
    )";

    driver.parse(&filename);
    auto ctx = driver.type_check();
    CHECK(ctx.did_fail());
    auto &errors = driver.errors();
    REQUIRE(errors.size() == 1);
    auto &msg = errors[0].message_;
    REQUIRE(msg.size() == 2);
    CHECK(msg[1].message_ == "`foo` used after being moved");
    auto &details = errors[0].details_;
    REQUIRE(details.size() == 3);
    CHECK(details[0].message_ == "value assigned to `foo` here");
    CHECK(details[1].message_ == "value moved out of `foo` here");
    CHECK(details[2].message_ == "`foo` used here after move");
}

TEST_CASE("print symbol table") {
    auto input    = std::stringstream();
    auto output   = std::stringstream();
    auto driver   = Driver(input, output);
    auto filename = std::string("test");

    input << R"(
    foo := 123;
    bar := foo;
    baz := foo;
    )";
    driver.parse(&filename);
    auto ctx   = driver.type_check();
    auto print = std::stringstream();
    ctx.print_symbols(print);
    CHECK(
        print.str() == "Symbol Type\n"
                       "baz    [object [class U64]]\n"
                       "bar    [object [class U64]]\n"
                       "foo    [object [class U64]] (uninitialized)\n");
}
