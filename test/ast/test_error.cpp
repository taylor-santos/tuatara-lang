//
// Created by taylor-santos on 2/10/2022 at 16:16.
//

#include "doctest/doctest.h"

#include "location.hh"
#include "ast/error.hpp"
#include "type/type_checker.hpp"
#include "type/error.hpp"

#include <sstream>

TEST_CASE("AST Error get_type") {
    auto loc = yy::location{};
    auto u64 = AST::Error(loc);
    auto out = std::ostringstream();
    auto ctx = TypeChecker::Context(out);

    auto &type = u64.get_type(ctx);
    auto  err  = dynamic_cast<const TypeChecker::Error *>(&type);
    REQUIRE(err != nullptr);
    CHECK_MESSAGE(out.str() == "", "Error.get_type() should not report any errors");
}
