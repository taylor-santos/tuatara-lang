//
// Created by taylor-santos on 2/10/2022 at 17:00.
//

#include "doctest/doctest.h"

#include "location.hh"
#include "ast/variable.hpp"
#include "type/type_checker.hpp"
#include "type/object.hpp"
#include "driver.hpp"

TEST_SUITE_BEGIN("AST/Variable");

TEST_CASE("get_type") {
    auto loc    = yy::location();
    auto errors = std::vector<print::Error>();
    auto ctx    = TypeChecker::Context(errors);

    GIVEN("an initialized variable") {
        auto node     = AST::Variable("foo", loc);
        auto old_type = TypeChecker::Object(TypeChecker::Context::builtins.U64, loc);
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
        auto node     = AST::Variable("foo", loc);
        auto old_type = TypeChecker::Object(TypeChecker::Context::builtins.U64, loc);
        auto old_node = AST::Error(loc);
        ctx.set_symbol(
            "foo",
            old_type,
            TypeChecker::Uninit{TypeChecker::Uninit::Reason::NOT_DEFINED, old_node});

        WHEN("the variable is type checked") {
            auto &type = node.get_type(ctx);

            THEN("the type checker should fail") {
                CHECK(ctx.did_fail());

                AND_THEN("it should still return the correct type") {
                    auto obj = dynamic_cast<const TypeChecker::Object *>(&type);
                    REQUIRE(obj != nullptr);
                    auto &cl = obj->get_class();
                    CHECK(cl.get_name() == "U64");

                    AND_THEN("the correct error message should be produced") {
                        REQUIRE(errors.size() == 1);
                        auto &msg = errors[0].message_;
                        REQUIRE(msg.size() == 2);
                        CHECK(msg[1].message_ == "`foo` used before initialization");
                        auto &details = errors[0].details_;
                        REQUIRE(details.size() == 2);
                        CHECK(
                            details[0].message_ == "`foo` declared without being initialized here");
                        CHECK(details[1].message_ == "`foo` used here");
                    }
                }
            }
        }
    }
    GIVEN("a moved variable") {
        auto node     = AST::Variable("foo", loc);
        auto old_type = TypeChecker::Object(TypeChecker::Context::builtins.U64, loc);
        auto old_node = AST::Error(loc);
        ctx.set_symbol(
            "foo",
            old_type,
            TypeChecker::Uninit{TypeChecker::Uninit::Reason::MOVED_FROM, old_node});

        WHEN("the variable is type checked") {
            auto &type = node.get_type(ctx);

            THEN("the type checker should fail") {
                CHECK(ctx.did_fail());

                AND_THEN("it should still return the correct type") {
                    auto obj = dynamic_cast<const TypeChecker::Object *>(&type);
                    REQUIRE(obj != nullptr);
                    auto &cl = obj->get_class();
                    CHECK(cl.get_name() == "U64");

                    AND_THEN("the correct error message should be produced") {
                        REQUIRE(errors.size() == 1);
                        auto &msg = errors[0].message_;
                        REQUIRE(msg.size() == 2);
                        CHECK(msg[1].message_ == "`foo` used after being moved");
                        auto &details = errors[0].details_;
                        REQUIRE(details.size() == 3);
                        CHECK(details[0].message_ == "value assigned to `foo` here");
                        CHECK(details[1].message_ == "value moved out of `foo` here");
                        CHECK(details[2].message_ == "`foo` used here after move");
                    }
                }
            }
        }
    }
}
