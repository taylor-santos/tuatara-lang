//
// Created by taylor-santos on 7/15/2022 at 18:17.
//

#include "type/tuple.hpp"
#include "type/class.hpp"

namespace TypeChecker {

Tuple::Tuple(std::vector<std::unique_ptr<Type>> &&types, yy::location loc)
    : Type(loc)
    , types_{std::move(types)} {}

Tuple::Tuple(yy::location loc)
    : Type(loc)
    , types_{} {}

Tuple::~Tuple() = default;

void
Tuple::print(std::ostream &os) const {
    os << "(";
    std::string sep;
    for (auto &type : types_) {
        os << sep;
        sep = ", ";
        type->print(os);
    }
    os << ")";
}

Relation
Tuple::compare(const Type &other) const {
    const auto *tup_ptr = dynamic_cast<const Tuple *>(&other);
    if (!tup_ptr) return Relation::UNRELATED;

    return Relation::UNRELATED; // TODO: tuple relation
}

} // namespace TypeChecker
