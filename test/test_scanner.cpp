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
        WHEN("a scanner is constructed") {
            THEN("it should not throw an exception") {
                CHECK_NOTHROW(auto scanner = yy::Scanner("test", iss));
            }
        }
    }
}

TEST_CASE("scanner's scan method returns EOF on empty input") {
    GIVEN("a scanner constructed from an empty stream") {
        auto iss     = std::istringstream();
        auto scanner = yy::Scanner("test", iss);
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
    auto scanner = yy::Scanner("test", iss);
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
    auto scanner = yy::Scanner("test", iss);
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
    auto scanner = yy::Scanner("test", iss);

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
    auto scanner = yy::Scanner("test", iss);

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
                    THEN("it should return an int token") {
                        REQUIRE(symbol.kind() == yy::Parser::symbol_kind::S_INT);
                        auto var = symbol.value.as<int_variant>();
                        AND_THEN("the int should be unsigned and have 64 bits") {
                            REQUIRE(std::holds_alternative<uint64_t>(var));
                            auto u64 = std::get<uint64_t>(var);
                            AND_THEN("the token value should match the input") {
                                CHECK(u64 == val);
                            }
                        }
                    }
                }
            }
        }
    }
}

TEST_CASE("int out of range should throw") {
    auto iss     = std::stringstream();
    auto scanner = yy::Scanner("test", iss);

    auto cases = std::vector{
        std::pair{
            "18_446_744_073_709_551_616",
            "syntax error, 18446744073709551616 out of range of U64 (0 to 18446744073709551615)"},
        {"18_446_744_073_709_551_616u64",
         "syntax error, 18446744073709551616 out of range of U64 (0 to 18446744073709551615)"},
        {"4_294_967_296u32", "syntax error, 4294967296 out of range of U32 (0 to 4294967295)"},
        {"65_536u16", "syntax error, 65536 out of range of U16 (0 to 65535)"},
        {"256u8", "syntax error, 256 out of range of U8 (0 to 255)"},
    };
    for (auto &[input, output] : cases) {
        SUBCASE(input) {
            GIVEN("an int that would overflow") {
                iss << input;
                WHEN("the scanner's scan method is invoked") {
                    THEN("it should throw an exception") {
                        CHECK_THROWS_WITH_AS(scanner.scan(), output, yy::Parser::syntax_error);
                    }
                }
            }
        }
    }
}

TEST_CASE("int in range should not throw") {
    auto iss     = std::stringstream();
    auto scanner = yy::Scanner("test", iss);

    auto cases = std::vector{
        std::pair{"18_446_744_073_709_551_615", 18446744073709551615ull},
        {"18_446_744_073_709_551_615u64", 18446744073709551615ull},
        {"4_294_967_295u32", 4294967295ull},
        {"65_535u16", 65535ull},
        {"255u8", 255ull},
    };
    for (auto &[input, output] : cases) {
        SUBCASE(input) {
            GIVEN("an int that would overflow") {
                iss << input;
                WHEN("the scanner's scan method is invoked") {
                    THEN("it should not throw an exception") {
                        auto token = scanner.scan();
                        AND_THEN("the token should have type int") {
                            REQUIRE(token.kind() == yy::Parser::symbol_kind_type::S_INT);
                            auto var = token.value.as<int_variant>();
                            AND_THEN("it should have the correct value") {
                                auto correct = output;
                                std::visit([&](auto &&val) { CHECK(val == correct); }, var);
                            }
                        }
                    }
                }
            }
        }
    }
}

TEST_CASE("valid identifiers") {
    auto iss     = std::stringstream();
    auto scanner = yy::Scanner("test", iss);

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
    auto scanner = yy::Scanner("test", iss);

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
