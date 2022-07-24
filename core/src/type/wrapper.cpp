//
// Created by taylor-santos on 7/17/2022 at 15:24.
//

#include "type/wrapper.hpp"

namespace TypeChecker {

Wrapper::Wrapper(const Type &type, yy::location loc)
    : Type(loc)
    , type_{type} {}

Wrapper::Wrapper(const Type &type)
    : Type(type.get_loc())
    , type_{type} {}

Wrapper::~Wrapper() = default;

void
Wrapper::print(std::ostream &os, bool) const {
    type_.print(os);
}

Relation
Wrapper::get_relation(const Type &other) const {
    return type_.compare(other);
}

} // namespace TypeChecker
