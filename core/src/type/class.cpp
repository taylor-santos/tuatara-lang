//
// Created by taylor-santos on 2/7/2022 at 19:24.
//

#include "type/class.hpp"

#include <utility>

namespace TypeChecker {

Class::Class(std::string name, std::optional<yy::location> loc)
    : Type(loc)
    , name_{std::move(name)} {}

Class::~Class() = default;

void
Class::print(std::ostream &os) const {
    os << name_;
}

std::string
Class::get_name() const {
    return name_;
}

Relation
Class::compare(const Type &other) const {
    const auto *cl_ptr = dynamic_cast<const Class *>(&other);
    if (!cl_ptr) return Relation::UNRELATED;

    if (this == cl_ptr) return Relation::SAME_TYPE;
    // TODO Add subtyping support
    return Relation::UNRELATED;
}

} // namespace TypeChecker
