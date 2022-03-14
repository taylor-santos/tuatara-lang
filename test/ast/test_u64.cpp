//
// Created by taylor-santos on 2/10/2022 at 16:03.
//

#include "doctest/doctest.h"

#include "location.hh"
#include "ast/u64.hpp"
#include "type/type_checker.hpp"
#include "type/class.hpp"
#include "type/object.hpp"
#include "printer.hpp"

#include <sstream>

TEST_SUITE_BEGIN("AST/U64");

TEST_CASE("get_type") {
    auto  loc    = yy::location();
    auto  errors = std::vector<print::Message>();
    auto  ctx    = TypeChecker::Context(errors);
    auto  u64    = AST::U64(uint64_t(123), loc);
    auto &type   = u64.get_type(ctx);

    REQUIRE(!ctx.did_fail());
    auto obj = dynamic_cast<const TypeChecker::Object *>(&type);
    REQUIRE(obj != nullptr);
    auto &cl = obj->get_class();
    CHECK(cl.get_name() == "U64");
    CHECK(errors.empty());
}
