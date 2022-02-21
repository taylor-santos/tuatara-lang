//
// Created by taylor-santos on 2/10/2022 at 16:21.
//

#include "doctest/doctest.h"

#include "type/type_checker.hpp"
#include "type/object.hpp"
#include "driver.hpp"

TEST_SUITE_BEGIN("AST/ObjectType");

TEST_CASE("get_type") {
    auto loc    = yy::location();
    auto errors = std::vector<print::Error>();
    auto ctx    = TypeChecker::Context(errors);

    GIVEN("a valid class name") {
        auto node = AST::ObjectType("U64", loc, loc);

        WHEN("the type checker is run") {
            auto &type = node.get_type(ctx);

            THEN("the type checker should succeed") {
                CHECK(!ctx.did_fail());
                CHECK(errors.empty());

                AND_THEN("it should have object type") {
                    auto obj = dynamic_cast<const TypeChecker::Object *>(&type);
                    REQUIRE(obj != nullptr);

                    AND_THEN("the correct class is found") {
                        auto &cl = obj->get_class();
                        CHECK(cl.get_name() == "U64");
                    }
                }
            }
        }
    }
    GIVEN("an invalid class name") {
        auto node = AST::ObjectType("Foo", loc, loc);

        WHEN("the type checker is run") {
            auto &type = node.get_type(ctx);

            THEN("the type checker should fail") {
                CHECK(ctx.did_fail());

                AND_THEN("an error should be emitted") {
                    REQUIRE(errors.size() == 1);
                    auto &msg = errors[0].message_;
                    REQUIRE(msg.size() == 2);
                    CHECK(msg[1].message_ == "`Foo` does not name a type");
                    auto &details = errors[0].details_;
                    REQUIRE(details.size() == 1);
                    CHECK(details[0].message_ == "used here");
                }
            }
        }
    }
}
