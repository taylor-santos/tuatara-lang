//
// Created by taylor-santos on 2/13/2022 at 18:18.
//

#include "doctest/doctest.h"

#include "type/class.hpp"

#include <sstream>

TEST_SUITE_BEGIN("Type/Class");

TEST_CASE("print") {
    auto node = TypeChecker::Class("Foo");
    auto out  = std::stringstream();
    node.print(out, false);
    CHECK(out.str() == "[class Foo]");
}
