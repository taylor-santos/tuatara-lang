//
// Created by taylor-santos on 12/30/2021 at 20:28.
//

#include "doctest/doctest.h"

#include <sstream>
#include "scanner.hpp"
#include "parser.hpp"

class ScannerMock : public yy::Scanner {
public:
    ScannerMock(bool &did_scan, std::istream &is, std::ostream &os)
        : yy::Scanner("mock scanner", is, os)
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
        auto oss     = std::ostringstream();
        auto scanner = yy::Scanner("test", iss, oss);
        WHEN("a parser is constructed") {
            THEN("it should not throw an exception") {
                CHECK_NOTHROW(auto parser = yy::Parser(scanner));
            }
        }
    }
}

TEST_CASE("parser parse method invokes scanner scan method") {
    GIVEN("a parser constructed from a mock scanner") {
        auto iss      = std::istringstream();
        auto oss      = std::ostringstream();
        bool did_scan = false;
        auto scanner  = ScannerMock(did_scan, iss, oss);
        auto parser   = yy::Parser(scanner);
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
    auto oss     = std::ostringstream();
    auto scanner = yy::Scanner("test", iss, oss);
    auto parser  = yy::Parser(scanner);
    GIVEN("an input containing an unrecognized character") {
        iss << "\200";
        WHEN("the parser's parse method is invoked") {
            auto result = parser.parse();
            THEN("it should return a non-zero error code") {
                CHECK(result != 0);
                AND_THEN("the output stream should contain an error message") {
                    CHECK(oss.str() == "test:1:1 stray '\\200' in program\n");
                }
            }
        }
    }
}
