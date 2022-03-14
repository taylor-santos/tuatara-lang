//
// Created by taylor-santos on 2/10/2022 at 17:00.
//

#include "doctest/doctest.h"

#include "location.hh"
#include "ast/variable.hpp"
#include "type/type_checker.hpp"
#include "type/class.hpp"
#include "type/object.hpp"
#include "ast/error.hpp"
#include "printer.hpp"

#include <sstream>

TEST_SUITE_BEGIN("AST/Variable");

TEST_CASE("get_type") {
    auto loc    = yy::location();
    auto errors = std::vector<print::Message>();
    auto ctx    = TypeChecker::Context(errors);

    GIVEN("an initialized variable") {
        auto node     = AST::Variable("foo", loc);
        auto old_type = TypeChecker::Object(TypeChecker::Context::builtins.U64, loc);
        ctx.set_symbol("foo", old_type, loc);

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
        ctx.set_symbol(
            "foo",
            old_type,
            loc,
            TypeChecker::Uninit{TypeChecker::Uninit::Reason::NOT_DEFINED, loc});

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
                        auto msg = std::stringstream();
                        errors[0].print({""}, msg);
                        CHECK(
                            msg.str() == "error: variable `foo` used before initialization\n"
                                         "  ╭─[1:1]\n"
                                         "1 │ \n"
                                         "  · ▲ `foo` used here\n"
                                         "  · ▲ `foo` declared without being initialized here\n"
                                         "──╯\n");
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
            loc,
            TypeChecker::Uninit{TypeChecker::Uninit::Reason::MOVED_FROM, loc});

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
                        auto msg = std::stringstream();
                        errors[0].print({""}, msg);
                        CHECK(
                            msg.str() == "error: variable `foo` used after being moved\n"
                                         "  ╭─[1:1]\n"
                                         "1 │ \n"
                                         "  · ▲ `foo` used here after move\n"
                                         "  · ▲ value moved out of `foo` here\n"
                                         "  · ▲ value assigned to `foo` here\n"
                                         "──╯\n");
                    }
                }
            }
        }
    }
}
