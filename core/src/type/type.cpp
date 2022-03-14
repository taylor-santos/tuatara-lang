//
// Created by taylor-santos on 2/7/2022 at 19:22.
//

#include "type/type.hpp"

namespace TypeChecker {

Type::Type(std::optional<yy::location> loc)
    : loc_{loc} {}

Type::~Type() = default;

std::optional<yy::location>
Type::get_loc() const {
    return loc_;
}

std::ostream &
operator<<(std::ostream &os, const Type &type) {
    type.print(os);
    return os;
}

} // namespace TypeChecker
