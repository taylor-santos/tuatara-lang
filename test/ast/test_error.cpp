//
// Created by taylor-santos on 2/10/2022 at 16:16.
//

#include "doctest/doctest.h"

#include "location.hh"
#include "ast/error.hpp"
#include "type/type_checker.hpp"
#include "type/error.hpp"
#include "printer.hpp"

TEST_SUITE_BEGIN("AST/Error");

TEST_CASE("get_type") {
    auto loc    = yy::location{};
    auto u64    = AST::Error(loc);
    auto errors = std::vector<print::Message>();
    auto ctx    = TypeChecker::Context(errors);

    auto &type = u64.get_type(ctx);
    auto  err  = dynamic_cast<const TypeChecker::Error *>(&type);
    REQUIRE(err != nullptr);
}
