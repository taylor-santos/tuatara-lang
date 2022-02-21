//
// Created by taylor-santos on 2/13/2022 at 18:27.
//

#include "doctest/doctest.h"

#include "type/object.hpp"

#include <sstream>

TEST_SUITE_BEGIN("Type/Object");

TEST_CASE("print") {
    auto cl   = TypeChecker::Class("Foo");
    auto loc  = yy::location();
    auto node = TypeChecker::Object(cl, loc);
    auto out  = std::stringstream();
    node.print(out);
    CHECK(out.str() == "[object [class Foo]]");
}
