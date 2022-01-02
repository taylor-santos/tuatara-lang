//
// Created by taylor-santos on 12/30/2021 at 19:34.
//

#include "doctest/doctest.h"

#include <sstream>
#include <list>
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

TEST_CASE("white space should be ignored") {
    auto iss     = std::stringstream();
    auto oss     = std::ostringstream();
    auto scanner = yy::Scanner("test", iss, oss);
    GIVEN("an input containing only white space") {
        iss << " \t\n\r";
        WHEN("the scanner's scan method is invoked") {
            auto symbol = scanner.scan();
            THEN("it should return an EOF token") {
                CHECK(symbol.kind() == yy::Parser::symbol_kind_type::S_YYEOF);
            }
        }
    }
}

TEST_CASE("u64 ints should be tokenized correctly") {
    auto iss     = std::stringstream();
    auto oss     = std::ostringstream();
    auto scanner = yy::Scanner("test", iss, oss);

    // Cases
    std::list<std::pair<std::string, uint64_t>> cases{
        {"0", 0u},
        {"1", 1u},
        {"23", 23u},
        {"2_3", 23u},
        {"1234", 1234u},
        {"1_234", 1234u},
        {"1_2__3_4", 1234u},
        {"9876543210", 9876543210u},
        {"9_876_543_210", 9876543210u},
        {"18446744073709551615", 18446744073709551615u},
        {"18_446_744_073_709_551_615", 18446744073709551615u},
    };

    for (auto &[str, val] : cases) {
        SUBCASE(str.c_str()) {
            GIVEN("some an int literal") {
                iss << str;
                WHEN("the scanner's scan method is invoked") {
                    auto symbol = scanner.scan();
                    THEN("it should return a u64 token") {
                        CHECK(symbol.kind() == yy::Parser::symbol_kind::S_U64);
                        auto u64 = symbol.value.as<uint64_t>();
                        AND_THEN("the token value should match the input") {
                            CHECK(u64 == val);
                        }
                    }
                }
            }
        }
    }
}

TEST_CASE("int out of 64-bit range should throw") {
    auto iss     = std::stringstream();
    auto oss     = std::ostringstream();
    auto scanner = yy::Scanner("test", iss, oss);
    GIVEN("an int larger than the 64-bit limit") {
        iss << "18_446_744_073_709_551_616";
        WHEN("the scanner's scan method is invoked") {
            THEN("it should throw an exception") {
                CHECK_THROWS_WITH_AS(
                    scanner.scan(),
                    "syntax error, 18_446_744_073_709_551_616 out of range of u64",
                    yy::Parser::syntax_error);
            }
        }
    }
}
