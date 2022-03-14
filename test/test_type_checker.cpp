//
// Created by taylor-santos on 2/12/2022 at 15:43.
//

#include "doctest/doctest.h"
#include "driver.hpp"
#include "printer.hpp"

#include <sstream>
#include <algorithm>

TEST_SUITE_BEGIN("TypeChecker");

TEST_CASE("uninitialized variable") {
    auto input    = std::stringstream();
    auto output   = std::stringstream();
    auto driver   = Driver(input);
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
    auto msg = std::stringstream();
    errors[0].print({"", "    foo :: U64;", "    bar := foo;"}, msg);
    CHECK(
        msg.str() == "error: variable `foo` used before initialization\n"
                     "  ╭─[test:3:12]\n"
                     "2 │     foo :: U64;\n"
                     "  ·     ─── `foo` declared without being initialized here\n"
                     "3 │     bar := foo;\n"
                     "  ·            ─── `foo` used here\n"
                     "──╯\n");
}

TEST_CASE("moved variable") {
    auto input    = std::stringstream();
    auto output   = std::stringstream();
    auto driver   = Driver(input);
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
    auto msg = std::stringstream();
    errors[0].print({"", "    foo := 123;", "    bar := foo;", "    baz := foo;"}, msg);
    CHECK(
        msg.str() == "error: variable `foo` used after being moved\n"
                     "  ╭─[test:4:12]\n"
                     "2 │     foo := 123;\n"
                     "  ·     ─── value assigned to `foo` here\n"
                     "3 │     bar := foo;\n"
                     "  ·            ─── value moved out of `foo` here\n"
                     "4 │     baz := foo;\n"
                     "  ·            ─── `foo` used here after move\n"
                     "──╯\n");
}

TEST_CASE("print symbol table") {
    auto input    = std::stringstream();
    auto output   = std::stringstream();
    auto driver   = Driver(input);
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
                       "bar    [object [class U64]]\n"
                       "baz    [object [class U64]]\n"
                       "foo    [object [class U64]] (uninitialized)\n");
}
