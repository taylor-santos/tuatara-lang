//
// Created by taylor-santos on 2/10/2022 at 17:00.
//

#include "doctest/doctest.h"

#include "location.hh"
#include "ast/variable.hpp"
#include "type/type_checker.hpp"
#include "type/object.hpp"

#include <sstream>

TEST_CASE("AST Variable get_type") {
    auto loc      = yy::location{};
    auto node     = AST::Variable("foo", loc);
    auto old_type = TypeChecker::Object(TypeChecker::Context::builtins.U64);
    auto out      = std::ostringstream();
    auto ctx      = TypeChecker::Context(out);
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
        ctx.set_symbol(
            "foo",
            old_type,
            TypeChecker::Uninit{TypeChecker::Uninit::Reason::NOT_DEFINED, node});

        WHEN("the variable is type checked") {
            auto &type = node.get_type(ctx);

            THEN("the type checker should fail") {
                CHECK(ctx.did_fail());
                CHECK(out.str() == "1.1: `foo` used before initialization\n");

                AND_THEN("it should still return the correct type") {
                    auto obj = dynamic_cast<const TypeChecker::Object *>(&type);
                    REQUIRE(obj != nullptr);
                    auto &cl = obj->get_class();
                    CHECK(cl.get_name() == "U64");
                }
            }
        }
    }
    GIVEN("a moved-from variable") {
        ctx.set_symbol(
            "foo",
            old_type,
            TypeChecker::Uninit{TypeChecker::Uninit::Reason::MOVED_FROM, node});

        WHEN("the variable is type checked") {
            auto &type = node.get_type(ctx);

            THEN("the type checker should fail") {
                CHECK(ctx.did_fail());
                CHECK(out.str() == "1.1: `foo` used after being moved\n");

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
