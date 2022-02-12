//
// Created by taylor-santos on 2/10/2022 at 16:16.
//

#include "doctest/doctest.h"

#include "location.hh"
#include "ast/error.hpp"
#include "type/context.hpp"
#include "type/error.hpp"

TEST_CASE("AST Error get_type") {
    auto loc = yy::location{};
    auto u64 = AST::Error(loc);
    auto ctx = TypeChecker::Context();

    auto &type = u64.get_type(ctx);
    auto  err  = dynamic_cast<const TypeChecker::Error *>(&type);
    REQUIRE(err != nullptr);
}
