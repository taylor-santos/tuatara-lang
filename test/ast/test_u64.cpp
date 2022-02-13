//
// Created by taylor-santos on 2/10/2022 at 16:03.
//

#include "doctest/doctest.h"

#include "location.hh"
#include "ast/u64.hpp"
#include "type/type_checker.hpp"
#include "type/object.hpp"

#include <sstream>

TEST_CASE("AST U64 get_type") {
    auto loc = yy::location{};
    auto u64 = AST::U64(uint64_t(123), loc);
    auto out = std::ostringstream();
    auto ctx = TypeChecker::Context(out);

    auto &type = u64.get_type(ctx);
    auto  obj  = dynamic_cast<const TypeChecker::Object *>(&type);
    REQUIRE(obj != nullptr);
    auto &cl = obj->get_class();
    CHECK(cl.get_name() == "U64");
    CHECK(out.str() == "");
}
