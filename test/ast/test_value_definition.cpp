//
// Created by taylor-santos on 2/10/2022 at 16:54.
//

#include "doctest/doctest.h"

#include "location.hh"
#include "ast/value_definition.hpp"
#include "type/type_checker.hpp"
#include "type/class.hpp"
#include "type/object.hpp"
#include "ast/object_type.hpp"
#include "ast/u64.hpp"
#include "ast/variable.hpp"
#include "test_util.hpp"
#include "printer.hpp"

#include <sstream>

TEST_SUITE_BEGIN("AST/ValueDefinition");

TEST_CASE("get_type") {
    auto loc    = yy::location();
    auto errors = std::vector<print::Message>();
    auto ctx    = TypeChecker::Context(errors);

    GIVEN("a new identifier") {
        auto expr = std::make_unique<AST::U64>(123, loc);
        auto node = AST::ValueDefinition("foo", loc, std::move(expr), loc);

        THEN("the node should type check correctly") {
            auto &type = node.get_type(ctx);
            CHECK(!ctx.did_fail());
            auto  obj = dynamic_cast<const TypeChecker::Object *>(&type);
            auto &cl  = obj->get_class();
            CHECK(cl.get_name() == "U64");
            CHECK(errors.empty());
        }
    }
    GIVEN("an existing identifier") {
        auto expr     = std::make_unique<AST::U64>(123, loc);
        auto node     = AST::ValueDefinition("foo", loc, std::move(expr), loc);
        auto old_type = TypeChecker::Object(TypeChecker::Context::builtins.U64, loc);
        ctx.set_symbol("foo", old_type, loc);

        THEN("type checking should fail") {
            auto &type = node.get_type(ctx);
            CHECK(ctx.did_fail());
            REQUIRE(errors.size() == 1);
            auto msg = std::stringstream();
            errors[0].print({""}, msg);
            CHECK(
                msg.str() == "error: variable `foo` already defined\n"
                             "  ╭─[1:1]\n"
                             "1 │ \n"
                             "  · ▲ redefined `foo` here\n"
                             "  · ▲ previous definition of `foo` here\n"
                             "──╯\n");
        }
    }
    GIVEN("an invalid value") {
        auto expr = std::make_unique<AST::Variable>("bar", LOC(nullptr, 1, 8, 1, 11));
        auto node = AST::ValueDefinition("foo", loc, std::move(expr), loc);

        THEN("type checking should fail") {
            auto &type = node.get_type(ctx);
            CHECK(ctx.did_fail());
            REQUIRE(errors.size() == 1);
            auto msg = std::stringstream();
            errors[0].print({""}, msg);
            CHECK(
                msg.str() == "error: `bar` was not declared in this scope\n"
                             "  ╭─[1:8]\n"
                             "1 │ \n"
                             "  ·        ─── `bar` used here\n"
                             "──╯\n");
        }
    }
}
