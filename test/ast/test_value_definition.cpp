//
// Created by taylor-santos on 2/10/2022 at 16:54.
//

#include "doctest/doctest.h"

#include "location.hh"
#include "ast/value_definition.hpp"
#include "type/type_checker.hpp"
#include "type/object.hpp"
#include "ast/object_type.hpp"
#include "ast/u64.hpp"
#include "ast/variable.hpp"

#include <sstream>

TEST_CASE("AST ValueDefinition get_type") {
    auto loc = yy::location{};
    auto out = std::ostringstream();
    GIVEN("a new identifier") {
        auto def  = std::make_unique<AST::U64>(123, loc);
        auto node = AST::ValueDefinition("foo", loc, std::move(def), loc);
        auto ctx  = TypeChecker::Context(out);

        THEN("the node should type check correctly") {
            auto &type = node.get_type(ctx);
            auto  obj  = dynamic_cast<const TypeChecker::Object *>(&type);
            auto &cl   = obj->get_class();
            CHECK(cl.get_name() == "U64");
        }
    }
    GIVEN("an existing identifier") {
        auto def      = std::make_unique<AST::U64>(123, loc);
        auto node     = AST::ValueDefinition("foo", loc, std::move(def), loc);
        auto ctx      = TypeChecker::Context(out);
        auto old_type = TypeChecker::Object(TypeChecker::Context::builtins.U64);
        ctx.set_symbol("foo", old_type);

        THEN("type checking should throw an error") {
            CHECK_THROWS((void)node.get_type(ctx));
        }
    }
    GIVEN("an invalid value") {
        auto def  = std::make_unique<AST::Variable>("bar", loc);
        auto node = AST::ValueDefinition("foo", loc, std::move(def), loc);
        auto ctx  = TypeChecker::Context(out);

        THEN("type checking should fail") {
            try {
                (void)node.get_type(ctx);
            } catch (...) {
            }
            CHECK(ctx.did_fail());
            CHECK(out.str() == "1.1: `bar` was not declared in this scope\n");
        }
    }
}
