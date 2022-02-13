//
// Created by taylor-santos on 2/10/2022 at 16:31.
//

#include "doctest/doctest.h"

#include "location.hh"
#include "ast/type_definition.hpp"
#include "type/type_checker.hpp"
#include "type/object.hpp"
#include "ast/object_type.hpp"

#include <sstream>

TEST_CASE("AST TypeDefinition get_type") {
    auto loc = yy::location{};
    auto def = std::make_unique<AST::ObjectType>("U64", loc, loc);
    auto out = std::ostringstream();
    GIVEN("a new identifier") {
        auto node = AST::TypeDefinition("foo", loc, std::move(def), loc);
        auto ctx  = TypeChecker::Context(out);

        THEN("the node should type check correctly") {
            auto &type = node.get_type(ctx);
            auto  obj  = dynamic_cast<const TypeChecker::Object *>(&type);
            auto &cl   = obj->get_class();
            CHECK(cl.get_name() == "U64");
        }
    }
    GIVEN("an existing identifier") {
        auto node     = AST::TypeDefinition("foo", loc, std::move(def), loc);
        auto ctx      = TypeChecker::Context(out);
        auto old_type = TypeChecker::Object(TypeChecker::Context::builtins.U64);
        ctx.set_symbol("foo", old_type);

        THEN("type checking should throw an error") {
            CHECK_THROWS((void)node.get_type(ctx));
        }
    }
}
