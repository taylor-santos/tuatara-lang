//
// Created by taylor-santos on 7/15/2022 at 18:48.
//

#include "type/func.hpp"
#include "type/class.hpp"

namespace TypeChecker {

Func::Func(std::unique_ptr<Tuple> arg_type, std::unique_ptr<Type> ret_type, yy::location loc)
    : Type(loc)
    , arg_type_{std::move(arg_type)}
    , ret_type_{std::move(ret_type)} {}

Func::~Func() = default;

void
Func::print(std::ostream &os) const {
    (void)os;
    // TODO
}

Relation
Func::compare(const Type &other) const {
    const auto *tup_ptr = dynamic_cast<const Func *>(&other);
    if (!tup_ptr) return Relation::UNRELATED;

    return Relation::UNRELATED; // TODO: func relation
}

} // namespace TypeChecker
