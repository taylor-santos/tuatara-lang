//
// Created by taylor-santos on 2/14/2022 at 12:30.
//

#include "doctest/doctest.h"

#include "printer.hpp"
#include "test_util.hpp"

#include <sstream>

DOCTEST_TEST_SUITE_BEGIN("Printer");

TEST_CASE("printer") {
    auto lines = std::vector<std::string>{
        "foo",
        "bar baz",
        "12345 67890",
    };
    auto file    = std::string("filename");
    auto out     = std::stringstream();
    auto printer = print::Printer(lines, out);

    REQUIRE(out.rdbuf() != std::cout.rdbuf());
    REQUIRE(out.rdbuf() != std::cerr.rdbuf());

    GIVEN("a message with no details") {
        printer.error({{"foo", rang::fg::red}}, POS(&file, 1, 1), {});

        THEN("just the message should be printed") {
            CHECK(
                out.str() == "foo\n"
                             "  --> filename:1:1\n");
        }
    }
}
