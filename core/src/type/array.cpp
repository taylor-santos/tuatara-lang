//
// Created by taylor-santos on 7/25/2022 at 14:13.
//

#include "type/array.hpp"
#include "type/class.hpp"

#include <utility>

namespace TypeChecker {

Array::Array(const Type &type, yy::location loc)
    : Type(loc)
    , type_{type} {}

Array::~Array() = default;

void
Array::print(std::ostream &os, bool) const {
    type_.print(os, true);
    os << "[]";
}

const Type &
Array::element_type() const {
    return type_;
}

Relation
Array::get_relation(const Type &other) const {
    auto *arr = dynamic_cast<const TypeChecker::Array *>(&other);
    if (!arr) {
        return Relation::UNRELATED;
    }
    return type_.compare(arr->type_);
}

} // namespace TypeChecker
