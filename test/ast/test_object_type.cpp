//
// Created by taylor-santos on 2/10/2022 at 16:21.
//

#include "doctest/doctest.h"

#include "location.hh"
#include "ast/object_type.hpp"
#include "type/context.hpp"
#include "type/object.hpp"

TEST_CASE("AST ObjectType get_type") {
    auto loc = yy::location{};
    GIVEN("a valid class name") {
        auto u64 = AST::ObjectType("U64", loc, loc);
        auto ctx = TypeChecker::Context();

        WHEN("the object is type checked") {
            auto &type = u64.get_type(ctx);
            auto  obj  = dynamic_cast<const TypeChecker::Object *>(&type);

            THEN("it should have object type") {
                REQUIRE(obj != nullptr);

                AND_THEN("the correct class is found") {
                    auto &cl = obj->get_class();
                    CHECK(cl.get_name() == "U64");
                }
            }
        }
    }
    GIVEN("an invalid class name") {
        auto u64 = AST::ObjectType("Foobar", loc, loc);
        auto ctx = TypeChecker::Context();

        WHEN("the object is type checked") {
            THEN("it should throw a type error") {
                CHECK_THROWS((void)u64.get_type(ctx));
            }
        }
    }
}
