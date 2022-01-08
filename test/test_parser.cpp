//
// Created by taylor-santos on 12/30/2021 at 20:28.
//

#include "doctest/doctest.h"

#include <sstream>
#include <list>

#include "scanner.hpp"
#include "parser.hpp"

class ScannerMock : public yy::Scanner {
public:
    ScannerMock(bool &did_scan, std::istream &is)
        : yy::Scanner("mock scanner", is)
        , did_scan_{did_scan} {}
    yy::Parser::symbol_type
    scan() override {
        did_scan_ = true;
        return yy::Parser::make_EOF(yy::location{});
    }

private:
    bool &did_scan_;
};

TEST_SUITE_BEGIN("Parser");

TEST_CASE("parser constructs without exception") {
    GIVEN("a valid scanner") {
        auto iss     = std::istringstream();
        auto scanner = yy::Scanner("test", iss);
        auto oss     = std::ostringstream();
        auto ast     = std::vector<std::unique_ptr<AST::Expression>>();
        auto failed  = false;

        WHEN("a parser is constructed") {
            THEN("it should not throw an exception") {
                CHECK_NOTHROW(auto parser = yy::Parser(scanner, oss, ast, failed));
            }
        }
    }
}

TEST_CASE("parser parse method invokes scanner scan method") {
    GIVEN("a parser constructed from a mock scanner") {
        auto iss      = std::istringstream();
        bool did_scan = false;
        auto scanner  = ScannerMock(did_scan, iss);
        auto oss      = std::ostringstream();
        auto ast      = std::vector<std::unique_ptr<AST::Expression>>();
        auto failed   = false;
        auto parser   = yy::Parser(scanner, oss, ast, failed);
        WHEN("the parser's parse method is invoked") {
            parser.parse();
            THEN("it should call the scanner's scan method") {
                CHECK_MESSAGE(did_scan, "scanner.scan() did not get called");
            }
        }
    }
}

TEST_CASE("a scanner syntax error should be handled internally") {
    auto iss     = std::stringstream();
    auto scanner = yy::Scanner("test", iss);
    auto oss     = std::ostringstream();
    auto ast     = std::vector<std::unique_ptr<AST::Expression>>();
    auto failed  = false;
    auto parser  = yy::Parser(scanner, oss, ast, failed);
    GIVEN("an input containing an unrecognized character") {
        iss << "\200";
        WHEN("the input is parsed") {
            auto result = parser.parse();
            THEN("it should return a non-zero error code") {
                CHECK(result != 0);
                AND_THEN("the error flag should be set") {
                    CHECK(failed == true);
                    AND_THEN("the output stream should contain an error message") {
                        CHECK(oss.str() == "test:1:1 stray '\\200' in program\n");
                    }
                }
            }
        }
    }
}

TEST_CASE("valid expressions") {
    auto iss     = std::stringstream();
    auto scanner = yy::Scanner("test", iss);
    auto oss     = std::ostringstream();
    auto ast     = std::vector<std::unique_ptr<AST::Expression>>();
    auto failed  = false;
    auto parser  = yy::Parser(scanner, oss, ast, failed);

    std::list<std::pair<const char *, std::vector<std::string>>> cases{
        {"foo :: Int;",
         {R"({"node":"type definition","location":{"begin":{"filename":"test","line":1,"column":1},"end":{"filename":"test","line":1,"column":11}},"name":"foo","name location":{"begin":{"filename":"test","line":1,"column":1},"end":{"filename":"test","line":1,"column":4}},"type":{"node":"object type","location":{"begin":{"filename":"test","line":1,"column":8},"end":{"filename":"test","line":1,"column":11}},"class":"Int","name location":{"begin":{"filename":"test","line":1,"column":8},"end":{"filename":"test","line":1,"column":11}}}})"}},
        {"foo := 123;",
         {R"({"node":"value definition","location":{"begin":{"filename":"test","line":1,"column":1},"end":{"filename":"test","line":1,"column":11}},"name":"foo","name location":{"begin":{"filename":"test","line":1,"column":1},"end":{"filename":"test","line":1,"column":4}},"value":{"node":"U64","location":{"begin":{"filename":"test","line":1,"column":8},"end":{"filename":"test","line":1,"column":11}},"value":123}})"}},
        {"foo := bar;",
         {R"({"node":"value definition","location":{"begin":{"filename":"test","line":1,"column":1},"end":{"filename":"test","line":1,"column":11}},"name":"foo","name location":{"begin":{"filename":"test","line":1,"column":1},"end":{"filename":"test","line":1,"column":4}},"value":{"node":"variable","location":{"begin":{"filename":"test","line":1,"column":8},"end":{"filename":"test","line":1,"column":11}},"name":"bar"}})"}},
    };

    for (auto &[input, json_outputs] : cases) {
        SUBCASE(input) {
            GIVEN("an input containing a valid expression") {
                iss << input;
                WHEN("the input is parsed") {
                    auto result = parser.parse();
                    THEN("the error flag should be false") {
                        CHECK(failed == false);
                        AND_THEN(
                            "the produced AST should have the expected number of expressions") {
                            auto n = json_outputs.size();
                            REQUIRE(ast.size() == n);
                            AND_THEN("the expression's json representation should match the "
                                     "expected output") {
                                for (size_t i = 0; i < n; i++) {
                                    std::ostringstream out;
                                    ast[i]->to_json(out);
                                    CHECK(out.str() == json_outputs[i]);
                                }
                                AND_THEN("the output stream should be empty") {
                                    CHECK(oss.str() == "");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

TEST_CASE("expression error handling") {
    auto iss     = std::stringstream();
    auto scanner = yy::Scanner("test", iss);
    auto oss     = std::ostringstream();
    auto ast     = std::vector<std::unique_ptr<AST::Expression>>();
    auto failed  = false;
    auto parser  = yy::Parser(scanner, oss, ast, failed);

    auto input        = "foo := Int;\nbar :: 123;";
    auto json_outputs = std::vector<std::string>{
        R"({"node":"error","location":{"begin":{"filename":"test","line":1,"column":1},"end":{"filename":"test","line":1,"column":11}}})",
        R"({"node":"error","location":{"begin":{"filename":"test","line":2,"column":1},"end":{"filename":"test","line":2,"column":11}}})",
    };

    GIVEN("an input with more than one syntax error") {
        iss << input;
        WHEN("the input is parsed") {
            auto result = parser.parse();
            THEN("an error code is returned") {
                CHECK(result != 0);
                CHECK(failed == true);
                AND_THEN("both errors are reported") {
                    CHECK(
                        oss.str() ==
                        "test:1:8 syntax error, unexpected type name, expecting + or - or "
                        "identifier or int literal\n"
                        "test:2:8 syntax error, unexpected int literal, expecting type name\n");
                    AND_THEN("the produced ast should match the expected output") {
                        auto n = json_outputs.size();
                        REQUIRE(ast.size() == n);
                        for (size_t i = 0; i < n; i++) {
                            std::ostringstream out;
                            ast[i]->to_json(out);
                            CHECK(json_outputs[i] == out.str());
                        }
                        AND_THEN("the error flag should be true") {
                            CHECK(failed == true);
                        }
                    }
                }
            }
        }
    }
}
