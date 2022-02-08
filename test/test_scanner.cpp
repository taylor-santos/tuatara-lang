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
        auto iss   = std::istringstream();
        auto lines = LineStream(iss);
        WHEN("a scanner is constructed") {
            THEN("it should not throw an exception") {
                CHECK_NOTHROW(auto scanner = yy::Scanner("test", lines));
            }
        }
    }
}

TEST_CASE("scanner's scan method returns EOF on empty input") {
    GIVEN("a scanner constructed from an empty stream") {
        auto iss     = std::istringstream();
        auto lines   = LineStream(iss);
        auto scanner = yy::Scanner("test", lines);
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
    auto lines   = LineStream(iss);
    auto scanner = yy::Scanner("test", lines);
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
    auto lines   = LineStream(iss);
    auto scanner = yy::Scanner("test", lines);
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

TEST_CASE("line numbers should be properly tracked") {
    auto iss     = std::stringstream();
    auto lines   = LineStream(iss);
    auto scanner = yy::Scanner("test", lines);

    std::list cases{
        std::pair{"CR", "foo\rbar\r\rbaz"},
        {"LF", "foo\nbar\n\nbaz"},
        {"CRLF", "foo\r\nbar\r\n\r\nbaz"},
    };

    for (auto &[name, input] : cases) {
        SUBCASE(name) {
            GIVEN("an input with line breaks") {
                iss << input;
                WHEN("the scanner's scan method is invoked") {
                    auto foo = scanner.scan();
                    auto bar = scanner.scan();
                    auto baz = scanner.scan();
                    THEN("the proper line numbers are recorded") {
                        CHECK(foo.location.begin.line == 1);
                        CHECK(foo.location.begin.column == 1);
                        CHECK(foo.location.end.line == 1);
                        CHECK(foo.location.end.column == 4);
                        CHECK(bar.location.begin.line == 2);
                        CHECK(bar.location.begin.column == 1);
                        CHECK(bar.location.end.line == 2);
                        CHECK(bar.location.end.column == 4);
                        CHECK(baz.location.begin.line == 4);
                        CHECK(baz.location.begin.column == 1);
                        CHECK(baz.location.end.line == 4);
                        CHECK(baz.location.end.column == 4);
                    }
                }
            }
        }
    }
}

TEST_CASE("u64 ints should be tokenized correctly") {
    auto iss     = std::stringstream();
    auto lines   = LineStream(iss);
    auto scanner = yy::Scanner("test", lines);

    // Cases
    std::list cases{
        std::pair{"0", 0ull},
        {"1", 1ull},
        {"23", 23ull},
        {"2_3", 23ull},
        {"1234", 1234ull},
        {"1_234", 1234ull},
        {"1_2__3_4", 1234ull},
        {"9876543210", 9876543210ull},
        {"9_876_543_210", 9876543210ull},
        {"18446744073709551615", 18446744073709551615ull},
        {"18_446_744_073_709_551_615", 18446744073709551615ull},
    };

    for (auto &[str, val] : cases) {
        SUBCASE(str) {
            GIVEN("an int literal") {
                iss << str;
                WHEN("the scanner's scan method is invoked") {
                    auto symbol = scanner.scan();
                    THEN("it should return a u64 token") {
                        REQUIRE(symbol.kind() == yy::Parser::symbol_kind::S_U64);
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
    auto lines   = LineStream(iss);
    auto scanner = yy::Scanner("test", lines);
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

TEST_CASE("valid identifiers") {
    auto iss     = std::stringstream();
    auto lines   = LineStream(iss);
    auto scanner = yy::Scanner("test", lines);

    // Cases
    std::list cases{
        "a",
        "abc",
        "aBcDe",
        "a123",
        "b_",
        "c_123a",
        "xY_z",
        "aBC____",
    };

    for (const auto &ident : cases) {
        SUBCASE(ident) {
            GIVEN("an identifier") {
                iss << ident;
                WHEN("the scanner's scan method is invoked") {
                    auto symbol = scanner.scan();
                    THEN("it should return an ident token") {
                        REQUIRE(symbol.kind() == yy::Parser::symbol_kind::S_IDENT);
                        auto str = symbol.value.as<std::string>();
                        AND_THEN("the token value should match the input") {
                            CHECK(str == ident);
                        }
                    }
                }
            }
        }
    }
}

TEST_CASE("valid type names") {
    auto iss     = std::stringstream();
    auto lines   = LineStream(iss);
    auto scanner = yy::Scanner("test", lines);

    // Cases
    std::list cases{
        "Int",
        "Bool",
        "U64",
        "MyClass",
        "Maybe",
        "Some",
        "None",
        "Snake_Case",
        "ABC123",
    };

    for (const auto &ident : cases) {
        SUBCASE(ident) {
            GIVEN("a type name") {
                iss << ident;
                WHEN("the scanner's scan method is invoked") {
                    auto symbol = scanner.scan();
                    THEN("it should return a type name token") {
                        REQUIRE(symbol.kind() == yy::Parser::symbol_kind::S_TYPENAME);
                        auto str = symbol.value.as<std::string>();
                        AND_THEN("the token value should match the input") {
                            CHECK(str == ident);
                        }
                    }
                }
            }
        }
    }
}
