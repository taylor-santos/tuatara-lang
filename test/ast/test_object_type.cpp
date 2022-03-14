//
// Created by taylor-santos on 2/10/2022 at 16:21.
//

#include "doctest/doctest.h"

#include "type/type_checker.hpp"
#include "type/class.hpp"
#include "type/object.hpp"
#include "ast/object_type.hpp"
#include "printer.hpp"

#include <sstream>

TEST_SUITE_BEGIN("AST/ObjectType");

TEST_CASE("get_type") {
    auto loc    = yy::location();
    auto errors = std::vector<print::Message>();
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
    }
}
