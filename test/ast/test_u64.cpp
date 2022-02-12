//
// Created by taylor-santos on 2/10/2022 at 16:03.
//

#include "doctest/doctest.h"
#include "location.hh"
#include "ast/u64.hpp"
#include "type/context.hpp"
#include "type/object.hpp"

TEST_CASE("AST U64 get_type") {
    auto loc = yy::location{};
    auto u64 = AST::U64(uint64_t(123), loc);
    auto ctx = TypeChecker::Context();

    auto &type = u64.get_type(ctx);
    auto  obj  = dynamic_cast<const TypeChecker::Object *>(&type);
    REQUIRE(obj != nullptr);
    auto &cl = obj->get_class();
    CHECK(cl.get_name() == "U64");
}
