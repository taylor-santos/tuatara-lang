//
// Created by taylor-santos on 12/30/2021 at 19:34.
//

#include "doctest/doctest.h"

#include <sstream>
#include "scanner.hpp"

TEST_SUITE_BEGIN("Scanner");

TEST_CASE("scanner constructs without exception") {
    GIVEN("an input and output stream") {
        auto iss = std::istringstream();
        auto oss = std::ostringstream();
        WHEN("a scanner is constructed") {
            THEN("it should not throw an exception") {
                CHECK_NOTHROW(auto scanner = yy::Scanner("test", iss, oss));
            }
        }
    }
}

TEST_CASE("scanner's scan method returns EOF on empty input") {
    GIVEN("a scanner constructed from an empty stream") {
        auto iss     = std::istringstream();
        auto oss     = std::ostringstream();
        auto scanner = yy::Scanner("test", iss, oss);
        WHEN("the scanner's scan method is invoked") {
            auto symbol = scanner.scan();
            THEN("it should return an EOF token") {
                CHECK(symbol.kind() == yy::Parser::symbol_kind_type::S_YYEOF);
            }
        }
    }
}

TEST_CASE("scanner should output an error when lexing an unrecognized character") {
    auto iss     = std::stringstream();
    auto oss     = std::ostringstream();
    auto scanner = yy::Scanner("test", iss, oss);
    GIVEN("an input containing an unrecognized character") {
        iss << "\200";
        WHEN("the scanner's scan method is invoked") {
            THEN("an error message should be thrown") {
                CHECK_THROWS_WITH_AS(
                    scanner.scan(),
                    "stray '\\200' in program",
                    yy::Parser::syntax_error);
            }
        }
    }
}
