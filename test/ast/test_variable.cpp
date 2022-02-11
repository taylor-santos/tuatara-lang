//
// Created by taylor-santos on 2/10/2022 at 17:00.
//

#include "doctest/doctest.h"

#include "location.hh"
#include "ast/variable.hpp"
#include "type/context.hpp"
#include "type/object.hpp"

TEST_CASE("AST Variable get_type") {
    auto loc      = yy::location{};
    auto node     = AST::Variable("foo", loc);
    auto old_type = TypeChecker::Object(TypeChecker::Context::builtins.U64);
    auto ctx      = TypeChecker::Context();
    GIVEN("an initialized variable") {
        ctx.set_symbol("foo", old_type);

        WHEN("the variable is type checked") {
            auto &type = node.get_type(ctx);

            THEN("it should return the correct type") {
                auto obj = dynamic_cast<const TypeChecker::Object *>(&type);
                REQUIRE(obj != nullptr);
                auto &cl = obj->get_class();
                CHECK(cl.get_name() == "U64");
            }
        }
    }
    GIVEN("an uninitialized variable") {
        ctx.set_symbol("foo", old_type, false);

        WHEN("the variable is type checked") {
            auto &type = node.get_type(ctx);

            THEN("the type checker should fail") {
                CHECK(ctx.did_fail());

                AND_THEN("it should still return the correct type") {
                    auto obj = dynamic_cast<const TypeChecker::Object *>(&type);
                    REQUIRE(obj != nullptr);
                    auto &cl = obj->get_class();
                    CHECK(cl.get_name() == "U64");
                }
            }
        }
    }
}
