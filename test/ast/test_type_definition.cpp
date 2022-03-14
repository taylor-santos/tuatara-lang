//
// Created by taylor-santos on 2/10/2022 at 16:31.
//

#include "doctest/doctest.h"

#include "location.hh"
#include "ast/type_definition.hpp"
#include "type/type_checker.hpp"
#include "type/class.hpp"
#include "type/object.hpp"
#include "ast/object_type.hpp"
#include "printer.hpp"

#include <sstream>

TEST_SUITE_BEGIN("AST/TypeDefinition");

TEST_CASE("get_type") {
    auto loc    = yy::location();
    auto errors = std::vector<print::Message>();
    auto ctx    = TypeChecker::Context(errors);

    GIVEN("a new identifier") {
        auto expr = std::make_unique<AST::ObjectType>("U64", loc, loc);
        auto node = AST::TypeDefinition("foo", loc, std::move(expr), loc);

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
        auto expr     = std::make_unique<AST::ObjectType>("U64", loc, loc);
        auto node     = AST::TypeDefinition("foo", loc, std::move(expr), loc);
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
    GIVEN("an invalid type") {
        auto expr = std::make_unique<AST::ObjectType>("Foo", loc, loc);
        auto node = AST::TypeDefinition("foo", loc, std::move(expr), loc);

        THEN("type checking should fail") {
            auto &type = node.get_type(ctx);
            CHECK(ctx.did_fail());
            REQUIRE(errors.size() == 1);
            auto msg = std::stringstream();
            errors[0].print({""}, msg);
            CHECK(
                msg.str() == "error: `Foo` does not name a type\n"
                             "  ╭─[1:1]\n"
                             "1 │ \n"
                             "  · ▲ `Foo` used here\n"
                             "──╯\n");
        }
    }
}
