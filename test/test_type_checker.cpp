//
// Created by taylor-santos on 2/12/2022 at 15:43.
//

#include "doctest/doctest.h"

#include <sstream>
#include <algorithm>

#include "scanner.hpp"
#include "parser.hpp"
#include "type/type_checker.hpp"

TEST_CASE("uninitialized variable") {
    auto code = R"(
    foo :: U64;
    bar := foo;
    )";

    auto input   = std::istringstream(code);
    auto lines   = LineStream(input);
    auto scanner = yy::Scanner("test", lines);
    auto out     = std::ostringstream();
    auto ast     = std::vector<std::unique_ptr<AST::Expression>>();
    auto failed  = false;
    auto parser  = yy::Parser(scanner, out, lines.lines(), ast, failed);
    auto ctx     = TypeChecker::Context(out);
    parser.parse();

    std::for_each(ast.begin(), ast.end(), [&ctx](auto &expr) { (void)expr->get_type(ctx); });
    CHECK(ctx.did_fail());
    CHECK(out.str() == "test:3.12-14: `foo` used before initialization\n");
}

TEST_CASE("moved variable") {
    auto code = R"(
    foo := 123;
    bar := foo;
    baz := foo;
    )";

    auto input   = std::istringstream(code);
    auto lines   = LineStream(input);
    auto scanner = yy::Scanner("test", lines);
    auto out     = std::ostringstream();
    auto ast     = std::vector<std::unique_ptr<AST::Expression>>();
    auto failed  = false;
    auto parser  = yy::Parser(scanner, out, lines.lines(), ast, failed);
    auto ctx     = TypeChecker::Context(out);
    parser.parse();

    std::for_each(ast.begin(), ast.end(), [&ctx](auto &expr) { expr->get_type(ctx); });
    CHECK(ctx.did_fail());
    CHECK(out.str() == "test:4.12-14: `foo` used after being moved\n");
}
