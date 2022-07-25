//
// Created by taylor-santos on 2/13/2022 at 18:26.
//

#include "doctest/doctest.h"

#include "type/error.hpp"

#include <sstream>

TEST_SUITE_BEGIN("Type/Error");

TEST_CASE("print") {
    auto loc  = yy::location();
    auto node = TypeChecker::Error(loc);
    auto out  = std::stringstream();
    node.print(out, false);
    CHECK(out.str() == "<error>");
}
